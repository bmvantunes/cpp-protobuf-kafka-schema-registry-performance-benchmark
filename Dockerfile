FROM ubuntu:26.04

ARG BUF_VERSION=1.72.0
ARG BOOST_VERSION=1.91.0
ARG BOOST_ARCHIVE_VERSION=1_91_0
ARG BENCHMARK_BOOST_INSTALL_MODE=binary
ARG BENCHMARK_BOOST_REPORT_VERSION=${BOOST_VERSION}
ARG CMAKE_VERSION=4.4.2
ARG BENCHMARK_CMAKE_INSTALL_MODE=binary
ARG PROTOBUF_VERSION=35.1
ARG PROTOBUF_C_VERSION=1.5.2
ARG RDKAFKA_VERSION=2.15.0
ARG YYJSON_VERSION=0.12.0
ARG NLOHMANN_JSON_VERSION=3.12.0
ARG JSONCPP_VERSION=1.9.8
ARG TARGETARCH

COPY patches/protobuf-c-compat.h /tmp/protobuf-c-compat.h

ENV DEBIAN_FRONTEND=noninteractive \
    LD_LIBRARY_PATH=/usr/local/lib \
    PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/local/share/pkgconfig \
    BENCHMARK_BUF_VERSION=${BUF_VERSION} \
    BENCHMARK_BOOST_VERSION=${BENCHMARK_BOOST_REPORT_VERSION} \
    BENCHMARK_CMAKE_VERSION=${CMAKE_VERSION} \
    BENCHMARK_PROTOBUF_VERSION=${PROTOBUF_VERSION} \
    BENCHMARK_PROTOBUF_C_VERSION=${PROTOBUF_C_VERSION} \
    BENCHMARK_RDKAFKA_VERSION=${RDKAFKA_VERSION} \
    BENCHMARK_YYJSON_VERSION=${YYJSON_VERSION} \
    BENCHMARK_NLOHMANN_JSON_VERSION=${NLOHMANN_JSON_VERSION} \
    BENCHMARK_JSONCPP_VERSION=${JSONCPP_VERSION} \
    BENCHMARK_CLANG_VERSION=22.1.2

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       build-essential \
       clang-22 \
       lld-22 \
       autoconf \
       automake \
       ca-certificates \
       cmake \
       curl \
       git \
       libabsl-dev \
       libcurl4-openssl-dev \
       liblz4-dev \
       libtool \
       libsasl2-dev \
       linux-tools-generic \
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

RUN if [ "${BENCHMARK_CMAKE_INSTALL_MODE}" = "apt" ]; then \
      echo "Using Ubuntu-packaged CMake for emulation compatibility"; \
      cmake --version; \
    else \
      case "${TARGETARCH}" in \
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
      && cmake --version; \
    fi

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

RUN git clone --depth 1 --branch "${JSONCPP_VERSION}" https://github.com/open-source-parsers/jsoncpp.git /opt/jsoncpp \
    && cmake -S /opt/jsoncpp -B /tmp/jsoncpp-build -G Ninja \
       -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON \
       -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF \
       -DJSONCPP_WITH_EXAMPLE=OFF \
    && cmake --build /tmp/jsoncpp-build \
    && cmake --install /tmp/jsoncpp-build \
    && ldconfig \
    && rm -rf /tmp/jsoncpp-build /opt/jsoncpp

RUN git clone --depth 1 --branch "v${PROTOBUF_C_VERSION}" https://github.com/protobuf-c/protobuf-c.git /opt/protobuf-c \
    && sed -i 's/descriptor_->label()/GetFieldLabel(descriptor_)/g' /opt/protobuf-c/protoc-gen-c/*.cc \
    && for source in /opt/protobuf-c/protoc-gen-c/*.cc; do sed -i '1i#include "/tmp/protobuf-c-compat.h"' "${source}"; done \
    && cmake -S /opt/protobuf-c/build-cmake -B /tmp/protobuf-c-build -G Ninja \
       -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_PROTOC=ON \
    && cmake --build /tmp/protobuf-c-build \
    && cmake --install /tmp/protobuf-c-build \
    && ldconfig \
    && rm -rf /tmp/protobuf-c-build /opt/protobuf-c

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

RUN if [ "${BENCHMARK_BOOST_INSTALL_MODE}" = "apt" ]; then \
      echo "Using Ubuntu-packaged Boost.JSON for emulation compatibility"; \
      apt-get update \
      && apt-get install -y --no-install-recommends libboost-json-dev \
      && rm -rf /var/lib/apt/lists/* \
      && ldconfig; \
    else \
      curl --fail --silent --show-error --location \
         "https://archives.boost.io/release/${BOOST_VERSION}/source/boost_${BOOST_ARCHIVE_VERSION}.tar.gz" \
         --output /tmp/boost.tar.gz \
      && mkdir -p /opt/boost \
      && tar -xzf /tmp/boost.tar.gz --strip-components=1 -C /opt/boost \
      && cd /opt/boost \
      && ./bootstrap.sh --with-libraries=json --prefix=/usr/local \
      && ./b2 -j2 variant=release link=shared threading=multi install \
      && ldconfig \
      && rm -rf /tmp/boost.tar.gz /opt/boost; \
    fi

WORKDIR /work
COPY . /work

ENTRYPOINT ["/work/scripts/container_benchmark.sh"]
