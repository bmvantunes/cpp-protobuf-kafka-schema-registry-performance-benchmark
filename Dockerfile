FROM ubuntu:24.04

ARG BUF_VERSION=1.50.0
ARG TARGETARCH

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       build-essential \
       ca-certificates \
       cmake \
       curl \
       git \
       libcurl4-openssl-dev \
       libboost-json1.83-dev \
       libjsoncpp-dev \
       libprotobuf-c-dev \
       libprotobuf-dev \
       nlohmann-json3-dev \
       protobuf-c-compiler \
       protobuf-compiler \
       python3 \
       rapidjson-dev \
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

RUN git clone --depth 1 --branch 0.10.0 https://github.com/ibireme/yyjson.git /opt/yyjson \
    && cmake -S /opt/yyjson -B /tmp/yyjson-build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DYYJSON_BUILD_TESTS=OFF \
    && cmake --build /tmp/yyjson-build --parallel \
    && cmake --install /tmp/yyjson-build \
    && rm -rf /tmp/yyjson-build /opt/yyjson/.git

WORKDIR /work
COPY . /work

ENTRYPOINT ["/work/scripts/container_benchmark.sh"]
