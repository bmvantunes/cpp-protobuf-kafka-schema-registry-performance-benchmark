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
| copy | 0 | lz4 | 0 | 1 | 10 | 49 | 43,196.86 | 47,993.00 | 48,186.16 | 48,186.16 | 0 |
| copy | 0 | lz4 | 0 | 100 | 10 | 49 | 724.19 | 725.14 | 882.65 | 882.65 | 0 |
| copy | 0 | lz4 | 5 | 1 | 10 | 49 | 43,611.13 | 48,417.62 | 48,583.95 | 48,583.95 | 0 |
| copy | 0 | lz4 | 5 | 100 | 10 | 49 | 675.83 | 692.75 | 828.04 | 828.04 | 0 |
| copy | 0 | none | 0 | 1 | 10 | 49 | 40,814.51 | 45,322.76 | 47,002.76 | 47,002.76 | 0 |
| copy | 0 | none | 0 | 100 | 10 | 49 | 605.79 | 647.77 | 944.52 | 944.52 | 0 |
| copy | 0 | none | 5 | 1 | 10 | 49 | 41,739.30 | 46,360.21 | 46,796.91 | 46,796.91 | 0 |
| copy | 0 | none | 5 | 100 | 10 | 49 | 683.30 | 684.27 | 782.77 | 782.77 | 0 |
| copy | 0 | zstd | 0 | 1 | 10 | 49 | 47,164.52 | 52,352.35 | 52,534.91 | 52,534.91 | 0 |
| copy | 0 | zstd | 0 | 100 | 10 | 49 | 1,068.33 | 1,159.12 | 1,217.06 | 1,217.06 | 0 |
| copy | 0 | zstd | 5 | 1 | 10 | 49 | 47,055.55 | 52,268.49 | 52,728.29 | 52,728.29 | 0 |
| copy | 0 | zstd | 5 | 100 | 10 | 49 | 1,122.91 | 1,215.73 | 1,314.21 | 1,314.21 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
