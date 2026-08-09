# Kafka producer benchmark

> Encoding plus real librdkafka producer handoff. No decoding benchmark is included.

## Run contract

- Configurations: `12`
- Encodes/messages per repetition: `1000000`
- Repetitions per configuration: `10`
- `enqueue` covers serialization plus the librdkafka `produce()` handoff.
- `flush` covers the remaining producer/broker delivery time for the configured acknowledgement mode.
- `end_to_end` is enqueue plus flush and is the relevant result for this producer-path benchmark.

## Results

| Mode | Acks | Compression | Linger ms | Batch messages | Reps | Bytes | Median enqueue ns/msg | Median end-to-end ns/msg | p95 end-to-end ns/msg | p99 end-to-end ns/msg | Errors |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| owned | 0 | lz4 | 0 | 1 | 10 | 49 | 40,994.95 | 45,515.14 | 46,407.06 | 46,407.06 | 0 |
| owned | 0 | lz4 | 0 | 100 | 10 | 49 | 890.59 | 932.03 | 1,053.75 | 1,053.75 | 0 |
| owned | 0 | lz4 | 5 | 1 | 10 | 49 | 39,926.62 | 44,331.91 | 45,591.13 | 45,591.13 | 0 |
| owned | 0 | lz4 | 5 | 100 | 10 | 49 | 853.70 | 854.58 | 930.24 | 930.24 | 0 |
| owned | 0 | none | 0 | 1 | 10 | 49 | 39,834.49 | 44,345.07 | 46,087.07 | 46,087.07 | 0 |
| owned | 0 | none | 0 | 100 | 10 | 49 | 919.65 | 931.00 | 1,056.42 | 1,056.42 | 0 |
| owned | 0 | none | 5 | 1 | 10 | 49 | 40,193.93 | 44,631.63 | 45,210.82 | 45,210.82 | 0 |
| owned | 0 | none | 5 | 100 | 10 | 49 | 863.01 | 863.09 | 916.88 | 916.88 | 0 |
| owned | 0 | zstd | 0 | 1 | 10 | 49 | 41,924.53 | 46,618.76 | 46,951.40 | 46,951.40 | 0 |
| owned | 0 | zstd | 0 | 100 | 10 | 49 | 1,062.25 | 1,143.43 | 1,244.23 | 1,244.23 | 0 |
| owned | 0 | zstd | 5 | 1 | 10 | 49 | 43,224.49 | 48,006.02 | 48,673.25 | 48,673.25 | 0 |
| owned | 0 | zstd | 5 | 100 | 10 | 49 | 1,040.22 | 1,126.30 | 1,232.22 | 1,232.22 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
