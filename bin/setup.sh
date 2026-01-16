#!/usr/bin/env bash

install() {
    echo "Installing packages..."
    conan install . --build=missing
    cmake --preset conan-release
}

build() {
    echo "Building Market Data Dissemination Simulator with CMake..."
    cmake --build --preset conan-release
    make -C build/Release
}

clean() {
    echo "Cleaning Market Data Dissemination Simulator..."
    cmake --build --target clean --preset conan-release
    rm -f MarketDataSimulatorServer
    echo "Done."
}

case "$1" in
    install)
        install
        ;;
    build)
        build
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {install|build|clean}"
        exit 1
        ;;
esac