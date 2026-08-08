# Kafka producer benchmark

> Encoding plus real librdkafka producer handoff. No decoding benchmark is included.

## Run contract

- Configurations: `5`
- Encodes/messages per repetition: `1000000`
- Repetitions per configuration: `10`
- `enqueue` covers serialization plus the librdkafka `produce()` handoff.
- `flush` covers the remaining producer/broker delivery time for the configured acknowledgement mode.
- `end_to_end` is enqueue plus flush and is the relevant result for this producer-path benchmark.

## Results

| Mode | Acks | Compression | Linger ms | Batch messages | Reps | Bytes | Median enqueue ns/msg | Median end-to-end ns/msg | p95 end-to-end ns/msg | p99 end-to-end ns/msg | Errors |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| copy | 0 | lz4 | 0 | 1 | 10 | 49 | 46,172.77 | 50,711.85 | 93,854.69 | 93,854.69 | 0 |
| copy | 0 | none | 0 | 1 | 10 | 49 | 40,755.03 | 45,293.98 | 72,086.57 | 72,086.57 | 0 |
| copy | 0 | zstd | 0 | 1 | 10 | 49 | 47,813.85 | 52,674.56 | 113,391.02 | 113,391.02 | 0 |
| copy | 1 | none | 5 | 100 | 10 | 49 | 594.65 | 666.56 | 2,896.48 | 2,896.48 | 0 |
| owned | 1 | none | 5 | 100 | 10 | 49 | 670.03 | 746.25 | 4,049.69 | 4,049.69 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
