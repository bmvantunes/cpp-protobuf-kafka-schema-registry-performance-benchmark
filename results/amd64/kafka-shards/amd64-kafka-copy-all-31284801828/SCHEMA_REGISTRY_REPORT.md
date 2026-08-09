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
| one_string_ten_int64 | 82.47 | 83.41 | 85.56 | 90.82 | 55 | 1.15% | 3.75% |
| one_string_ten_decimal_strings | 148.72 | 147.71 | 151.41 | 156.85 | 219 | -0.68% | 1.81% |
| ten_strings_fifty_decimal_strings | 606.25 | 597.42 | 713.14 | 738.36 | 1199 | -1.46% | 17.63% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 147.71 | 6,767,525.35 | 148.55 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 151.41 | 6,483,116.67 | 170.68 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 156.85 | 6,291,349.90 | 164.75 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 83.41 | 11,858,457.45 | 87.67 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 85.56 | 11,681,656.18 | 86.66 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 90.82 | 10,993,311.67 | 91.97 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 597.42 | 1,655,487.16 | 657.37 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 713.14 | 1,167,175.75 | 2,046.86 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 738.36 | 1,360,382.82 | 837.38 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 1,940,320.00 | 2,671,896.60 | 8,158,831.00 | 1,712,335.00 | 8,158,831.00 | 374.27 |
| cold_lookup_new_connection | 10 | 1 | 2,569,377.50 | 2,605,415.90 | 3,270,499.00 | 2,139,721.00 | 3,270,499.00 | 383.82 |
| cold_register | 10 | 1 | 15,134,361.50 | 15,345,171.40 | 17,450,067.00 | 13,666,637.00 | 17,450,067.00 | 65.17 |
| concurrent_registration | 10 | 200 | 86,461,144.50 | 88,113,427.70 | 144,041,267.00 | 36,821,014.00 | 144,041,267.00 | 11.35 |
| registry_lookup_keepalive | 10 | 1 | 2,654,525.50 | 3,506,442.80 | 11,001,362.00 | 1,638,208.00 | 11,001,362.00 | 285.19 |
| registry_unavailable | 10 | 0 | 203,380.00 | 240,097.80 | 603,974.00 | 158,296.00 | 603,974.00 | 4,164.97 |
| retry_failure_then_success | 10 | 200 | 2,204,785.00 | 2,482,200.40 | 4,500,239.00 | 1,938,633.00 | 4,500,239.00 | 402.87 |

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
