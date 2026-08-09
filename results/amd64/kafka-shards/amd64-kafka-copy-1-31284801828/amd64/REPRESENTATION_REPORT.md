# Exact decimal representation benchmark

> Protobuf encoding only. No decoding, parsing, or network work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- Warmup encodes excluded: `10000`
- Integer/binary scale: `8` decimal places
- The integer and binary representations use the same fixed-scale numeric values; the string representation preserves decimal text.

## Results

| Representation | Reps | Bytes | Median ns/encode | Mean ns/encode | p95 ns | p99 ns | p99.9 ns | p99.99 ns | Mean M/s |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| fixed64_scale_8 | 10 | 99 | 27.33 | 27.13 | 28.01 | 28.01 | 28.01 | 28.01 | 36.86 |
| bytes_128bit_coefficient_scale_8 | 10 | 189 | 57.83 | 57.76 | 58.07 | 58.07 | 58.07 | 58.07 | 17.31 |
| scaled_sint64_scale_8 | 10 | 79 | 68.35 | 68.46 | 69.03 | 69.03 | 69.03 | 69.03 | 14.61 |
| decimal_string | 10 | 169 | 179.61 | 179.49 | 180.66 | 180.66 | 180.66 | 180.66 | 5.57 |

## Trade-off

- Strings are self-describing and preserve exact text, but include decimal characters and protobuf length-delimited overhead.
- Fixed-scale `sint64` is compact for values that fit the agreed scale and range; the scale must be part of the schema contract.
- `fixed64` avoids varint-size variation but always spends eight coefficient bytes.
- `bytes` with a 16-byte coefficient supports a wider exact range, at the cost of a larger fixed payload.
