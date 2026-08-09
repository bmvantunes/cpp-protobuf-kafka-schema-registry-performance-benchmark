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
| copy | 1 | lz4 | 0 | 1 | 10 | 49 | 93,591.65 | 103,962.90 | 105,258.88 | 105,258.88 | 0 |
| copy | 1 | lz4 | 0 | 100 | 10 | 49 | 1,005.13 | 1,115.97 | 1,525.15 | 1,525.15 | 0 |
| copy | 1 | lz4 | 5 | 1 | 10 | 49 | 94,119.34 | 104,678.69 | 105,504.24 | 105,504.24 | 0 |
| copy | 1 | lz4 | 5 | 100 | 10 | 49 | 1,021.19 | 1,134.16 | 1,282.97 | 1,282.97 | 0 |
| copy | 1 | none | 0 | 1 | 10 | 49 | 91,524.10 | 101,600.19 | 106,112.33 | 106,112.33 | 0 |
| copy | 1 | none | 0 | 100 | 10 | 49 | 1,060.69 | 1,175.25 | 1,907.19 | 1,907.19 | 0 |
| copy | 1 | none | 5 | 1 | 10 | 49 | 92,521.37 | 102,722.99 | 103,139.72 | 103,139.72 | 0 |
| copy | 1 | none | 5 | 100 | 10 | 49 | 1,046.91 | 1,159.99 | 1,288.02 | 1,288.02 | 0 |
| copy | 1 | zstd | 0 | 1 | 10 | 49 | 94,817.16 | 105,539.49 | 105,692.25 | 105,692.25 | 0 |
| copy | 1 | zstd | 0 | 100 | 10 | 49 | 1,275.21 | 1,397.17 | 1,571.60 | 1,571.60 | 0 |
| copy | 1 | zstd | 5 | 1 | 10 | 49 | 95,358.47 | 105,961.44 | 107,023.60 | 107,023.60 | 0 |
| copy | 1 | zstd | 5 | 100 | 10 | 49 | 1,278.69 | 1,402.90 | 1,491.49 | 1,491.49 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
