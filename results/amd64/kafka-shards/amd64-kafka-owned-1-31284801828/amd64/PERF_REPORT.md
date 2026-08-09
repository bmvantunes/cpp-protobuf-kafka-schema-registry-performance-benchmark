# Hardware-counter benchmark

> Dockerized `perf stat` around the exact-decimal representation benchmark. No decoding work is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions: `10`
- perf exit status: `127`
- Events requested: cycles, instructions, cache references, cache misses, branches, and branch misses.

## Raw perf output

```text
perf output missing
```

If events are unavailable under the container host, the output is retained explicitly rather than being presented as a valid counter measurement. Repeat this phase on production Linux hardware with the required perf permissions for authoritative microarchitectural comparisons.
