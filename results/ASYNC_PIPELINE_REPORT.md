# Asynchronous hot-path pipeline benchmark

> Producer-side measurements cover the non-blocking event handoff. Protobuf and pipe formatting measurements cover worker-side work. No decoding, broker, or network work is included.

## Run contract

- Iterations per row: `1000000`
- Measured repetitions: `10`
- Warmup iterations: `10000`
- Queue capacity: `1048576` pointer slots
- Handoff model: `preallocated_immutable_event_pool_pointer`

The queue is preallocated larger than one measured repetition so the producer never waits for the worker during the baseline handoff test. A dropped count is still reported and must be zero for a valid throughput row.

## Results

| Mode | Test case | Completed | Dropped | Payload + header bytes | Hot handoff p50 ns | Hot handoff p99 ns | Worker p50 ns | Worker p99 ns | Worker wall M/s |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|
| async_both | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 41.00 | 83.00 | 333.00 | 354.50 | 2.88 |
| async_both | one_string_ten_int64 | 1,000,000 | 0 | 55 | 41.00 | 83.00 | 250.00 | 250.00 | 3.78 |
| async_both | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 41.00 | 42.00 | 1,250.00 | 1,375.00 | 0.78 |
| async_handoff | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 83.00 | 84.00 | 0.00 | 42.00 | 8.36 |
| async_handoff | one_string_ten_int64 | 1,000,000 | 0 | 55 | 42.00 | 84.00 | 0.00 | 42.00 | 11.60 |
| async_handoff | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 83.00 | 84.00 | 0.00 | 42.00 | 8.31 |
| async_pipe | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 42.00 | 83.50 | 83.00 | 125.00 | 7.41 |
| async_pipe | one_string_ten_int64 | 1,000,000 | 0 | 55 | 41.00 | 84.00 | 125.00 | 167.00 | 6.08 |
| async_pipe | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 41.00 | 83.00 | 250.00 | 292.00 | 3.37 |
| async_protobuf | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 41.00 | 42.00 | 250.00 | 292.00 | 3.44 |
| async_protobuf | one_string_ten_int64 | 1,000,000 | 0 | 55 | 41.00 | 83.00 | 125.00 | 167.00 | 6.19 |
| async_protobuf | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 41.00 | 42.00 | 1,000.00 | 1,166.50 | 0.93 |
| sync_both | one_string_ten_decimal_strings | 1,000,000 | 0 | 219 | 312.50 | 354.50 | 312.50 | 354.50 | 2.98 |
| sync_both | one_string_ten_int64 | 1,000,000 | 0 | 55 | 209.00 | 250.00 | 209.00 | 250.00 | 3.98 |
| sync_both | ten_strings_fifty_decimal_strings | 1,000,000 | 0 | 1199 | 1,229.00 | 1,354.50 | 1,229.00 | 1,354.50 | 0.79 |

## Mode definitions

- `sync_both`: control path; the producer performs Protobuf framing and pipe formatting itself.
- `async_handoff`: producer performs only the non-blocking ring handoff; the worker drains the queue without serialization work.
- `async_protobuf`: producer hands off; the worker populates the generated message and serializes the six-byte Confluent-framed payload.
- `async_pipe`: producer hands off; the worker formats the pipe-delimited line into a reusable buffer.
- `async_both`: producer hands off; the worker performs both Protobuf serialization and pipe formatting.

## Interpretation

The number to protect on the trading thread is hot handoff p50/p99, not worker encoding time. The worker rows answer whether the encoder can keep up with the event rate. `async_handoff` isolates the queue cost; `async_both` is the relevant design when one worker creates both Kafka bytes and human-readable log lines.

The benchmark uses an immutable event-pool pointer handoff to model transferring ownership from a pool without hot-thread allocation or variable-size copying. A production implementation that copies a fixed-size event into the ring should be benchmarked as a separate queue variant because its cost depends on the exact event layout and string storage strategy.

A non-zero dropped count means the configured worker/queue combination did not sustain the attempted event rate. It is not a successful 1M-encoding result and must be investigated rather than averaged away.
