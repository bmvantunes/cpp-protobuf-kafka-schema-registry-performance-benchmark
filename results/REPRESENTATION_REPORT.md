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
| fixed64_scale_8 | 10 | 99 | 58.70 | 59.71 | 68.95 | 68.95 | 68.95 | 68.95 | 16.75 |
| bytes_128bit_coefficient_scale_8 | 10 | 189 | 77.30 | 81.21 | 114.97 | 114.97 | 114.97 | 114.97 | 12.31 |
| decimal_string | 10 | 169 | 202.87 | 205.13 | 278.78 | 278.78 | 278.78 | 278.78 | 4.87 |
| scaled_sint64_scale_8 | 10 | 79 | 243.30 | 224.62 | 360.52 | 360.52 | 360.52 | 360.52 | 4.45 |

## Trade-off

- Strings are self-describing and preserve exact text, but include decimal characters and protobuf length-delimited overhead.
- Fixed-scale `sint64` is compact for values that fit the agreed scale and range; the scale must be part of the schema contract.
- `fixed64` avoids varint-size variation but always spends eight coefficient bytes.
- `bytes` with a 16-byte coefficient supports a wider exact range, at the cost of a larger fixed payload.
