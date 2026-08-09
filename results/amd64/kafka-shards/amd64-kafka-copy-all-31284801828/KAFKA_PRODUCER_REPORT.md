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
| copy | all | lz4 | 0 | 1 | 10 | 49 | 83,314.04 | 92,522.55 | 92,671.96 | 92,671.96 | 0 |
| copy | all | lz4 | 0 | 100 | 10 | 49 | 931.07 | 1,048.57 | 1,586.00 | 1,586.00 | 0 |
| copy | all | lz4 | 5 | 1 | 10 | 49 | 83,050.33 | 92,277.22 | 92,698.29 | 92,698.29 | 0 |
| copy | all | lz4 | 5 | 100 | 10 | 49 | 906.12 | 1,004.00 | 1,175.14 | 1,175.14 | 0 |
| copy | all | none | 0 | 1 | 10 | 49 | 83,593.95 | 92,770.82 | 93,338.88 | 93,338.88 | 0 |
| copy | all | none | 0 | 100 | 10 | 49 | 949.72 | 1,055.95 | 1,515.37 | 1,515.37 | 0 |
| copy | all | none | 5 | 1 | 10 | 49 | 83,713.10 | 93,026.28 | 93,539.15 | 93,539.15 | 0 |
| copy | all | none | 5 | 100 | 10 | 49 | 949.04 | 1,052.10 | 1,192.36 | 1,192.36 | 0 |
| copy | all | zstd | 0 | 1 | 10 | 49 | 85,595.06 | 95,001.87 | 95,802.81 | 95,802.81 | 0 |
| copy | all | zstd | 0 | 100 | 10 | 49 | 1,203.27 | 1,316.17 | 1,462.63 | 1,462.63 | 0 |
| copy | all | zstd | 5 | 1 | 10 | 49 | 85,692.21 | 95,221.87 | 96,141.99 | 96,141.99 | 0 |
| copy | all | zstd | 5 | 100 | 10 | 49 | 1,195.20 | 1,309.41 | 1,507.79 | 1,507.79 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
