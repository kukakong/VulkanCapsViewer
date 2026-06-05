#!/bin/bash
# ============================================================
# Vulkan Caps Viewer - Android Build Script
# ============================================================
# This script builds the Vulkan Caps Viewer for Android
# Supporting ARM32 (armeabi-v7a) and ARM64 (arm64-v8a)
# ============================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Vulkan Caps Viewer - Android Builder${NC}"
echo -e "${GREEN}========================================${NC}"

# Configuration
PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build_android"
OUTPUT_DIR="${PROJECT_DIR}/dist/android"

# Qt Configuration (Modify these paths according to your environment)
QT_VERSION="${QT_VERSION:-6.8.3}"
QT_ANDROID_DIR="${QT_ANDROID_DIR:-/opt/Qt/${QT_VERSION}/android}"
ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-/opt/android-sdk}"
ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-${ANDROID_SDK_ROOT}/ndk/25.2.9519653}"

# Check required tools
check_requirements() {
    echo -e "\n${YELLOW}Checking requirements...${NC}"
    
    local missing=0
    
    # Check Qt
    if [ ! -d "$QT_ANDROID_DIR" ]; then
        echo -e "${RED}ERROR: Qt Android not found at: $QT_ANDROID_DIR${NC}"
        echo "Please set QT_ANDROID_DIR environment variable"
        missing=1
    else
        echo -e "${GREEN}✓ Qt found at: $QT_ANDROID_DIR${NC}"
    fi
    
    # Check Android SDK
    if [ ! -d "$ANDROID_SDK_ROOT" ]; then
        echo -e "${RED}ERROR: Android SDK not found at: $ANDROID_SDK_ROOT${NC}"
        echo "Please set ANDROID_SDK_ROOT environment variable"
        missing=1
    else
        echo -e "${GREEN}✓ Android SDK found at: $ANDROID_SDK_ROOT${NC}"
    fi
    
    # Check Android NDK
    if [ ! -d "$ANDROID_NDK_ROOT" ]; then
        echo -e "${RED}ERROR: Android NDK not found at: $ANDROID_NDK_ROOT${NC}"
        echo "Please set ANDROID_NDK_ROOT environment variable"
        missing=1
    else
        echo -e "${GREEN}✓ Android NDK found at: $ANDROID_NDK_ROOT${NC}"
    fi
    
    # Check qmake
    QMAKE="${QT_ANDROID_DIR}/bin/qmake"
    if [ ! -f "$QMAKE" ]; then
        echo -e "${RED}ERROR: qmake not found at: $QMAKE${NC}"
        missing=1
    else
        echo -e "${GREEN}✓ qmake found${NC}"
    fi
    
    if [ $missing -eq 1 ]; then
        echo -e "\n${RED}Missing requirements. Please install them first.${NC}"
        exit 1
    fi
}

# Build for specific architecture
build_arch() {
    local arch=$1
    local arch_name=$2
    
    echo -e "\n${YELLOW}Building for $arch_name ($arch)...${NC}"
    
    local build_dir="${BUILD_DIR}/${arch}"
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Run qmake
    "${QT_ANDROID_DIR}/bin/qmake" \
        "${PROJECT_DIR}/vulkanCapsViewer.pro" \
        -spec android-clang \
        "ANDROID_ABIS=${arch}" \
        "ANDROID_SDK_ROOT=${ANDROID_SDK_ROOT}" \
        "ANDROID_NDK_ROOT=${ANDROID_NDK_ROOT}" \
        "QT_ANDROID_BUILD=${QT_ANDROID_DIR}" \
        CONFIG+=release
    
    # Build
    make -j$(nproc)
    
    echo -e "${GREEN}✓ Build completed for $arch_name${NC}"
}

# Create APK
create_apk() {
    local arch=$1
    local arch_name=$2
    
    echo -e "\n${YELLOW}Creating APK for $arch_name...${NC}"
    
    local build_dir="${BUILD_DIR}/${arch}"
    local output_apk="${OUTPUT_DIR}/VulkanCapsViewer-${arch_name}.apk"
    
    mkdir -p "${OUTPUT_DIR}"
    
    # Use androiddeployqt to create APK
    "${QT_ANDROID_DIR}/bin/androiddeployqt" \
        --input "${build_dir}/android-vulkanCapsViewer-deployment-settings.json" \
        --output "${build_dir}/android-build" \
        --android-platform android-33 \
        --gradle \
        --release
    
    # Copy APK to output directory
    cp "${build_dir}/android-build/build/outputs/apk/release/android-build-release-unsigned.apk" "$output_apk"
    
    echo -e "${GREEN}✓ APK created: $output_apk${NC}"
}

# Main build process
main() {
    check_requirements
    
    # Create directories
    mkdir -p "${BUILD_DIR}"
    mkdir -p "${OUTPUT_DIR}"
    
    # Build for ARM64 (arm64-v8a)
    build_arch "arm64-v8a" "arm64"
    create_apk "arm64-v8a" "arm64"
    
    # Build for ARM32 (armeabi-v7a)
    build_arch "armeabi-v7a" "arm32"
    create_apk "armeabi-v7a" "arm32"
    
    echo -e "\n${GREEN}========================================${NC}"
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo -e "\nOutput files:"
    echo "  - ${OUTPUT_DIR}/VulkanCapsViewer-arm64.apk"
    echo "  - ${OUTPUT_DIR}/VulkanCapsViewer-arm32.apk"
}

# Run main
main "$@"
