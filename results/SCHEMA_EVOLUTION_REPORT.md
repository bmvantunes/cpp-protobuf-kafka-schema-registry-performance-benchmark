# Schema Registry evolution benchmark

> Control-plane schema evolution only. No decoding or per-message network path is included.

## Run contract

- Repetitions: `10`
- Each repetition registers v1, registers v2 under the same subject, then lists the subject versions.
- v2 adds `venue` while preserving v1 fields and the same fully-qualified Protobuf message name.

## Results

| Phase | Reps | Status codes | Median ns | Mean ns | p95 ns | Response bytes |
|---|---:|---|---:|---:|---:|---:|
| lookup_versions | 10 | 200 | 4,377,601.00 | 6,922,328.70 | 30,456,010.00 | 5.0 |
| register_v1 | 10 | 200 | 26,430,026.50 | 45,235,710.50 | 227,148,755.00 | 2400.0 |
| register_v2 | 10 | 200 | 25,443,927.00 | 37,029,063.10 | 133,458,973.00 | 2422.0 |

## Interpretation

- A successful v2 registration under the v1 subject demonstrates the tested Registry compatibility path for this additive schema change.
- This does not replace a full compatibility-policy matrix; production rollout should also test breaking changes and the configured BACKWARD/FORWARD/FULL policy explicitly.
