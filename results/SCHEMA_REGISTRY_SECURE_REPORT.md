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
| one_string_ten_int64 | 82.52 | 82.98 | 87.42 | 90.95 | 55 | 0.56% | 5.94% |
| one_string_ten_decimal_strings | 203.28 | 148.73 | 155.29 | 157.75 | 219 | -26.84% | -23.61% |
| ten_strings_fifty_decimal_strings | 600.22 | 597.80 | 612.19 | 628.60 | 1199 | -0.40% | 1.99% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 148.73 | 6,726,492.09 | 150.18 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 155.29 | 6,037,991.04 | 200.76 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 157.75 | 6,302,873.23 | 162.31 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 82.98 | 11,985,377.84 | 85.73 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 87.42 | 11,303,586.06 | 95.03 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 90.95 | 10,964,010.64 | 93.72 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 597.80 | 1,671,323.25 | 601.70 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 612.19 | 1,614,544.59 | 673.66 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 628.60 | 1,566,003.93 | 696.17 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 6,941,812.50 | 7,768,641.40 | 14,276,220.00 | 6,028,848.00 | 14,276,220.00 | 128.72 |
| cold_lookup_new_connection | 10 | 1 | 10,531,336.50 | 10,626,452.20 | 13,691,286.00 | 9,053,689.00 | 13,691,286.00 | 94.10 |
| cold_register | 10 | 1 | 15,910,310.50 | 17,045,548.80 | 26,011,489.00 | 14,269,719.00 | 26,011,489.00 | 58.67 |
| concurrent_registration | 10 | 200 | 91,220,799.50 | 88,092,621.90 | 137,813,218.00 | 35,376,686.00 | 137,813,218.00 | 11.35 |
| registry_lookup_keepalive | 10 | 1 | 3,849,698.50 | 10,024,759.10 | 64,320,096.00 | 2,705,081.00 | 64,320,096.00 | 99.75 |
| registry_unavailable | 10 | 0 | 146,379.50 | 163,313.60 | 338,761.00 | 115,504.00 | 338,761.00 | 6,123.19 |
| retry_failure_then_success | 10 | 200 | 7,671,396.50 | 7,630,116.50 | 8,898,559.00 | 6,486,778.00 | 8,898,559.00 | 131.06 |

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
