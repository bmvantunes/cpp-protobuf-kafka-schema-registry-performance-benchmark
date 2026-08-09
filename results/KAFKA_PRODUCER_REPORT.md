# Kafka producer benchmark

> Encoding plus real librdkafka producer handoff. No decoding benchmark is included.

## Run contract

- Configurations: `72`
- Encodes/messages per repetition: `1000000`
- Repetitions per configuration: `10`
- `enqueue` covers serialization plus the librdkafka `produce()` handoff.
- `flush` covers the remaining producer/broker delivery time for the configured acknowledgement mode.
- `end_to_end` is enqueue plus flush and is the relevant result for this producer-path benchmark.

## Results

| Mode | Acks | Compression | Linger ms | Batch messages | Reps | Bytes | Median enqueue ns/msg | Median end-to-end ns/msg | p95 end-to-end ns/msg | p99 end-to-end ns/msg | Errors |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| copy | 0 | lz4 | 0 | 1 | 10 | 49 | 49,224.71 | 53,941.98 | 74,960.06 | 74,960.06 | 0 |
| copy | 0 | lz4 | 0 | 100 | 10 | 49 | 459.38 | 499.93 | 637.96 | 637.96 | 0 |
| copy | 0 | lz4 | 5 | 1 | 10 | 49 | 41,544.73 | 46,174.51 | 46,690.77 | 46,690.77 | 0 |
| copy | 0 | lz4 | 5 | 100 | 10 | 49 | 507.67 | 507.76 | 551.78 | 551.78 | 0 |
| copy | 0 | none | 0 | 1 | 10 | 49 | 42,456.27 | 47,609.74 | 63,083.53 | 63,083.53 | 0 |
| copy | 0 | none | 0 | 100 | 10 | 49 | 494.98 | 551.97 | 1,156.63 | 1,156.63 | 0 |
| copy | 0 | none | 5 | 1 | 10 | 49 | 42,169.04 | 46,845.21 | 67,508.94 | 67,508.94 | 0 |
| copy | 0 | none | 5 | 100 | 10 | 49 | 702.68 | 788.12 | 954.10 | 954.10 | 0 |
| copy | 0 | zstd | 0 | 1 | 10 | 49 | 42,540.86 | 47,125.46 | 48,614.27 | 48,614.27 | 0 |
| copy | 0 | zstd | 0 | 100 | 10 | 49 | 607.80 | 610.78 | 885.15 | 885.15 | 0 |
| copy | 0 | zstd | 5 | 1 | 10 | 49 | 44,322.25 | 49,025.99 | 68,494.06 | 68,494.06 | 0 |
| copy | 0 | zstd | 5 | 100 | 10 | 49 | 610.41 | 611.21 | 932.45 | 932.45 | 0 |
| copy | 1 | lz4 | 0 | 1 | 10 | 49 | 62,735.25 | 70,327.37 | 83,922.19 | 83,922.19 | 0 |
| copy | 1 | lz4 | 0 | 100 | 10 | 49 | 934.03 | 1,002.92 | 1,911.06 | 1,911.06 | 0 |
| copy | 1 | lz4 | 5 | 1 | 10 | 49 | 65,931.82 | 73,576.38 | 88,908.52 | 88,908.52 | 0 |
| copy | 1 | lz4 | 5 | 100 | 10 | 49 | 557.53 | 623.03 | 736.04 | 736.04 | 0 |
| copy | 1 | none | 0 | 1 | 10 | 49 | 62,526.03 | 68,981.85 | 77,062.97 | 77,062.97 | 0 |
| copy | 1 | none | 0 | 100 | 10 | 49 | 658.41 | 738.26 | 842.99 | 842.99 | 0 |
| copy | 1 | none | 5 | 1 | 10 | 49 | 70,822.42 | 81,161.24 | 100,764.53 | 100,764.53 | 0 |
| copy | 1 | none | 5 | 100 | 10 | 49 | 595.26 | 669.15 | 844.43 | 844.43 | 0 |
| copy | 1 | zstd | 0 | 1 | 10 | 49 | 63,616.55 | 70,028.15 | 110,860.43 | 110,860.43 | 0 |
| copy | 1 | zstd | 0 | 100 | 10 | 49 | 649.03 | 700.72 | 1,107.21 | 1,107.21 | 0 |
| copy | 1 | zstd | 5 | 1 | 10 | 49 | 59,351.92 | 66,071.35 | 79,860.74 | 79,860.74 | 0 |
| copy | 1 | zstd | 5 | 100 | 10 | 49 | 649.31 | 714.76 | 813.68 | 813.68 | 0 |
| copy | all | lz4 | 0 | 1 | 10 | 49 | 60,738.07 | 67,908.32 | 80,326.41 | 80,326.41 | 0 |
| copy | all | lz4 | 0 | 100 | 10 | 49 | 666.26 | 740.71 | 776.62 | 776.62 | 0 |
| copy | all | lz4 | 5 | 1 | 10 | 49 | 57,704.50 | 64,074.82 | 73,636.01 | 73,636.01 | 0 |
| copy | all | lz4 | 5 | 100 | 10 | 49 | 740.27 | 810.28 | 2,064.72 | 2,064.72 | 0 |
| copy | all | none | 0 | 1 | 10 | 49 | 60,948.96 | 67,358.40 | 76,429.11 | 76,429.11 | 0 |
| copy | all | none | 0 | 100 | 10 | 49 | 677.55 | 747.95 | 3,640.39 | 3,640.39 | 0 |
| copy | all | none | 5 | 1 | 10 | 49 | 60,202.48 | 67,291.16 | 76,794.36 | 76,794.36 | 0 |
| copy | all | none | 5 | 100 | 10 | 49 | 1,010.63 | 1,098.15 | 3,546.59 | 3,546.59 | 0 |
| copy | all | zstd | 0 | 1 | 10 | 49 | 63,044.16 | 69,394.71 | 83,398.94 | 83,398.94 | 0 |
| copy | all | zstd | 0 | 100 | 10 | 49 | 870.99 | 934.84 | 2,091.94 | 2,091.94 | 0 |
| copy | all | zstd | 5 | 1 | 10 | 49 | 73,405.62 | 79,764.85 | 98,797.49 | 98,797.49 | 0 |
| copy | all | zstd | 5 | 100 | 10 | 49 | 645.95 | 678.92 | 863.84 | 863.84 | 0 |
| owned | 0 | lz4 | 0 | 1 | 10 | 49 | 52,733.74 | 60,202.20 | 74,195.87 | 74,195.87 | 0 |
| owned | 0 | lz4 | 0 | 100 | 10 | 49 | 544.79 | 572.67 | 630.00 | 630.00 | 0 |
| owned | 0 | lz4 | 5 | 1 | 10 | 49 | 55,470.67 | 61,643.35 | 142,561.94 | 142,561.94 | 0 |
| owned | 0 | lz4 | 5 | 100 | 10 | 49 | 619.35 | 635.47 | 782.13 | 782.13 | 0 |
| owned | 0 | none | 0 | 1 | 10 | 49 | 42,326.07 | 46,690.62 | 61,063.23 | 61,063.23 | 0 |
| owned | 0 | none | 0 | 100 | 10 | 49 | 694.61 | 740.57 | 1,022.17 | 1,022.17 | 0 |
| owned | 0 | none | 5 | 1 | 10 | 49 | 40,804.85 | 45,120.73 | 57,854.06 | 57,854.06 | 0 |
| owned | 0 | none | 5 | 100 | 10 | 49 | 548.51 | 552.35 | 602.28 | 602.28 | 0 |
| owned | 0 | zstd | 0 | 1 | 10 | 49 | 41,045.58 | 45,631.82 | 60,839.49 | 60,839.49 | 0 |
| owned | 0 | zstd | 0 | 100 | 10 | 49 | 684.11 | 685.48 | 738.94 | 738.94 | 0 |
| owned | 0 | zstd | 5 | 1 | 10 | 49 | 40,628.25 | 45,182.90 | 55,109.33 | 55,109.33 | 0 |
| owned | 0 | zstd | 5 | 100 | 10 | 49 | 641.97 | 642.45 | 711.65 | 711.65 | 0 |
| owned | 1 | lz4 | 0 | 1 | 10 | 49 | 56,643.71 | 62,970.52 | 63,668.73 | 63,668.73 | 0 |
| owned | 1 | lz4 | 0 | 100 | 10 | 49 | 706.72 | 775.90 | 929.09 | 929.09 | 0 |
| owned | 1 | lz4 | 5 | 1 | 10 | 49 | 57,649.83 | 63,957.77 | 82,465.22 | 82,465.22 | 0 |
| owned | 1 | lz4 | 5 | 100 | 10 | 49 | 587.72 | 597.02 | 875.34 | 875.34 | 0 |
| owned | 1 | none | 0 | 1 | 10 | 49 | 57,541.21 | 63,861.84 | 74,259.39 | 74,259.39 | 0 |
| owned | 1 | none | 0 | 100 | 10 | 49 | 641.24 | 713.57 | 857.41 | 857.41 | 0 |
| owned | 1 | none | 5 | 1 | 10 | 49 | 57,169.81 | 63,442.71 | 83,032.04 | 83,032.04 | 0 |
| owned | 1 | none | 5 | 100 | 10 | 49 | 590.73 | 650.71 | 1,687.49 | 1,687.49 | 0 |
| owned | 1 | zstd | 0 | 1 | 10 | 49 | 55,882.20 | 62,091.71 | 74,914.80 | 74,914.80 | 0 |
| owned | 1 | zstd | 0 | 100 | 10 | 49 | 695.62 | 718.79 | 832.82 | 832.82 | 0 |
| owned | 1 | zstd | 5 | 1 | 10 | 49 | 56,128.70 | 62,384.59 | 82,588.51 | 82,588.51 | 0 |
| owned | 1 | zstd | 5 | 100 | 10 | 49 | 668.10 | 689.00 | 909.91 | 909.91 | 0 |
| owned | all | lz4 | 0 | 1 | 10 | 49 | 57,615.73 | 64,027.30 | 64,340.19 | 64,340.19 | 0 |
| owned | all | lz4 | 0 | 100 | 10 | 49 | 698.13 | 765.36 | 803.90 | 803.90 | 0 |
| owned | all | lz4 | 5 | 1 | 10 | 49 | 58,771.08 | 65,673.72 | 75,995.68 | 75,995.68 | 0 |
| owned | all | lz4 | 5 | 100 | 10 | 49 | 974.66 | 1,110.79 | 1,638.29 | 1,638.29 | 0 |
| owned | all | none | 0 | 1 | 10 | 49 | 57,484.55 | 63,878.59 | 67,738.50 | 67,738.50 | 0 |
| owned | all | none | 0 | 100 | 10 | 49 | 638.75 | 705.66 | 896.26 | 896.26 | 0 |
| owned | all | none | 5 | 1 | 10 | 49 | 59,177.26 | 66,238.77 | 74,194.75 | 74,194.75 | 0 |
| owned | all | none | 5 | 100 | 10 | 49 | 597.69 | 681.44 | 895.35 | 895.35 | 0 |
| owned | all | zstd | 0 | 1 | 10 | 49 | 56,712.02 | 62,964.04 | 75,752.18 | 75,752.18 | 0 |
| owned | all | zstd | 0 | 100 | 10 | 49 | 708.19 | 756.18 | 856.22 | 856.22 | 0 |
| owned | all | zstd | 5 | 1 | 10 | 49 | 57,712.02 | 64,039.18 | 72,651.37 | 72,651.37 | 0 |
| owned | all | zstd | 5 | 100 | 10 | 49 | 664.91 | 667.73 | 886.83 | 886.83 | 0 |

## Interpretation

- Compare `copy` and `owned` carefully: the owned path transfers an allocated payload to librdkafka, while the copy path reuses a caller-owned buffer and asks librdkafka to copy it.
- `acks=0` measures producer handoff with no broker acknowledgement guarantee; it must not be treated as durable delivery.
- Compression and batching can reduce wire bytes at the cost of producer CPU and latency. The correct choice depends on the latency budget and broker/network constraints.
- This report measures producer handoff and flush behavior, not consumer throughput or decoding.

## Raw data

Every configuration has one CSV file with all repetitions. The raw rows include enqueue, flush, end-to-end elapsed time, delivery errors, and configuration fields.
