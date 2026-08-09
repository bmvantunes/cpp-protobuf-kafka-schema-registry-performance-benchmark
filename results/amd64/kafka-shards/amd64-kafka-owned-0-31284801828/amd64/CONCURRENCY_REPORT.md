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
| shared_buffer_mutex | 1 | 10 | 49 | 69.84 | 69.71 | 14.35 |
| shared_buffer_mutex | 2 | 10 | 49 | 392.15 | 389.84 | 2.57 |
| shared_buffer_mutex | 4 | 10 | 49 | 396.34 | 392.59 | 2.55 |
| shared_buffer_mutex | 8 | 10 | 49 | 392.21 | 392.36 | 2.55 |
| shared_readonly | 1 | 10 | 49 | 61.47 | 61.09 | 16.38 |
| shared_readonly | 2 | 10 | 49 | 46.23 | 46.23 | 21.63 |
| shared_readonly | 4 | 10 | 49 | 45.29 | 45.33 | 22.06 |
| shared_readonly | 8 | 10 | 49 | 45.34 | 45.35 | 22.05 |
| thread_local | 1 | 10 | 49 | 60.49 | 59.90 | 16.72 |
| thread_local | 2 | 10 | 49 | 43.90 | 43.88 | 22.79 |
| thread_local | 4 | 10 | 49 | 43.93 | 44.35 | 22.56 |
| thread_local | 8 | 10 | 49 | 44.23 | 44.29 | 22.58 |

## Mode definitions

- `thread_local`: each worker has its own protobuf message and output buffer.
- `shared_readonly`: workers serialize the same immutable message into independent buffers.
- `shared_buffer_mutex`: workers serialize into one shared buffer under a mutex, exposing lock contention.
- Thread creation and barrier setup are outside the timed encode loop.
