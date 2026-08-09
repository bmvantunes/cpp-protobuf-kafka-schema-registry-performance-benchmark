# C++ Kafka serialization benchmark — linux/amd64-emulated

> Encoding only. No decode, parse, consumer, or deserialization benchmark is included.

## Run identity

- Requested architecture: `linux/amd64-emulated`
- Observed machine: `macOS-26.6-arm64-arm-64bit`
- Observed machine architecture: `arm64`
- Python: `3.9.6`
- Steady-state contract: `1,000,000` encodes per measured repetition and `10` measured repetitions.
- Control-plane Registry paths intentionally use ten live requests per path; they are not hot-loop encode measurements.
- Every phase was executed through Docker; absolute values are host- and scheduler-dependent.

## Phase index

- [REPORT.md](#reportmd)

## REPORT.md

# Protobuf and JSON encoding benchmark

> Encoding only. No decode, parse, or schema-registry/network time is included.

## Run contract

- Encodes per repetition: `1000000`
- Repetitions per benchmark: `10`
- Warmup encodes (excluded): `10000`
- Decimal representation: protobuf and JSON both use decimal strings; JSON does not parse floating-point decimals.
- Generated types: `buf generate` with Google C++ `SPEED`, `CODE_SIZE`, and `LITE_RUNTIME` variants, plus protobuf-c.

## Verdict by payload

The fastest row is selected by median nanoseconds per encode across the recorded repetitions.

| Payload | Fastest | Median ns/encode | Encodes/sec | Bytes |
|---|---|---:|---:|---:|
| one_string_ten_decimal_strings | google_protobuf / lite_runtime / SerializeToArray_preallocated | 174.46 | 5.68 M/s | 213 |
| one_string_ten_int64 | google_protobuf / lite_runtime / SerializeToArray_preallocated | 58.37 | 17.12 M/s | 49 |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime / SerializeToArray_preallocated | 922.08 | 1.08 M/s | 1193 |

## Full aggregate results

| Kind | Library | Codegen | API | Payload | Reps | Bytes | Median ns/encode | Mean ns/encode | Mean M/s |
|---|---|---|---|---|---:|---:|---:|---:|---:|
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 174.46 | 176.15 | 5.68 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_decimal_strings | 10 | 213 | 180.88 | 181.33 | 5.51 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 182.95 | 182.99 | 5.46 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 247.05 | 254.20 | 3.97 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 314.29 | 337.18 | 3.16 |
| json | boost_json | n/a | serialize | one_string_ten_decimal_strings | 10 | 364 | 408.54 | 420.26 | 2.38 |
| json | yyjson | n/a | mut_write | one_string_ten_decimal_strings | 10 | 419 | 416.29 | 422.93 | 2.37 |
| json | rapidjson | n/a | writer | one_string_ten_decimal_strings | 10 | 364 | 1,033.83 | 1,042.69 | 0.96 |
| json | nlohmann_json | n/a | dump | one_string_ten_decimal_strings | 10 | 364 | 1,132.90 | 1,158.53 | 0.86 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_decimal_strings | 10 | 213 | 2,424.18 | 2,441.95 | 0.41 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_decimal_strings | 10 | 213 | 2,431.49 | 2,452.17 | 0.41 |
| json | jsoncpp | n/a | writeString | one_string_ten_decimal_strings | 10 | 364 | 2,808.41 | 2,825.63 | 0.35 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 58.37 | 58.40 | 17.12 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | one_string_ten_int64 | 10 | 49 | 66.25 | 68.74 | 14.62 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 67.65 | 70.69 | 14.29 |
| protobuf | google_protobuf | speed | SerializeToString | one_string_ten_int64 | 10 | 49 | 76.75 | 77.39 | 12.93 |
| protobuf | protobuf_c | c_generated | pack_preallocated | one_string_ten_int64 | 10 | 49 | 149.99 | 150.10 | 6.66 |
| json | yyjson | n/a | mut_write | one_string_ten_int64 | 10 | 215 | 202.02 | 202.23 | 4.94 |
| json | boost_json | n/a | serialize | one_string_ten_int64 | 10 | 210 | 457.11 | 462.64 | 2.16 |
| json | rapidjson | n/a | writer | one_string_ten_int64 | 10 | 210 | 514.58 | 530.40 | 1.89 |
| json | nlohmann_json | n/a | dump | one_string_ten_int64 | 10 | 210 | 758.57 | 759.39 | 1.32 |
| protobuf | google_protobuf | code_size | SerializeToString | one_string_ten_int64 | 10 | 49 | 2,152.33 | 2,198.63 | 0.46 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | one_string_ten_int64 | 10 | 49 | 2,163.29 | 2,168.68 | 0.46 |
| json | jsoncpp | n/a | writeString | one_string_ten_int64 | 10 | 210 | 2,369.94 | 2,415.96 | 0.41 |
| protobuf | google_protobuf | lite_runtime | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 922.08 | 929.64 | 1.08 |
| protobuf | google_protobuf | lite_runtime | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 935.01 | 942.85 | 1.06 |
| protobuf | protobuf_c | c_generated | pack_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 982.14 | 991.49 | 1.01 |
| protobuf | google_protobuf | speed | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 1,119.91 | 1,234.41 | 0.83 |
| protobuf | google_protobuf | speed | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 1,452.20 | 1,509.08 | 0.69 |
| json | yyjson | n/a | mut_write | ten_strings_fifty_decimal_strings | 10 | 2279 | 1,852.26 | 1,950.59 | 0.52 |
| json | boost_json | n/a | serialize | ten_strings_fifty_decimal_strings | 10 | 1979 | 2,103.26 | 2,131.30 | 0.47 |
| json | nlohmann_json | n/a | dump | ten_strings_fifty_decimal_strings | 10 | 1979 | 5,833.63 | 5,862.60 | 0.17 |
| json | rapidjson | n/a | writer | ten_strings_fifty_decimal_strings | 10 | 1979 | 6,676.39 | 6,757.13 | 0.15 |
| protobuf | google_protobuf | code_size | SerializeToArray_preallocated | ten_strings_fifty_decimal_strings | 10 | 1193 | 12,744.32 | 12,777.44 | 0.08 |
| protobuf | google_protobuf | code_size | SerializeToString | ten_strings_fifty_decimal_strings | 10 | 1193 | 12,854.92 | 14,289.43 | 0.07 |
| json | jsoncpp | n/a | writeString | ten_strings_fifty_decimal_strings | 10 | 1979 | 15,010.94 | 16,233.96 | 0.06 |

## Protobuf-only comparison

| Payload | Library / generation | API | Median ns/encode | Relative to fastest protobuf |
|---|---|---|---:|---:|
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 174.46 | 1.00x |
| one_string_ten_decimal_strings | protobuf_c / c_generated | pack_preallocated | 180.88 | 1.04x |
| one_string_ten_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 182.95 | 1.05x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 247.05 | 1.42x |
| one_string_ten_decimal_strings | google_protobuf / speed | SerializeToString | 314.29 | 1.80x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToString | 2,424.18 | 13.90x |
| one_string_ten_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 2,431.49 | 13.94x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToArray_preallocated | 58.37 | 1.00x |
| one_string_ten_int64 | google_protobuf / lite_runtime | SerializeToString | 66.25 | 1.13x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToArray_preallocated | 67.65 | 1.16x |
| one_string_ten_int64 | google_protobuf / speed | SerializeToString | 76.75 | 1.31x |
| one_string_ten_int64 | protobuf_c / c_generated | pack_preallocated | 149.99 | 2.57x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToString | 2,152.33 | 36.87x |
| one_string_ten_int64 | google_protobuf / code_size | SerializeToArray_preallocated | 2,163.29 | 37.06x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToArray_preallocated | 922.08 | 1.00x |
| ten_strings_fifty_decimal_strings | google_protobuf / lite_runtime | SerializeToString | 935.01 | 1.01x |
| ten_strings_fifty_decimal_strings | protobuf_c / c_generated | pack_preallocated | 982.14 | 1.07x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToArray_preallocated | 1,119.91 | 1.21x |
| ten_strings_fifty_decimal_strings | google_protobuf / speed | SerializeToString | 1,452.20 | 1.57x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToArray_preallocated | 12,744.32 | 13.82x |
| ten_strings_fifty_decimal_strings | google_protobuf / code_size | SerializeToString | 12,854.92 | 13.94x |

## JSON-only comparison

| Payload | Library | Median ns/encode | Relative to fastest JSON |
|---|---|---:|---:|
| one_string_ten_decimal_strings | boost_json | 408.54 | 1.00x |
| one_string_ten_decimal_strings | yyjson | 416.29 | 1.02x |
| one_string_ten_decimal_strings | rapidjson | 1,033.83 | 2.53x |
| one_string_ten_decimal_strings | nlohmann_json | 1,132.90 | 2.77x |
| one_string_ten_decimal_strings | jsoncpp | 2,808.41 | 6.87x |
| one_string_ten_int64 | yyjson | 202.02 | 1.00x |
| one_string_ten_int64 | boost_json | 457.11 | 2.26x |
| one_string_ten_int64 | rapidjson | 514.58 | 2.55x |
| one_string_ten_int64 | nlohmann_json | 758.57 | 3.75x |
| one_string_ten_int64 | jsoncpp | 2,369.94 | 11.73x |
| ten_strings_fifty_decimal_strings | yyjson | 1,852.26 | 1.00x |
| ten_strings_fifty_decimal_strings | boost_json | 2,103.26 | 1.14x |
| ten_strings_fifty_decimal_strings | nlohmann_json | 5,833.63 | 3.15x |
| ten_strings_fifty_decimal_strings | rapidjson | 6,676.39 | 3.60x |
| ten_strings_fifty_decimal_strings | jsoncpp | 15,010.94 | 8.10x |

## Raw data

- `raw.csv` contains every individual repetition and is the source for the aggregates above.
- Re-run on a quiet, pinned CPU if comparing small differences; CPU frequency, thermal state, compiler, allocator, and container host affect absolute numbers.


## Toolchain and host metadata

### toolchain_versions.txt

```text
architecture=x86_64
kernel=7.0.11-orbstack-00360-gc9bc4d96ac70
compiler=c++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0
cmake=cmake version 4.2.3
buf=1.72.0
protoc=libprotoc 35.1
python=Python 3.14.4
protobuf_package=35.1
protobuf_c_package=1.5.2
boost_json_package=1.90.0
jsoncpp_package=not-installed
rapidjson_package=1.1.0+dfsg2-7.6ubuntu1
librdkafka=2.15.0
yyjson=0.12.0
nlohmann_json=3.12.0
jsoncpp=1.9.8
clang=Ubuntu clang version 22.1.2 (1ubuntu1)
```

### docker-host.txt

```text
Darwin Brunos-Mac-mini.local 25.6.0 Darwin Kernel Version 25.6.0: Sat Jul 11 15:24:35 PDT 2026; root:xnu-12377.161.13~4/RELEASE_ARM64_T8103 arm64
```

### docker-version.txt

```text
Client:
 Version:           29.4.0
 API version:       1.54
 Go version:        go1.26.3
 Git commit:        9d7ad9f
 Built:             Thu Jun  4 11:19:31 2026
 OS/Arch:           darwin/arm64
 Context:           orbstack

Server: Docker Engine - Community
 Engine:
  Version:          29.4.0
  API version:      1.54 (minimum version 1.40)
  Go version:       go1.26.1
  Git commit:       daa0cb7f
  Built:            Tue Apr  7 08:35:43 2026
  OS/Arch:          linux/arm64
  Experimental:     true
 containerd:
  Version:          v2.2.2
  GitCommit:        301b2dac98f15c27117da5c8af12118a041a31d9
 runc:
  Version:          1.4.2
  GitCommit:        c241c0bb5e60a8e8c1b2e53d4eca8d0068d8d57e
 docker-init:
  Version:          0.19.0
  GitCommit:        de40ad0
```

### docker-info.txt

```text
Client:
 Version:    29.4.0
 Context:    orbstack
 Debug Mode: false
 Plugins:
  buildx: Docker Buildx (Docker Inc.)
    Version:  v0.33.0
    Path:     /Users/bruno/.docker/cli-plugins/docker-buildx
  compose: Docker Compose (Docker Inc.)
    Version:  v5.1.2
    Path:     /Users/bruno/.docker/cli-plugins/docker-compose
  dev: Docker Dev Environments (Docker Inc.)
    Version:  v0.0.3
    Path:     /usr/local/lib/docker/cli-plugins/docker-dev
  extension: Manages Docker extensions (Docker Inc.)
    Version:  v0.2.13
    Path:     /usr/local/lib/docker/cli-plugins/docker-extension
  sbom: View the packaged-based Software Bill Of Materials (SBOM) for an image (Anchore Inc.)
    Version:  0.6.0
    Path:     /usr/local/lib/docker/cli-plugins/docker-sbom
  scan: Docker Scan (Docker Inc.)
    Version:  v0.21.0
    Path:     /usr/local/lib/docker/cli-plugins/docker-scan

Server:
 Containers: 0
  Running: 0
  Paused: 0
  Stopped: 0
 Images: 11
 Server Version: 29.4.0
 Storage Driver: overlayfs
  driver-type: io.containerd.snapshotter.v1
 Logging Driver: json-file
 Cgroup Driver: cgroupfs
 Cgroup Version: 2
 Plugins:
  Volume: local
  Network: bridge host ipvlan macvlan null overlay
  Log: awslogs fluentd gcplogs gelf journald json-file local splunk syslog
 CDI spec directories:
  /etc/cdi
  /var/run/cdi
 Swarm: inactive
 Runtimes: io.containerd.runc.v2 runc
 Default Runtime: runc
 Init Binary: docker-init
 containerd version: 301b2dac98f15c27117da5c8af12118a041a31d9
 runc version: c241c0bb5e60a8e8c1b2e53d4eca8d0068d8d57e
 init version: de40ad0
 Security Options:
  seccomp
   Profile: builtin
  cgroupns
 Kernel Version: 7.0.11-orbstack-00360-gc9bc4d96ac70
 Operating System: OrbStack
 OSType: linux
 Architecture: aarch64
 CPUs: 8
 Total Memory: 7.818GiB
 Name: orbstack
 ID: b2bcb197-2ded-4246-9b9d-ea26eddea79f
 Docker Root Dir: /var/lib/docker
 Debug Mode: false
 HTTP Proxy: http://proxy.orb.internal:8305
 HTTPS Proxy: http://proxy.orb.internal:8305
 No Proxy: localhost,127.0.0.1,127.0.0.0/8,::1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16,0.250.250.0/24,*.orb.internal,*.local,gateway.docker.internal,host.internal,host.docker.internal,host.lima.internal,docker.for.mac.localhost,docker.for.mac.host.internal
 Experimental: true
 Insecure Registries:
  127.0.0.0/8
  ::1/128
 Live Restore Enabled: false
 Product License: Community Engine
 Default Address Pools:
   Base: 192.168.97.0/24, Size: 24
   Base: 192.168.107.0/24, Size: 24
   Base: 192.168.117.0/24, Size: 24
   Base: 192.168.147.0/24, Size: 24
   Base: 192.168.148.0/24, Size: 24
   Base: 192.168.155.0/24, Size: 24
   Base: 192.168.156.0/24, Size: 24
   Base: 192.168.158.0/24, Size: 24
   Base: 192.168.163.0/24, Size: 24
   Base: 192.168.164.0/24, Size: 24
   Base: 192.168.165.0/24, Size: 24
   Base: 192.168.166.0/24, Size: 24
   Base: 192.168.167.0/24, Size: 24
   Base: 192.168.171.0/24, Size: 24
   Base: 192.168.172.0/24, Size: 24
   Base: 192.168.181.0/24, Size: 24
   Base: 192.168.183.0/24, Size: 24
   Base: 192.168.186.0/24, Size: 24
   Base: 192.168.207.0/24, Size: 24
   Base: 192.168.214.0/24, Size: 24
   Base: 192.168.215.0/24, Size: 24
   Base: 192.168.216.0/24, Size: 24
   Base: 192.168.223.0/24, Size: 24
   Base: 192.168.227.0/24, Size: 24
   Base: 192.168.228.0/24, Size: 24
   Base: 192.168.229.0/24, Size: 24
   Base: 192.168.237.0/24, Size: 24
   Base: 192.168.239.0/24, Size: 24
   Base: 192.168.242.0/24, Size: 24
   Base: 192.168.247.0/24, Size: 24
   Base: fd07:b51a:cc66:d000::/56, Size: 64
 Firewall Backend: iptables
```

## Raw artifacts

The accompanying workflow artifact contains the raw CSV files, metadata, Docker version output, and this report. CSV files are retained for statistical re-analysis; the tables above are the human-readable snapshot committed or uploaded for review.
