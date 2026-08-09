# Schema Registry evolution benchmark

> Control-plane schema evolution only. No decoding or per-message network path is included.

## Run contract

- Repetitions: `10`
- Each repetition registers v1, registers v2 under the same subject, then lists the subject versions.
- v2 adds `venue` while preserving v1 fields and the same fully-qualified Protobuf message name.

## Results

| Phase | Reps | Status codes | Median ns | Mean ns | p95 ns | Response bytes |
|---|---:|---|---:|---:|---:|---:|
| lookup_versions | 10 | 200 | 2,311,340.00 | 2,627,674.60 | 5,055,068.00 | 5.0 |
| register_v1 | 10 | 200 | 16,665,916.50 | 33,255,510.00 | 180,357,368.00 | 2400.0 |
| register_v2 | 10 | 200 | 16,896,194.00 | 21,815,966.90 | 53,886,700.00 | 2422.0 |

## Interpretation

- A successful v2 registration under the v1 subject demonstrates the tested Registry compatibility path for this additive schema change.
- This does not replace a full compatibility-policy matrix; production rollout should also test breaking changes and the configured BACKWARD/FORWARD/FULL policy explicitly.
