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
| int64_small_varints | 10 | 29.00 | 78.77 | 78.80 | 12.69 |
| string_short_ascii | 10 | 49.00 | 94.75 | 97.80 | 10.30 |
| string_multibyte_utf8 | 10 | 67.92 | 104.08 | 106.36 | 9.42 |
| string_long_ascii_256 | 10 | 299.00 | 104.67 | 105.83 | 9.46 |
| int64_mixed_distribution | 10 | 76.50 | 104.68 | 105.82 | 9.46 |
| int64_large_varints | 10 | 109.00 | 114.04 | 117.23 | 8.57 |
| int64_negative_standard_varint | 10 | 119.00 | 118.49 | 121.34 | 8.26 |

## Interpretation

- Standard negative `int64` values use the ordinary protobuf int64 wire encoding and can expand to ten bytes each; compare them with small positive varints.
- Large positive values expose the upper varint-width path without changing the field type.
- Mixed values approximate branch and payload-size variation in a live feed.
- Long and multibyte strings isolate length-delimited payload size and UTF-8 byte-count effects.
