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
| owned | all | lz4 | 0 | 1 | 10 | 49 | 94,228.73 | 104,582.21 | 105,826.53 | 105,826.53 | 0 |
| owned | all | lz4 | 0 | 100 | 10 | 49 | 1,082.14 | 1,225.26 | 1,593.93 | 1,593.93 | 0 |
| owned | all | lz4 | 5 | 1 | 10 | 49 | 93,904.05 | 104,492.32 | 110,636.89 | 110,636.89 | 0 |
| owned | all | lz4 | 5 | 100 | 10 | 49 | 1,012.87 | 1,118.62 | 1,303.56 | 1,303.56 | 0 |
| owned | all | none | 0 | 1 | 10 | 49 | 92,462.91 | 102,672.69 | 104,498.42 | 104,498.42 | 0 |
| owned | all | none | 0 | 100 | 10 | 49 | 1,016.88 | 1,131.60 | 1,608.14 | 1,608.14 | 0 |
| owned | all | none | 5 | 1 | 10 | 49 | 93,600.10 | 104,009.68 | 105,270.95 | 105,270.95 | 0 |
| owned | all | none | 5 | 100 | 10 | 49 | 1,033.37 | 1,148.06 | 1,260.20 | 1,260.20 | 0 |
| owned | all | zstd | 0 | 1 | 10 | 49 | 94,498.69 | 104,894.11 | 109,252.23 | 109,252.23 | 0 |
| owned | all | zstd | 0 | 100 | 10 | 49 | 1,297.54 | 1,422.76 | 1,691.46 | 1,691.46 | 0 |
| owned | all | zstd | 5 | 1 | 10 | 49 | 94,845.61 | 105,322.26 | 107,300.97 | 107,300.97 | 0 |
| owned | all | zstd | 5 | 100 | 10 | 49 | 1,480.15 | 1,608.82 | 1,661.49 | 1,661.49 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
