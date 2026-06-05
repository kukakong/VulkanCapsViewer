#!/bin/bash
# ============================================================
# Vulkan Caps Viewer - Linux ARM Build Script
# ============================================================
# This script builds the Vulkan Caps Viewer for Linux ARM
# Supporting ARM32 (armhf) and ARM64 (aarch64)
# ============================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Vulkan Caps Viewer - Linux ARM Builder${NC}"
echo -e "${GREEN}========================================${NC}"

# Configuration
PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
OUTPUT_DIR="${PROJECT_DIR}/dist/linux"

# Qt Configuration
QT_VERSION="${QT_VERSION:-6.8.3}"

# Cross-compile toolchains
ARM32_TOOLCHAIN="${ARM32_TOOLCHAIN:-/usr/bin/arm-linux-gnueabihf-g++}"
ARM64_TOOLCHAIN="${ARM64_TOOLCHAIN:-/usr/bin/aarch64-linux-gnu-g++}"

# Check requirements
check_requirements() {
    echo -e "\n${YELLOW}Checking requirements...${NC}"
    
    # Check for cross-compiler
    if command -v arm-linux-gnueabihf-g++ &> /dev/null; then
        echo -e "${GREEN}✓ ARM32 cross-compiler found${NC}"
    else
        echo -e "${YELLOW}⚠ ARM32 cross-compiler not found, skipping ARM32 build${NC}"
        BUILD_ARM32=false
    fi
    
    if command -v aarch64-linux-gnu-g++ &> /dev/null; then
        echo -e "${GREEN}✓ ARM64 cross-compiler found${NC}"
    else
        echo -e "${YELLOW}⚠ ARM64 cross-compiler not found, skipping ARM64 build${NC}"
        BUILD_ARM64=false
    fi
    
    # Check qmake
    if command -v qmake6 &> /dev/null; then
        QMAKE="qmake6"
        echo -e "${GREEN}✓ qmake6 found${NC}"
    elif command -v qmake &> /dev/null; then
        QMAKE="qmake"
        echo -e "${GREEN}✓ qmake found${NC}"
    else
        echo -e "${RED}ERROR: qmake not found${NC}"
        exit 1
    fi
}

# Build for ARM64 (aarch64)
build_arm64() {
    echo -e "\n${YELLOW}Building for ARM64 (aarch64)...${NC}"
    
    local build_dir="${PROJECT_DIR}/build_linux_arm64"
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with qmake for cross-compilation
    $QMAKE \
        "${PROJECT_DIR}/vulkanCapsViewer.pro" \
        -spec linux-g++ \
        CONFIG+=release \
        DEFINES+=X11 \
        QMAKE_CXX="aarch64-linux-gnu-g++" \
        QMAKE_CC="aarch64-linux-gnu-gcc" \
        QMAKE_LINK="aarch64-linux-gnu-g++" \
        QMAKE_AR="aarch64-linux-gnu-ar" \
        QMAKE_RANLIB="aarch64-linux-gnu-ranlib" \
        QMAKE_STRIP="aarch64-linux-gnu-strip"
    
    # Build
    make -j$(nproc)
    
    # Strip binary
    aarch64-linux-gnu-strip vulkanCapsViewer
    
    # Copy to output
    mkdir -p "${OUTPUT_DIR}"
    cp vulkanCapsViewer "${OUTPUT_DIR}/vulkanCapsViewer-arm64"
    
    echo -e "${GREEN}✓ ARM64 build completed${NC}"
}

# Build for ARM32 (armhf)
build_arm32() {
    echo -e "\n${YELLOW}Building for ARM32 (armhf)...${NC}"
    
    local build_dir="${PROJECT_DIR}/build_linux_arm32"
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with qmake for cross-compilation
    $QMAKE \
        "${PROJECT_DIR}/vulkanCapsViewer.pro" \
        -spec linux-g++ \
        CONFIG+=release \
        DEFINES+=X11 \
        QMAKE_CXX="arm-linux-gnueabihf-g++" \
        QMAKE_CC="arm-linux-gnueabihf-gcc" \
        QMAKE_LINK="arm-linux-gnueabihf-g++" \
        QMAKE_AR="arm-linux-gnueabihf-ar" \
        QMAKE_RANLIB="arm-linux-gnueabihf-ranlib" \
        QMAKE_STRIP="arm-linux-gnueabihf-strip"
    
    # Build
    make -j$(nproc)
    
    # Strip binary
    arm-linux-gnueabihf-strip vulkanCapsViewer
    
    # Copy to output
    mkdir -p "${OUTPUT_DIR}"
    cp vulkanCapsViewer "${OUTPUT_DIR}/vulkanCapsViewer-arm32"
    
    echo -e "${GREEN}✓ ARM32 build completed${NC}"
}

# Native build (if running on ARM platform)
build_native() {
    echo -e "\n${YELLOW}Building natively...${NC}"
    
    local arch=$(uname -m)
    local build_dir="${PROJECT_DIR}/build_linux_native"
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure
    $QMAKE \
        "${PROJECT_DIR}/vulkanCapsViewer.pro" \
        CONFIG+=release \
        DEFINES+=X11
    
    # Build
    make -j$(nproc)
    
    # Strip binary
    strip vulkanCapsViewer
    
    # Copy to output
    mkdir -p "${OUTPUT_DIR}"
    cp vulkanCapsViewer "${OUTPUT_DIR}/vulkanCapsViewer-${arch}"
    
    echo -e "${GREEN}✓ Native build completed for ${arch}${NC}"
}

# Main
main() {
    BUILD_ARM32=true
    BUILD_ARM64=true
    
    check_requirements
    
    mkdir -p "${OUTPUT_DIR}"
    
    # Check if we're running on ARM platform
    local arch=$(uname -m)
    if [ "$arch" = "aarch64" ] || [ "$arch" = "armv7l" ]; then
        echo -e "${YELLOW}Running on ARM platform, building natively...${NC}"
        build_native
    else
        # Cross-compile
        if [ "$BUILD_ARM64" = true ]; then
            build_arm64
        fi
        
        if [ "$BUILD_ARM32" = true ]; then
            build_arm32
        fi
    fi
    
    echo -e "\n${GREEN}========================================${NC}"
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo -e "\nOutput files in: ${OUTPUT_DIR}"
    ls -la "${OUTPUT_DIR}"
}

main "$@"
