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
| one_string_ten_int64 | 82.47 | 82.81 | 85.60 | 91.97 | 55 | 0.42% | 3.81% |
| one_string_ten_decimal_strings | 148.72 | 150.11 | 154.01 | 157.25 | 219 | 0.93% | 3.55% |
| ten_strings_fifty_decimal_strings | 606.25 | 598.85 | 615.76 | 639.88 | 1199 | -1.22% | 1.57% |

## All cached paths

| Payload | API | Reps | Framed bytes | Median ns/encode | Mean M/s | p95 ns |
|---|---|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | cached_id_framed_in_place | 10 | 219 | 150.11 | 6,416,706.02 | 185.72 |
| one_string_ten_decimal_strings | cached_id_framed_with_copy | 10 | 219 | 154.01 | 6,403,819.75 | 170.09 |
| one_string_ten_decimal_strings | cached_serializer_string | 10 | 219 | 157.25 | 6,354,330.16 | 158.35 |
| one_string_ten_int64 | cached_id_framed_in_place | 10 | 55 | 82.81 | 11,930,810.84 | 91.78 |
| one_string_ten_int64 | cached_id_framed_with_copy | 10 | 55 | 85.60 | 11,608,483.02 | 89.08 |
| one_string_ten_int64 | cached_serializer_string | 10 | 55 | 91.97 | 10,740,896.28 | 99.91 |
| ten_strings_fifty_decimal_strings | cached_id_framed_in_place | 10 | 1199 | 598.85 | 1,661,444.47 | 628.00 |
| ten_strings_fifty_decimal_strings | cached_id_framed_with_copy | 10 | 1199 | 615.76 | 1,613,364.86 | 652.91 |
| ten_strings_fifty_decimal_strings | cached_serializer_string | 10 | 1199 | 639.88 | 1,559,939.75 | 671.69 |

## Live Schema Registry paths

These measurements include HTTP request/response and Registry processing. `registry_lookup_keepalive` means a keep-alive HTTP lookup; it is not a local schema-ID cache hit. The local cached-ID paths are the steady-state framing rows above. All live rows are control-plane latency measurements, not per-message serializer benchmarks.

| Path | Reps | Status/result | Median ns | Mean ns | p95 ns | Min ns | Max ns | Requests/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|
| cache_miss_404 | 10 | 404 | 8,605,383.00 | 9,385,837.20 | 14,284,949.00 | 7,043,637.00 | 14,284,949.00 | 106.54 |
| cold_lookup_new_connection | 10 | 1 | 9,630,900.50 | 10,152,786.60 | 14,165,695.00 | 8,293,726.00 | 14,165,695.00 | 98.50 |
| cold_register | 10 | 1 | 17,407,461.00 | 17,537,070.20 | 24,527,372.00 | 14,606,836.00 | 24,527,372.00 | 57.02 |
| concurrent_registration | 10 | 200 | 103,171,448.00 | 100,147,777.50 | 163,825,730.00 | 40,981,152.00 | 163,825,730.00 | 9.99 |
| registry_lookup_keepalive | 10 | 1 | 3,858,081.00 | 6,444,232.00 | 28,703,278.00 | 2,654,933.00 | 28,703,278.00 | 155.18 |
| registry_unavailable | 10 | 0 | 165,214.50 | 286,914.90 | 1,031,372.00 | 134,589.00 | 1,031,372.00 | 3,485.35 |
| retry_failure_then_success | 10 | 200 | 7,746,330.00 | 7,720,654.30 | 9,870,868.00 | 6,567,078.00 | 9,870,868.00 | 129.52 |

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
