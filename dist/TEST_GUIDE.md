# Vulkan Caps Viewer - 测试指南

## 目录
- [概述](#概述)
- [编译产物](#编译产物)
- [Linux ARM平台测试](#linux-arm平台测试)
- [Android平台测试](#android平台测试)
- [依赖库配置](#依赖库配置)
- [Vulkan能力测试程序](#vulkan能力测试程序)
- [常见问题](#常见问题)

---

## 概述

本文档提供了 Vulkan Caps Viewer 在 Android 和 Linux ARM 平台上的部署和测试说明。包含预编译的二进制文件和独立的 Vulkan 能力测试程序。

### 支持的平台架构

| 平台 | 架构 | 说明 |
|------|------|------|
| Linux | x86_64 | 64位x86系统 |
| Linux | aarch64 | 64位ARM系统 |
| Linux | armhf | 32位ARM系统 |
| Android | arm64-v8a | 64位ARM设备 |
| Android | armeabi-v7a | 32位ARM设备 |

---

## 编译产物

### 文件清单

```
dist/
├── bin/
│   ├── linux-x86_64/
│   │   ├── vulkanCapsViewer          # 完整GUI应用 (1.2MB)
│   │   └── vulkan_caps_test          # 独立测试程序 (58KB)
│   ├── linux-arm64/
│   │   ├── vulkanCapsViewer          # 完整GUI应用 (1.1MB)
│   │   └── vulkan_caps_test          # 独立测试程序 (67KB)
│   ├── linux-arm32/
│   │   └── vulkan_caps_test          # 独立测试程序 (45KB)
│   ├── android-arm64/
│   │   └── vulkan_caps_test          # Android测试程序 (478KB, 静态链接C++库)
│   └── android-arm32/
│       └── vulkan_caps_test          # Android测试程序 (283KB, 静态链接C++库)
├── vulkan_caps_test.cpp              # 测试程序源代码
├── CMakeLists.txt                    # 测试程序构建文件
├── toolchain-aarch64.cmake          # ARM64交叉编译工具链
├── toolchain-armhf.cmake            # ARM32交叉编译工具链
├── build_android.sh                  # Android构建脚本
├── build_linux_arm.sh                # Linux ARM构建脚本
├── CODE_WIKI.md                      # 项目架构文档
├── CODE_WIKI.html                    # 项目架构HTML文档
├── TEST_GUIDE.md                     # 本文档
└── TEST_GUIDE.html                   # 本文档HTML版本
```

### 两种测试工具

| 工具 | 类型 | 依赖 | 说明 |
|------|------|------|------|
| `vulkanCapsViewer` | 完整GUI应用 | Qt6, Vulkan | 完整的图形界面应用 |
| `vulkan_caps_test` | 命令行工具 | 仅Vulkan | 轻量级测试，适合无GUI环境 |

---

## Linux ARM平台测试

### 1. 部署文件到目标设备

```bash
# 方法一：使用scp传输
scp dist/bin/linux-arm64/vulkanCapsViewer user@target:/tmp/
scp dist/bin/linux-arm64/vulkan_caps_test user@target:/tmp/

# 方法二：使用USB存储
cp dist/bin/linux-arm64/* /media/usb/

# 方法三：使用adb（如果设备支持）
adb push dist/bin/linux-arm64/vulkan_caps_test /data/local/tmp/
```

### 2. 运行Vulkan能力测试程序

```bash
# 添加执行权限
chmod +x vulkan_caps_test

# 基本运行 - 显示所有设备信息
./vulkan_caps_test

# JSON格式输出
./vulkan_caps_test --json

# 保存报告到文件
./vulkan_caps_test --json -o report.json

# 显示帮助
./vulkan_caps_test --help
```

### 3. 运行完整GUI应用

```bash
# 添加执行权限
chmod +x vulkanCapsViewer

# 运行（需要X11或Wayland显示服务）
./vulkanCapsViewer

# 保存报告
./vulkanCapsViewer -s report.json

# 命令行上传报告
./vulkanCapsViewer --upload --deviceindex 0 --submitter "Your Name"
```

### 4. 依赖库检查

```bash
# 检查动态库依赖
ldd ./vulkan_caps_test
ldd ./vulkanCapsViewer

# 检查Vulkan库
ls -la /usr/lib/libvulkan.so* /usr/lib64/libvulkan.so*

# 检查Mali库
ls -la /lib/libmali.so /lib64/libmali.so /lib/libMali.so /lib64/libMali.so 2>/dev/null
ls -la /usr/lib/libmali.so /usr/lib64/libmali.so 2>/dev/null
```

---

## Android平台测试

### 1. 部署测试程序到设备

```bash
# 确认设备连接
adb devices

# 推送ARM64测试程序
adb push dist/bin/android-arm64/vulkan_caps_test /data/local/tmp/

# 推送ARM32测试程序
adb push dist/bin/android-arm32/vulkan_caps_test /data/local/tmp/

# 添加执行权限
adb shell chmod +x /data/local/tmp/vulkan_caps_test
```

### 2. 运行测试

```bash
# 运行测试
adb shell /data/local/tmp/vulkan_caps_test

# JSON格式输出
adb shell /data/local/tmp/vulkan_caps_test --json

# 保存报告到设备
adb shell /data/local/tmp/vulkan_caps_test --json -o /data/local/tmp/report.json

# 拉取报告到本地
adb pull /data/local/tmp/report.json ./
```

### 3. 检查Mali GPU库

```bash
# 检查Mali库
adb shell ls -la /vendor/lib/egl/libGLES_mali.so
adb shell ls -la /vendor/lib64/egl/libGLES_mali.so

# 检查Vulkan支持
adb shell dumpsys gpu

# 检查GPU信息
adb shell cat /sys/class/kgsl/kgsl-3d0/gpu_model
adb shell cat /sys/class/misc/mali0/device/gpuinfo
```

### 4. 安装完整APK（如有）

```bash
# 安装Vulkan Caps Viewer APK
adb install VulkanCapsViewer-arm64.apk

# 启动应用
adb shell am start -n de.saschawillems.vulkancapsviewer/org.qtproject.qt.android.bindings.QtActivity

# 查看日志
adb logcat -s VulkanCapsViewer
```

---

## 依赖库配置

### Linux平台 - Mali GPU驱动

Mali库可能位于以下位置：

| 路径 | 说明 |
|------|------|
| `/lib/libmali.so` | 系统库目录 (32位) |
| `/lib64/libmali.so` | 系统库目录 (64位) |
| `/lib/libMali.so` | 系统库目录 (32位, 大写) |
| `/lib64/libMali.so` | 系统库目录 (64位, 大写) |
| `/usr/lib/libmali.so` | 用户库目录 (32位) |
| `/usr/lib64/libmali.so` | 用户库目录 (64位) |

#### 配置Mali库为Vulkan ICD

```bash
# 查找Mali库
MALI_LIB=$(find /lib /usr/lib -name "libmali.so" -o -name "libMali.so" 2>/dev/null | head -1)
echo "Found Mali library: $MALI_LIB"

# 创建Vulkan ICD配置
sudo mkdir -p /etc/vulkan/icd.d/
sudo tee /etc/vulkan/icd.d/mali.json << EOF
{
    "file_format_version": "1.0.0",
    "ICD": {
        "library_path": "$MALI_LIB",
        "api_version": "1.3"
    }
}
EOF

# 或者设置环境变量
export VK_ICD_FILENAMES=/etc/vulkan/icd.d/mali.json
```

#### 配置库搜索路径

```bash
# 方法一：设置LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/mali/lib:$LD_LIBRARY_PATH

# 方法二：创建符号链接
sudo ln -sf /path/to/libmali.so /usr/lib/libvulkan.so.1

# 方法三：更新ldconfig
echo "/path/to/mali/lib" | sudo tee /etc/ld.so.conf.d/mali.conf
sudo ldconfig
```

### Android平台 - Mali GPU驱动

Mali库位置（由设备厂商预装）：

| 路径 | 说明 |
|------|------|
| `/vendor/lib/egl/libGLES_mali.so` | ARM32 Mali库 |
| `/vendor/lib64/egl/libGLES_mali.so` | ARM64 Mali库 |

#### 检查Mali库

```bash
# 检查Mali库是否存在
adb shell ls -la /vendor/lib/egl/libGLES_mali.so
adb shell ls -la /vendor/lib64/egl/libGLES_mali.so

# 检查Vulkan ICD
adb shell ls -la /vendor/etc/vulkan/icd.d/

# 检查GPU驱动信息
adb shell dumpsys gpu
```

---

## Vulkan能力测试程序

### 程序说明

`vulkan_caps_test` 是一个独立的命令行工具，用于检测和报告Vulkan设备能力信息。

**特点**：
- 不依赖Qt，仅需Vulkan运行时
- 支持所有ARM平台（Linux/Android, 32/64位）
- 输出纯文本或JSON格式
- 可保存报告到文件

### 检测内容

| 类别 | 信息 |
|------|------|
| 设备属性 | 名称、类型、API版本、驱动版本、VendorID、DeviceID |
| 扩展 | 所有支持的设备扩展 |
| 层 | 所有支持的设备层 |
| 内存 | 内存类型和堆（大小、标志位） |
| 队列 | 队列族属性（数量、标志位） |
| 特性 | Core 1.0 全部55个VkBool32特性字段 |
| 格式 | 50种常见格式支持情况（含ETC2/EAC/ASTC压缩格式） |
| Surface | Surface能力（如支持） |

### 使用示例

```bash
# 基本使用
./vulkan_caps_test

# JSON输出
./vulkan_caps_test --json

# 保存报告
./vulkan_caps_test --json -o device_report.json

# 在Android设备上运行
adb shell /data/local/tmp/vulkan_caps_test --json -o /data/local/tmp/report.json
adb pull /data/local/tmp/report.json
```

### 从源代码编译

```bash
# 原生编译
cd vulkan_test
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 交叉编译 ARM64
cmake .. -DCMAKE_TOOLCHAIN_FILE=../dist/toolchain-aarch64.cmake -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 交叉编译 ARM32
cmake .. -DCMAKE_TOOLCHAIN_FILE=../dist/toolchain-armhf.cmake -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Android ARM64 (需要Android NDK)
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-33 \
    -DANDROID_STL=c++_static \
    -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## 常见问题

### 1. 找不到Vulkan库

**症状**: `error while loading shared libraries: libvulkan.so.1`

**解决方案**:
```bash
# 安装Vulkan loader
sudo apt install libvulkan1

# 或设置库路径
export LD_LIBRARY_PATH=/path/to/vulkan/lib:$LD_LIBRARY_PATH
```

### 2. 无法创建Vulkan实例

**症状**: `Failed to create Vulkan instance`

**解决方案**:
```bash
# 检查ICD配置
ls /etc/vulkan/icd.d/
cat /etc/vulkan/icd.d/*.json

# 创建Mali ICD配置
sudo tee /etc/vulkan/icd.d/mali.json << EOF
{
    "file_format_version": "1.0.0",
    "ICD": {
        "library_path": "/usr/lib/libmali.so",
        "api_version": "1.3"
    }
}
EOF
```

### 3. Android上无法执行测试程序

**症状**: `Permission denied` 或 `No such file or directory`

**解决方案**:
```bash
# 确保文件有执行权限
adb shell chmod +x /data/local/tmp/vulkan_caps_test

# 确认文件架构匹配
adb shell file /data/local/tmp/vulkan_caps_test

# 检查CPU架构
adb shell getprop ro.product.cpu.abi
```

### 4. 无法检测到GPU

**症状**: 设备列表为空

**解决方案**:
1. 确认设备有支持Vulkan的GPU
2. 检查驱动版本是否支持Vulkan
3. 确认Mali库路径正确

```bash
# Linux检查
lspci | grep -i vga
glxinfo | grep "OpenGL renderer"

# Android检查
adb shell dumpsys SurfaceFlinger | grep -i gpu
adb shell cat /proc/gpuinfo 2>/dev/null
```

### 5. Qt6库缺失（仅GUI应用）

**症状**: `error while loading shared libraries: libQt6Widgets.so.6`

**解决方案**:
```bash
# 安装Qt6
sudo apt install qt6-base-dev

# 或使用vulkan_caps_test替代（无需Qt）
./vulkan_caps_test --json
```

---

## 测试报告模板

### 设备信息
- 设备型号:
- 操作系统:
- CPU架构:
- GPU型号:
- Mali库路径:

### 测试结果

| 测试项 | 结果 | 备注 |
|--------|------|------|
| vulkan_caps_test运行 | ☐ 通过 ☐ 失败 | |
| GPU检测 | ☐ 通过 ☐ 失败 | |
| 设备属性显示 | ☐ 通过 ☐ 失败 | |
| 扩展列表 | ☐ 通过 ☐ 失败 | |
| 格式支持 | ☐ 通过 ☐ 失败 | |
| JSON报告生成 | ☐ 通过 ☐ 失败 | |
| vulkanCapsViewer运行 | ☐ 通过 ☐ 失败 | 仅GUI版本 |
| 报告上传 | ☐ 通过 ☐ 失败 | 仅GUI版本 |

---

*文档版本: 2.0*
*更新时间: 2026-06-05*
*编译环境: Ubuntu 24.04, GCC 13.3.0, Qt 6.4.2, Android NDK r26d*
