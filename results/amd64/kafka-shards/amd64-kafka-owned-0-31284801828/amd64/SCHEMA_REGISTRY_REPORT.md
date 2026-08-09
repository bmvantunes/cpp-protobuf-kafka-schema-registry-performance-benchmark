# Confluent Schema Registry and Kafka framing benchmark

> Detailed report: pure protobuf encoding, cached Confluent framing, allocation/copy variants, and live Schema Registry HTTP paths.

## Executive verdict

For an HFT producer, Schema Registry is acceptable only when the schema ID is acquired before the hot path and cached locally. The steady-state framing work is a fixed byte-prefix operation; registration and network lookups are separate millisecond-scale control-plane operations and must not occur per message.

The benchmark uses a six-byte Protobuf Confluent prefix for a single top-level message: one magic byte, four big-endian schema-ID bytes, and the one-byte message-index encoding for index zero. The exact framing is included in the measured output size.

## Run contract

- Steady-state encodes per repetition: `1000000`
- Steady-state repetitions: `10`
- Excluded warmup encodes: `10000`
- Cold/control-plane repetitions: `10` requests per path
- Cold/control-plane paths intentionally use one HTTP request per repetition; sending one million live registration requests would benchmark Registry stress and storage behavior, not a production encode path.
- All work runs in Docker; the Registry is Confluent Schema Registry backed by Confluent Kafka in single-node KRaft mode.
- Decimal values remain exact strings in both protobuf and JSON payloads.

## Steady-state result versus pure protobuf

The pure baseline is Google protobuf generated with Buf using `SPEED` and preallocated `SerializeToArray`, matching the message implementation used by the Registry framing executable.

| Payload | Pure protobuf ns | In-place framed ns | Framed + copy ns | Cached serializer string ns | Framed bytes | In-place overhead | Copy overhead |
|---|---:|---:|---:|---:|---:|---:|---:|
| one_string_ten_int64 | 56.13 | 57.17 | 59.18 | 65.14 | 55 | 1.85% | 5.44% |
| one_string_ten_decimal_strings | 161.64 | 164.46 | 168.76 | 177.55 | 219 | 1.74% | 4.40% |
| ten_strings_fifty_decimal_strings | 904.73 | 897.20 | 920.17 | 935.16 | 1199 | -0.83% | 1.71% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 164.46 | 6,054,687.15 | 170.20 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 168.76 | 5,929,801.82 | 170.16 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 177.55 | 5,637,296.35 | 179.09 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 57.17 | 17,548,262.11 | 60.87 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 59.18 | 16,829,605.30 | 61.07 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 65.14 | 15,337,141.03 | 66.30 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 897.20 | 1,114,032.85 | 909.46 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 920.17 | 1,086,569.61 | 929.36 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 935.16 | 1,070,257.82 | 938.95 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 3,035,841.00 | 4,312,432.60 | 12,198,970.00 | 2,360,504.00 | 12,198,970.00 | 231.89 |
| cold_lookup_new_connection | 10 | 1 | 3,710,801.50 | 4,163,718.40 | 9,112,567.00 | 2,878,328.00 | 9,112,567.00 | 240.17 |
| cold_register | 10 | 1 | 17,757,481.00 | 18,455,193.30 | 27,027,036.00 | 14,254,120.00 | 27,027,036.00 | 54.19 |
| concurrent_registration | 10 | 200 | 99,270,014.00 | 96,031,441.40 | 149,898,614.00 | 27,358,194.00 | 149,898,614.00 | 10.41 |
| registry_lookup_keepalive | 10 | 1 | 2,981,710.50 | 4,176,344.40 | 11,265,721.00 | 2,623,313.00 | 11,265,721.00 | 239.44 |
| registry_unavailable | 10 | 0 | 458,754.00 | 1,343,060.10 | 3,282,424.00 | 250,895.00 | 3,282,424.00 | 744.57 |
| retry_failure_then_success | 10 | 200 | 5,723,839.00 | 5,313,711.50 | 7,912,594.00 | 3,077,377.00 | 7,912,594.00 | 188.19 |

## HFT interpretation

1. Register or resolve the schema ID during process startup, deployment, or a controlled recovery path.
2. Keep the schema ID in an immutable/read-mostly local cache. A cache miss must fail closed or use an explicitly non-HFT recovery path, not synchronously call Registry from the producer hot loop.
3. Prefer serializing directly into a buffer with reserved prefix space. That avoids a second payload copy and makes the six-byte framing cost visible and bounded.
4. Treat serializer-string paths as a convenience path, not the default HFT path; their allocation/copy behavior is visible in the cached-path table.
5. Registry availability is still operationally important even when it is absent from the hot path: startup, schema rollout, failover, and cache invalidation need timeouts, metrics, and a tested fallback policy.
6. Kafka compression, batching, broker acknowledgements, and network transport remain outside this benchmark and should be tested separately once the serializer/framing choice is fixed.

## Scope boundary

The companion `REPORT.md` contains the complete pure protobuf/JSON library matrix, including every Buf-generated protobuf variant and every JSON library. This Registry report intentionally uses the matching Google protobuf `SPEED` type for its wire-framing and Registry-path comparison; changing generated code and changing Registry integration at the same time would make the framing result harder to attribute.

## Reproducibility and raw data

- `schema_registry_raw.csv` contains every cached-path repetition and every measured HTTP request.
- The existing `raw.csv` is the pure protobuf/JSON baseline used for the comparison.
- Re-run on an isolated CPU when comparing small differences; CPU frequency, emulation mode, allocator state, and container host scheduling affect absolute values.
