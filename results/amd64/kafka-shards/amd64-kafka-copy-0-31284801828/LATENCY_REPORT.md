# Per-encode latency and allocation benchmark

> Every percentile below is calculated from one million individually timed encodes per repetition. No decoding or network work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- Warmup encodes excluded: `10000`
- Instrumentation uses a timer around every encode, so these are instrumented latency measurements and should be compared within this report, not directly substituted for the aggregate throughput benchmark.

## Results

| Payload | Path | Reps | Bytes | Median p50 ns | p90 ns | p99 ns | p99.9 ns | p99.99 ns | Max ns | Mean allocations/rep | Mean allocated bytes/rep |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| one_string_ten_decimal_strings | serialize_array_arena_message | 10 | 213 | 167.00 | 208.00 | 209.00 | 250.50 | 6,896.00 | 88,502.00 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_array_fresh_buffer | 10 | 213 | 208.00 | 209.00 | 229.50 | 250.00 | 5,896.00 | 56,918.00 | 1,000,000.00 | 213,000,000.00 |
| one_string_ten_decimal_strings | serialize_array_reuse | 10 | 213 | 167.00 | 187.50 | 209.00 | 209.00 | 5,354.50 | 33,730.00 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_string_reserved | 10 | 213 | 167.00 | 208.00 | 209.00 | 209.00 | 4,145.50 | 36,355.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_arena_message | 10 | 49 | 125.00 | 125.00 | 125.00 | 126.00 | 2,958.00 | 25,250.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_fresh_buffer | 10 | 49 | 125.00 | 166.00 | 167.00 | 167.00 | 2,958.50 | 40,230.00 | 1,000,000.00 | 49,000,000.00 |
| one_string_ten_int64 | serialize_array_reuse | 10 | 49 | 125.00 | 125.00 | 125.00 | 126.00 | 2,958.00 | 30,834.00 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_string_reserved | 10 | 49 | 125.00 | 125.00 | 125.00 | 167.00 | 3,000.00 | 31,688.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_arena_message | 10 | 1193 | 625.00 | 625.00 | 708.00 | 2,729.00 | 12,917.00 | 119,190.50 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_fresh_buffer | 10 | 1193 | 667.00 | 708.00 | 750.00 | 3,416.50 | 20,563.00 | 771,019.00 | 1,000,000.00 | 1,193,000,000.00 |
| ten_strings_fifty_decimal_strings | serialize_array_reuse | 10 | 1193 | 625.00 | 625.00 | 667.00 | 2,417.00 | 13,417.50 | 103,482.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_string_reserved | 10 | 1193 | 666.50 | 667.00 | 1,604.50 | 5,229.50 | 51,835.00 | 4,158,168.00 | 0.00 | 0.00 |

## Allocation interpretation

- `serialize_array_reuse` measures a caller-owned buffer reused across encodes.
- `serialize_array_fresh_buffer` intentionally allocates a new output vector for each encode and exposes allocator cost.
- `serialize_string_reserved` reuses a reserved string, while `serialize_array_arena_message` serializes a message allocated on a protobuf Arena.
- Allocation counts include allocations observed by the benchmark process during the timed loop; they are not a decoding metric.
