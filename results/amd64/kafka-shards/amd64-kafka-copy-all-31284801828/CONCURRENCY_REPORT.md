# Concurrency and contention benchmark

> Protobuf encoding only. Each row contains one million total encodes per repetition across the configured threads.

## Run contract

- Configurations: `12`
- Total encodes per repetition: `1000000`
- Repetitions: `10`
- Thread counts: `1,2,4,8`

## Results

| Mode | Threads | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---:|---:|---:|---:|---:|---:|
| shared_buffer_mutex | 1 | 10 | 49 | 101.04 | 103.01 | 9.73 |
| shared_buffer_mutex | 2 | 10 | 49 | 101.76 | 105.78 | 9.51 |
| shared_buffer_mutex | 4 | 10 | 49 | 101.65 | 103.77 | 9.65 |
| shared_buffer_mutex | 8 | 10 | 49 | 102.62 | 103.07 | 9.70 |
| shared_readonly | 1 | 10 | 49 | 88.16 | 90.26 | 11.13 |
| shared_readonly | 2 | 10 | 49 | 85.93 | 86.61 | 11.55 |
| shared_readonly | 4 | 10 | 49 | 86.61 | 86.86 | 11.52 |
| shared_readonly | 8 | 10 | 49 | 84.00 | 84.48 | 11.84 |
| thread_local | 1 | 10 | 49 | 100.44 | 109.51 | 9.44 |
| thread_local | 2 | 10 | 49 | 99.34 | 103.87 | 9.78 |
| thread_local | 4 | 10 | 49 | 97.29 | 101.93 | 10.05 |
| thread_local | 8 | 10 | 49 | 94.16 | 99.16 | 10.28 |

## Mode definitions

- `thread_local`: each worker has its own protobuf message and output buffer.
- `shared_readonly`: workers serialize the same immutable message into independent buffers.
- `shared_buffer_mutex`: workers serialize into one shared buffer under a mutex, exposing lock contention.
- Thread creation and barrier setup are outside the timed encode loop.
