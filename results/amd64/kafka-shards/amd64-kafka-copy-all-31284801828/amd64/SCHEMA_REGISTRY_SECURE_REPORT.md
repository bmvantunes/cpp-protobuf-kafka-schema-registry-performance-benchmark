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
| one_string_ten_int64 | 56.13 | 57.14 | 60.70 | 65.42 | 55 | 1.79% | 8.15% |
| one_string_ten_decimal_strings | 161.64 | 162.50 | 166.40 | 175.68 | 219 | 0.53% | 2.94% |
| ten_strings_fifty_decimal_strings | 904.73 | 902.58 | 922.61 | 931.33 | 1199 | -0.24% | 1.98% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 162.50 | 6,163,825.86 | 163.40 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 166.40 | 5,998,138.18 | 169.08 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 175.68 | 5,695,866.91 | 178.01 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 57.14 | 17,545,090.88 | 58.51 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 60.70 | 16,467,194.06 | 62.35 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 65.42 | 15,291,384.99 | 66.33 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 902.58 | 1,107,987.08 | 907.90 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 922.61 | 1,082,844.30 | 935.35 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 931.33 | 1,073,554.82 | 936.71 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 10,882,515.00 | 11,570,972.80 | 19,232,759.00 | 8,308,853.00 | 19,232,759.00 | 86.42 |
| cold_lookup_new_connection | 10 | 1 | 15,858,214.00 | 15,782,154.20 | 18,325,800.00 | 13,455,685.00 | 18,325,800.00 | 63.36 |
| cold_register | 10 | 1 | 17,231,843.50 | 18,870,405.30 | 31,627,368.00 | 14,408,122.00 | 31,627,368.00 | 52.99 |
| concurrent_registration | 10 | 200 | 119,109,780.00 | 118,457,359.80 | 184,805,953.00 | 46,503,186.00 | 184,805,953.00 | 8.44 |
| registry_lookup_keepalive | 10 | 1 | 5,014,369.00 | 8,705,215.00 | 40,706,901.00 | 3,355,987.00 | 40,706,901.00 | 114.87 |
| registry_unavailable | 10 | 0 | 454,790.50 | 1,094,973.20 | 2,723,178.00 | 245,171.00 | 2,723,178.00 | 913.26 |
| retry_failure_then_success | 10 | 200 | 12,411,743.50 | 12,437,988.50 | 16,016,312.00 | 9,568,411.00 | 16,016,312.00 | 80.40 |

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
