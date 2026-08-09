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
| one_string_ten_int64 | 82.52 | 83.62 | 85.86 | 91.93 | 55 | 1.33% | 4.05% |
| one_string_ten_decimal_strings | 203.28 | 150.16 | 152.65 | 160.72 | 219 | -26.13% | -24.91% |
| ten_strings_fifty_decimal_strings | 600.22 | 608.63 | 619.08 | 628.22 | 1199 | 1.40% | 3.14% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 150.16 | 6,662,145.29 | 151.82 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 152.65 | 6,533,398.41 | 156.15 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 160.72 | 6,176,183.03 | 168.46 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 83.62 | 11,940,498.11 | 85.87 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 85.86 | 11,537,897.96 | 90.92 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 91.93 | 10,598,081.54 | 111.46 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 608.63 | 1,619,626.24 | 673.89 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 619.08 | 1,615,933.10 | 622.98 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 628.22 | 1,568,795.28 | 710.98 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 2,006,081.00 | 2,455,819.60 | 6,835,914.00 | 1,778,595.00 | 6,835,914.00 | 407.20 |
| cold_lookup_new_connection | 10 | 1 | 2,845,064.50 | 3,037,162.00 | 4,479,634.00 | 2,475,616.00 | 4,479,634.00 | 329.25 |
| cold_register | 10 | 1 | 15,531,444.50 | 15,823,116.20 | 19,680,757.00 | 13,969,586.00 | 19,680,757.00 | 63.20 |
| concurrent_registration | 10 | 200 | 70,311,588.50 | 76,565,630.10 | 140,244,540.00 | 17,400,564.00 | 140,244,540.00 | 13.06 |
| registry_lookup_keepalive | 10 | 1 | 2,162,481.50 | 3,183,074.60 | 10,649,778.00 | 2,011,769.00 | 10,649,778.00 | 314.16 |
| registry_unavailable | 10 | 0 | 160,713.50 | 178,013.90 | 350,011.00 | 141,879.00 | 350,011.00 | 5,617.54 |
| retry_failure_then_success | 10 | 200 | 2,454,365.00 | 2,412,326.50 | 2,571,494.00 | 2,187,607.00 | 2,571,494.00 | 414.54 |

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
