# `int64` versus `fixed64` encoding benchmark

> Identical positive values, identical field count, preallocated `SerializeToArray`, encoding only. No decoding or Kafka work is included.

## Run contract

- Every row: `1,000,000` encodes per repetition × `10` measured repetitions.
- The only changed variable is the generated field wire type: signed `int64` versus unsigned `fixed64`.
- The `fixed64` schema is valid for non-negative values. Use `sfixed64` if signed semantics are required.

## Results

| Value range | Representation | Bytes | Median ns/encode | Mean ns/encode | Mean M/s | Fixed64 speed versus int64 |
|---|---|---:|---:|---:|---:|---:|
| positive_near_max | int64 | 109 | 119.84 | 122.71 | 8.34 | 1.00x |
| positive_near_max | fixed64 | 99 | 62.73 | 62.53 | 15.94 | 1.91x |
| positive_scaled_price | int64 | 79 | 117.76 | 129.94 | 8.49 | 1.00x |
| positive_scaled_price | fixed64 | 99 | 69.23 | 71.37 | 14.44 | 1.70x |
| positive_small | int64 | 49 | 136.44 | 129.89 | 7.33 | 1.00x |
| positive_small | fixed64 | 99 | 87.99 | 94.30 | 11.37 | 1.55x |

## Verdict

`fixed64` always uses eight payload bytes per numeric field, so its wire size is stable. `int64` uses a variable-length varint: small positive values are smaller, while large positive values approach the fixed-width size. The benchmark result, not intuition, determines whether the fixed-width serializer wins on the target CPU and generated code.

Do not choose `fixed64` solely because it avoids varint branching. It can be faster for large values, but it can also produce larger Kafka records. For prices, quantities, and other non-negative fixed-scale values, compare the exact production distribution and include broker/network bandwidth in the final decision.
