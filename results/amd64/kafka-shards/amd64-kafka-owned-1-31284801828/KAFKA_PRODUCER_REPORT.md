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
| owned | 1 | lz4 | 0 | 1 | 10 | 49 | 90,829.71 | 100,956.36 | 101,587.15 | 101,587.15 | 0 |
| owned | 1 | lz4 | 0 | 100 | 10 | 49 | 1,162.59 | 1,335.27 | 1,755.40 | 1,755.40 | 0 |
| owned | 1 | lz4 | 5 | 1 | 10 | 49 | 89,729.49 | 99,702.18 | 100,040.57 | 100,040.57 | 0 |
| owned | 1 | lz4 | 5 | 100 | 10 | 49 | 948.71 | 1,054.32 | 1,220.69 | 1,220.69 | 0 |
| owned | 1 | none | 0 | 1 | 10 | 49 | 89,380.71 | 99,251.08 | 101,590.37 | 101,590.37 | 0 |
| owned | 1 | none | 0 | 100 | 10 | 49 | 1,005.22 | 1,120.29 | 1,587.75 | 1,587.75 | 0 |
| owned | 1 | none | 5 | 1 | 10 | 49 | 90,827.67 | 100,949.14 | 101,471.56 | 101,471.56 | 0 |
| owned | 1 | none | 5 | 100 | 10 | 49 | 1,005.85 | 1,118.83 | 1,241.80 | 1,241.80 | 0 |
| owned | 1 | zstd | 0 | 1 | 10 | 49 | 91,060.33 | 101,153.56 | 101,811.41 | 101,811.41 | 0 |
| owned | 1 | zstd | 0 | 100 | 10 | 49 | 1,253.80 | 1,374.97 | 1,580.88 | 1,580.88 | 0 |
| owned | 1 | zstd | 5 | 1 | 10 | 49 | 91,774.55 | 101,939.78 | 103,132.73 | 103,132.73 | 0 |
| owned | 1 | zstd | 5 | 100 | 10 | 49 | 1,361.79 | 1,489.75 | 1,599.89 | 1,599.89 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
