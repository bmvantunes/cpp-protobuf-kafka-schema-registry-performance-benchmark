# ARM64 versus native AMD64 comparison

This comparison covers the two completed Docker runs:

| Run | Host | Toolchain | Evidence |
|---|---|---|---|
| ARM64 | macOS/OrbStack, arm64 | GCC 15.2, C++23, Buf 1.72, protobuf 35.1 | [`arm64.md`](arm64.md) |
| AMD64 | GitHub Actions `ubuntu-24.04`, native `x86_64` Azure runner | GCC 15.2, C++23, Buf 1.72, protobuf 35.1 | [`amd64.md`](amd64.md), [workflow run](https://github.com/bmvantunes/cpp-protobuf-kafka-schema-registry-performance-benchmark/actions/runs/31284801828) |

Both runs execute the benchmark workloads inside Docker. Every steady-state row uses 1,000,000 encodes per repetition and 10 measured repetitions; no decode, parse, consumer, or deserialization work is included.

## Pure encoding winners

Median nanoseconds per encode; lower is better. These are the fastest rows in each architecture’s complete protobuf/JSON matrix.

| Payload | ARM64 winner | AMD64 winner | ARM64 | AMD64 | AMD64 / ARM64 |
|---|---|---|---:|---:|---:|
| 1 string + 10 int64 | Google protobuf `LITE_RUNTIME`, preallocated array | Google protobuf `SPEED`, preallocated array | 80.39 ns | 56.13 ns | 0.70x |
| 1 string + 10 decimal strings | Google protobuf `SPEED`, `SerializeToString` | protobuf-c, preallocated pack | 160.29 ns | 102.86 ns | 0.64x |
| 10 strings + 50 decimal strings | protobuf-c, preallocated pack | protobuf-c, preallocated pack | 496.38 ns | 623.04 ns | 1.26x |

The winner is workload-dependent. Google protobuf is best for the compact int64 payload on both machines, with ARM64 favoring `LITE_RUNTIME` and AMD64 favoring `SPEED`; protobuf-c wins the largest string-heavy payload on both, while Google `SPEED` wins the smaller ARM64 decimal-string payload. The decimal cases are exact strings, as required, rather than floating-point JSON values.

## Protobuf versus JSON

Fastest median rows by payload:

| Payload | ARM64 protobuf | ARM64 JSON | AMD64 protobuf | AMD64 JSON |
|---|---:|---:|---:|---:|
| 1 string + 10 int64 | 80.39 ns | 95.66 ns (`yyjson`) | 56.13 ns | 104.31 ns (`yyjson`) |
| 1 string + 10 decimal strings | 160.29 ns | 211.49 ns (`yyjson`) | 102.86 ns | 272.21 ns (`boost::json`) |
| 10 strings + 50 decimal strings | 496.38 ns | 746.76 ns (`yyjson`) | 623.04 ns | 1,011.58 ns (`yyjson`) |

Protobuf is between 1.32x and 1.52x faster than the best JSON row on ARM64 and between 1.17x and 1.62x faster on native AMD64 in these measurements; its wire payload is also smaller. JSON remains viable for non-hot-path interoperability, but the benchmark does not support choosing it for the lowest-latency Kafka producer path.

## Exact decimal representations

Median nanoseconds per encode in the dedicated protobuf representation test:

| Representation | ARM64 | AMD64 | AMD64 / ARM64 |
|---|---:|---:|---:|
| `fixed64`, scale 8 | 58.70 ns | 27.33 ns | 0.47x |
| 128-bit coefficient in `bytes`, scale 8 | 77.30 ns | 57.83 ns | 0.75x |
| scaled `sint64`, scale 8 | 243.30 ns | 68.35 ns | 0.28x |
| decimal string | 202.87 ns | 179.61 ns | 0.89x |

For values that fit the agreed scale and range, fixed-scale integers are the fastest compact exact representation. `bytes` with a 128-bit coefficient preserves more range at a larger wire size. Decimal strings are the most self-describing but materially slower and larger.

## Kafka producer path

Both the native AMD64 GitHub run and the local ARM64 OrbStack run completed the full matrix: 2 ownership modes × 3 acknowledgement modes × 3 compression modes × 2 linger values × 2 batch sizes = 72 configurations, with 720 measured rows per architecture and zero delivery errors. The real producer matrix uses the representative 49-byte `one_string_ten_int64` message; all three payload shapes are covered in the pure encoding and Registry framing phases. See [`arm64.md`](arm64.md), [`amd64.md`](amd64.md), and each architecture’s `KAFKA_PRODUCER_REPORT.md` plus raw CSVs.

Representative native AMD64 median end-to-end results for `copy`, no compression, linger 0:

| Acks | Batch messages | Median end-to-end |
|---:|---:|---:|
| `0` | 1 | 44,525 ns/message |
| `0` | 100 | 742 ns/message |
| `1` | 1 | 57,974 ns/message |
| `1` | 100 | 956 ns/message |
| `all` | 1 | 103,869 ns/message |
| `all` | 100 | 1,162 ns/message |

Batching dominates these local single-node tests. `acks=0` is not a durability result; `acks=1` or `acks=all` must be selected according to the trading system’s loss/recovery policy. Compression and broker/network topology need production-cluster validation before a final Kafka setting is chosen.

## Schema Registry verdict

The Confluent framing prefix measured here is six bytes: magic byte, four-byte schema ID, and the one-byte message-index encoding for a single top-level message.

On AMD64, cached schema-ID framing added only 0.53%–1.85% in-place over the matching pure Google protobuf baseline, depending on payload. The framed output is 6 bytes larger. The extra copy path added 1.71%–5.44% in the plain Registry run. TLS and Basic Auth did not change the hot-loop rule: the local cached-ID framing path remained low-single-digit-percent overhead, while live Registry operations were millisecond-scale control-plane requests.

Measured AMD64 plain Registry medians:

| Operation | Median |
|---|---:|
| Cached in-place framing, int64 payload | 57.17 ns/message |
| Cached in-place framing, 1-string/10-decimal payload | 164.46 ns/message |
| Cached in-place framing, large string payload | 897.20 ns/message |
| Keep-alive schema lookup | 2.98 ms/request |
| Cold schema lookup | 3.71 ms/request |
| Schema registration | 17.76 ms/request |

Verdict: Schema Registry is acceptable for HFT only when the schema ID is resolved during startup, deployment, or a controlled recovery path and then cached locally. A Registry HTTP lookup or registration must not be placed in the Kafka producer hot loop. Prefer preallocated in-place framing; treat serializer-string and copy paths as convenience paths.

## What the architecture result does and does not prove

- The AMD64 result is native `x86_64` GitHub Actions hardware, not ARM emulation.
- The ARM64 result is Docker under OrbStack on the local arm64 machine, not bare-metal production ARM64.
- The compiler matrix also covers GCC 15.2 and Clang 22.1.2 under C++23 and C++26; absolute results still need a production compiler/flags audit.
- `perf` counters were unavailable in the container hosts, so no valid cycles/instructions/cache comparison is claimed.
- Absolute Kafka and Registry numbers are lab-host and scheduler dependent. Re-run the same Docker suite on the production pinned-core Linux hosts, real multi-node Kafka cluster, and final security configuration before using the numbers as trading-SLA budgets.
- No bare-metal production Linux host, production ARM64 host, or production multi-node Kafka cluster was available in this session. GitHub’s native AMD64 runner is useful architecture evidence, but it is not a production-hardware measurement.
