# Realistic value distribution benchmark

> Protobuf encoding only. No decoding or network work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- Warmup encodes excluded: `10000`
- Fixed corpus size: `1024` pre-populated messages
- The corpus changes which values are serialized without measuring message construction or random-number generation inside the timed loop.

## Results

| Distribution | Reps | Mean bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---:|---:|---:|---:|---:|
| int64_small_varints | 10 | 29.00 | 90.14 | 102.37 | 10.27 |
| string_short_ascii | 10 | 49.00 | 98.96 | 105.26 | 9.75 |
| string_multibyte_utf8 | 10 | 67.92 | 108.46 | 112.24 | 8.98 |
| int64_mixed_distribution | 10 | 76.50 | 109.39 | 116.44 | 8.68 |
| int64_negative_standard_varint | 10 | 119.00 | 126.76 | 146.22 | 7.14 |
| int64_large_varints | 10 | 109.00 | 152.15 | 181.46 | 6.08 |
| string_long_ascii_256 | 10 | 299.00 | 159.14 | 159.16 | 6.65 |

## Interpretation

- Standard negative `int64` values use the ordinary protobuf int64 wire encoding and can expand to ten bytes each; compare them with small positive varints.
- Large positive values expose the upper varint-width path without changing the field type.
- Mixed values approximate branch and payload-size variation in a live feed.
- Long and multibyte strings isolate length-delimited payload size and UTF-8 byte-count effects.
