#!/usr/bin/env bash

build() {
    echo "Building Market Data Dissemination Simulator with CMake..."
    cmake -S . -B build/
    make -C build/
}

clean() {
    echo "Clearing Market Data Dissemination Simulator"
    cmake --build build --target clean
    rm -f MarketDataSimulatorServer
}

case "$1" in
    build)
        build
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {build|clean}"
        exit 1
        ;;
esac