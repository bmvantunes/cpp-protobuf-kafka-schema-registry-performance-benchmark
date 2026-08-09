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
| one_string_ten_decimal_strings | serialize_array_arena_message | 10 | 213 | 167.00 | 208.00 | 209.00 | 209.00 | 4,833.50 | 37,022.00 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_array_fresh_buffer | 10 | 213 | 208.00 | 209.00 | 209.00 | 250.00 | 6,250.00 | 59,252.00 | 1,000,000.00 | 213,000,000.00 |
| one_string_ten_decimal_strings | serialize_array_reuse | 10 | 213 | 167.00 | 208.00 | 209.00 | 209.00 | 3,479.00 | 46,501.50 | 0.00 | 0.00 |
| one_string_ten_decimal_strings | serialize_string_reserved | 10 | 213 | 167.00 | 209.00 | 209.00 | 250.00 | 6,208.50 | 30,979.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_arena_message | 10 | 49 | 125.00 | 125.00 | 125.00 | 166.00 | 2,979.50 | 39,501.50 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_array_fresh_buffer | 10 | 49 | 125.00 | 167.00 | 167.00 | 167.00 | 3,312.50 | 57,231.00 | 1,000,000.00 | 49,000,000.00 |
| one_string_ten_int64 | serialize_array_reuse | 10 | 49 | 125.00 | 125.00 | 125.00 | 166.00 | 3,000.50 | 38,980.00 | 0.00 | 0.00 |
| one_string_ten_int64 | serialize_string_reserved | 10 | 49 | 125.00 | 125.00 | 125.00 | 167.00 | 3,000.00 | 43,813.50 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_arena_message | 10 | 1193 | 625.00 | 625.50 | 667.00 | 3,312.50 | 13,104.50 | 94,294.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_array_fresh_buffer | 10 | 1193 | 667.00 | 708.50 | 750.00 | 3,458.00 | 16,437.50 | 125,337.00 | 1,000,000.00 | 1,193,000,000.00 |
| ten_strings_fifty_decimal_strings | serialize_array_reuse | 10 | 1193 | 625.00 | 667.00 | 709.00 | 3,479.50 | 22,750.50 | 482,492.00 | 0.00 | 0.00 |
| ten_strings_fifty_decimal_strings | serialize_string_reserved | 10 | 1193 | 625.00 | 667.00 | 708.00 | 3,333.50 | 14,271.50 | 105,065.50 | 0.00 | 0.00 |

## Allocation interpretation

- `serialize_array_reuse` measures a caller-owned buffer reused across encodes.
- `serialize_array_fresh_buffer` intentionally allocates a new output vector for each encode and exposes allocator cost.
- `serialize_string_reserved` reuses a reserved string, while `serialize_array_arena_message` serializes a message allocated on a protobuf Arena.
- Allocation counts include allocations observed by the benchmark process during the timed loop; they are not a decoding metric.
