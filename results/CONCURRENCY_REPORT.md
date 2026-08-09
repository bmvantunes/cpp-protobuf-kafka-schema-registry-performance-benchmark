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
| shared_buffer_mutex | 1 | 10 | 49 | 102.77 | 103.66 | 9.65 |
| shared_buffer_mutex | 2 | 10 | 49 | 103.34 | 104.13 | 9.61 |
| shared_buffer_mutex | 4 | 10 | 49 | 104.66 | 107.45 | 9.36 |
| shared_buffer_mutex | 8 | 10 | 49 | 104.79 | 104.94 | 9.53 |
| shared_readonly | 1 | 10 | 49 | 85.40 | 86.04 | 11.63 |
| shared_readonly | 2 | 10 | 49 | 86.68 | 88.48 | 11.34 |
| shared_readonly | 4 | 10 | 49 | 89.31 | 91.30 | 11.00 |
| shared_readonly | 8 | 10 | 49 | 85.43 | 85.40 | 11.71 |
| thread_local | 1 | 10 | 49 | 86.96 | 90.52 | 11.18 |
| thread_local | 2 | 10 | 49 | 84.63 | 84.56 | 11.83 |
| thread_local | 4 | 10 | 49 | 84.72 | 88.86 | 11.41 |
| thread_local | 8 | 10 | 49 | 85.82 | 86.46 | 11.57 |

## Mode definitions

- `thread_local`: each worker has its own protobuf message and output buffer.
- `shared_readonly`: workers serialize the same immutable message into independent buffers.
- `shared_buffer_mutex`: workers serialize into one shared buffer under a mutex, exposing lock contention.
- Thread creation and barrier setup are outside the timed encode loop.
