FROM ubuntu:26.04

ARG BUF_VERSION=1.72.0
ARG CMAKE_VERSION=4.4.2
ARG PROTOBUF_VERSION=35.0
ARG RDKAFKA_VERSION=2.8.0
ARG YYJSON_VERSION=0.12.0
ARG NLOHMANN_JSON_VERSION=3.12.0
ARG TARGETARCH

ENV DEBIAN_FRONTEND=noninteractive \
    LD_LIBRARY_PATH=/usr/local/lib \
    PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/local/share/pkgconfig

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       build-essential \
       autoconf \
       automake \
       ca-certificates \
       curl \
       git \
       libabsl-dev \
       libcurl4-openssl-dev \
       libboost-json1.83-dev \
       libjsoncpp-dev \
       liblz4-dev \
       libtool \
       libsasl2-dev \
       linux-tools-generic \
       libprotobuf-c-dev \
       protobuf-c-compiler \
       python3 \
       rapidjson-dev \
       libssl-dev \
       libzstd-dev \
       ninja-build \
       pkg-config \
       zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

RUN case "${TARGETARCH}" in \
      amd64) BUF_ARCH=x86_64 ;; \
      arm64) BUF_ARCH=aarch64 ;; \
      *) echo "Unsupported architecture: ${TARGETARCH}" >&2; exit 1 ;; \
    esac \
    && curl --fail --silent --show-error --location \
       "https://github.com/bufbuild/buf/releases/download/v${BUF_VERSION}/buf-Linux-${BUF_ARCH}" \
       --output /usr/local/bin/buf \
    && chmod +x /usr/local/bin/buf \
       && buf --version

RUN case "${TARGETARCH}" in \
      amd64) CMAKE_ARCH=x86_64 ;; \
      arm64) CMAKE_ARCH=aarch64 ;; \
      *) echo "Unsupported architecture: ${TARGETARCH}" >&2; exit 1 ;; \
    esac \
    && curl --fail --silent --show-error --location \
       "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-${CMAKE_ARCH}.sh" \
       --output /tmp/cmake.sh \
    && chmod +x /tmp/cmake.sh \
    && /tmp/cmake.sh --skip-license --prefix=/usr/local \
    && rm /tmp/cmake.sh \
    && cmake --version

RUN git clone --depth 1 --branch "v${PROTOBUF_VERSION}" https://github.com/protocolbuffers/protobuf.git /opt/protobuf \
    && cmake -S /opt/protobuf -B /tmp/protobuf-build -G Ninja \
       -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=ON \
       -Dprotobuf_WITH_ZLIB=ON \
    && cmake --build /tmp/protobuf-build \
    && cmake --install /tmp/protobuf-build \
    && rm -rf /tmp/protobuf-build /opt/protobuf

RUN git clone --depth 1 --branch "v${RDKAFKA_VERSION}" https://github.com/confluentinc/librdkafka.git /opt/librdkafka \
    && cmake -S /opt/librdkafka -B /tmp/librdkafka-build -G Ninja \
       -DCMAKE_BUILD_TYPE=Release -DRDKAFKA_BUILD_TESTS=OFF -DRDKAFKA_BUILD_EXAMPLES=OFF \
       -DWITH_ZSTD=ON -DWITH_LZ4_EXT=ON -DWITH_SASL=ON -DWITH_SSL=ON \
    && cmake --build /tmp/librdkafka-build \
    && cmake --install /tmp/librdkafka-build \
    && rm -rf /tmp/librdkafka-build /opt/librdkafka

RUN git clone --depth 1 --branch "${YYJSON_VERSION}" https://github.com/ibireme/yyjson.git /opt/yyjson \
    && cmake -S /opt/yyjson -B /tmp/yyjson-build -G Ninja \
       -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DYYJSON_BUILD_TESTS=OFF \
    && cmake --build /tmp/yyjson-build \
    && cmake --install /tmp/yyjson-build \
    && rm -rf /tmp/yyjson-build /opt/yyjson

RUN git clone --depth 1 --branch "v${NLOHMANN_JSON_VERSION}" https://github.com/nlohmann/json.git /opt/nlohmann-json \
    && cmake -S /opt/nlohmann-json -B /tmp/nlohmann-json-build -G Ninja \
       -DCMAKE_BUILD_TYPE=Release -DJSON_BuildTests=OFF \
    && cmake --build /tmp/nlohmann-json-build \
    && cmake --install /tmp/nlohmann-json-build \
    && ldconfig \
    && rm -rf /tmp/nlohmann-json-build /opt/nlohmann-json

WORKDIR /work
COPY . /work

ENTRYPOINT ["/work/scripts/container_benchmark.sh"]
