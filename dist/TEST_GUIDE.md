# Vulkan Caps Viewer - 测试指南

## 目录
- [概述](#概述)
- [Android平台测试](#android平台测试)
- [Linux ARM平台测试](#linux-arm平台测试)
- [依赖库配置](#依赖库配置)
- [常见问题](#常见问题)

---

## 概述

本文档提供了 Vulkan Caps Viewer 在 Android 和 Linux ARM 平台上的构建、部署和测试说明。

### 支持的平台架构

| 平台 | 架构 | 说明 |
|------|------|------|
| Android | arm64-v8a | 64位ARM设备 |
| Android | armeabi-v7a | 32位ARM设备 |
| Linux | aarch64 | 64位ARM系统 |
| Linux | armhf | 32位ARM系统 |

---

## Android平台测试

### 1. 环境要求

#### 必需软件
- **Qt 6.8.3+** (Android版本)
- **Android SDK** (API Level 33+)
- **Android NDK** (r25+)
- **JDK 11+**
- **Gradle 8.0+**

#### 环境变量设置
```bash
# 设置Qt路径
export QT_ANDROID_DIR=/opt/Qt/6.8.3/android

# 设置Android SDK路径
export ANDROID_SDK_ROOT=/opt/android-sdk

# 设置Android NDK路径
export ANDROID_NDK_ROOT=$ANDROID_SDK_ROOT/ndk/25.2.9519653

# 设置Java路径
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk
```

### 2. 构建APK

#### 方法一：使用构建脚本
```bash
cd /path/to/VulkanCapsViewer/dist
chmod +x build_android.sh
./build_android.sh
```

#### 方法二：使用Qt Creator
1. 打开 Qt Creator
2. 打开项目文件 `vulkanCapsViewer.pro`
3. 选择 Android 编译套件 (arm64-v8a 或 armeabi-v7a)
4. 点击 构建 → 构建项目
5. 构建完成后，APK 位于 `build-android/android-build/build/outputs/apk/`

#### 方法三：命令行手动构建
```bash
# 创建构建目录
mkdir build_android && cd build_android

# 配置项目 (ARM64)
/opt/Qt/6.8.3/android/bin/qmake \
    ../vulkanCapsViewer.pro \
    -spec android-clang \
    "ANDROID_ABIS=arm64-v8a" \
    "ANDROID_SDK_ROOT=/opt/android-sdk" \
    "ANDROID_NDK_ROOT=/opt/android-sdk/ndk/25.2.9519653" \
    CONFIG+=release

# 编译
make -j$(nproc)

# 生成APK
/opt/Qt/6.8.3/android/bin/androiddeployqt \
    --input android-vulkanCapsViewer-deployment-settings.json \
    --output android-build \
    --android-platform android-33 \
    --gradle \
    --release
```

### 3. 安装APK

#### 使用adb安装
```bash
# ARM64设备
adb install VulkanCapsViewer-arm64.apk

# ARM32设备
adb install VulkanCapsViewer-arm32.apk

# 如果已安装，使用-r覆盖安装
adb install -r VulkanCapsViewer-arm64.apk
```

### 4. Android依赖库配置

Vulkan Caps Viewer 在 Android 平台上需要 Mali GPU 驱动库支持。

#### Mali库位置
```
ARM32: /vendor/lib/egl/libGLES_mali.so
ARM64: /vendor/lib64/egl/libGLES_mali.so
```

#### 检查设备上的Mali库
```bash
# 通过adb检查
adb shell ls -la /vendor/lib/egl/libGLES_mali.so
adb shell ls -la /vendor/lib64/egl/libGLES_mali.so

# 或者检查所有egl库
adb shell ls -la /vendor/lib/egl/
adb shell ls -la /vendor/lib64/egl/
```

#### 如果Mali库不存在
1. 确保设备使用的是ARM Mali GPU
2. 更新设备驱动程序
3. 联系设备制造商获取驱动支持

### 5. 运行测试

#### 启动应用
```bash
# 通过adb启动
adb shell am start -n de.saschawillems.vulkancapsviewer/org.qtproject.qt.android.bindings.QtActivity

# 或者直接在设备上点击应用图标
```

#### 查看日志
```bash
# 实时查看应用日志
adb logcat -s VulkanCapsViewer

# 查看所有Vulkan相关日志
adb logcat | grep -i vulkan

# 保存日志到文件
adb logcat > vulkan_caps_viewer.log
```

#### 测试检查项
- [ ] 应用正常启动
- [ ] 能够检测到GPU设备
- [ ] 显示设备属性信息
- [ ] 显示设备特性信息
- [ ] 显示支持的扩展列表
- [ ] 显示支持的格式列表
- [ ] 能够保存报告
- [ ] 能够上传报告到数据库

---

## Linux ARM平台测试

### 1. 环境要求

#### 必需软件
- **Qt 6.8.3+** (ARM版本或交叉编译工具链)
- **Vulkan SDK** 或 **Mali驱动**
- **CMake 3.10+** 或 **qmake**
- **GCC/Clang** (支持C++20)

#### 对于交叉编译
- ARM32: `arm-linux-gnueabihf-g++`
- ARM64: `aarch64-linux-gnu-g++`

### 2. 构建可执行文件

#### 方法一：使用构建脚本
```bash
cd /path/to/VulkanCapsViewer/dist
chmod +x build_linux_arm.sh
./build_linux_arm.sh
```

#### 方法二：在ARM设备上原生编译
```bash
# 创建构建目录
mkdir build && cd build

# 配置
qmake6 ../vulkanCapsViewer.pro CONFIG+=release DEFINES+=X11

# 或者使用CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
make -j$(nproc)
```

#### 方法三：交叉编译

**ARM64 (aarch64):**
```bash
mkdir build_arm64 && cd build_arm64

qmake6 ../vulkanCapsViewer.pro \
    CONFIG+=release \
    DEFINES+=X11 \
    QMAKE_CXX="aarch64-linux-gnu-g++" \
    QMAKE_CC="aarch64-linux-gnu-gcc" \
    QMAKE_LINK="aarch64-linux-gnu-g++"

make -j$(nproc)
aarch64-linux-gnu-strip vulkanCapsViewer
```

**ARM32 (armhf):**
```bash
mkdir build_arm32 && cd build_arm32

qmake6 ../vulkanCapsViewer.pro \
    CONFIG+=release \
    DEFINES+=X11 \
    QMAKE_CXX="arm-linux-gnueabihf-g++" \
    QMAKE_CC="arm-linux-gnueabihf-gcc" \
    QMAKE_LINK="arm-linux-gnueabihf-g++"

make -j$(nproc)
arm-linux-gnueabihf-strip vulkanCapsViewer
```

### 3. Linux依赖库配置

Vulkan Caps Viewer 在 Linux ARM 平台上需要 Mali GPU 驱动库支持。

#### Mali库可能位置

**标准位置:**
```
/lib/libmali.so
/lib64/libmali.so
/lib/libMali.so
/lib64/libMali.so
```

**用户空间位置:**
```
/usr/lib/libmali.so
/usr/lib64/libmali.so
/usr/lib/libMali.so
/usr/lib64/libMali.so
```

#### 检查Mali库
```bash
# 查找所有可能的Mali库
find /lib /usr/lib -name "*mali*" -o -name "*Mali*" 2>/dev/null

# 查看库详情
ldd /path/to/vulkanCapsViewer

# 检查Vulkan加载器
ls -la /usr/lib/libvulkan.so*
ls -la /usr/lib64/libvulkan.so*
```

#### 配置库路径

如果Mali库不在标准路径，需要配置：

```bash
# 方法一：设置LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/mali/lib:$LD_LIBRARY_PATH

# 方法二：创建符号链接
sudo ln -s /path/to/libmali.so /usr/lib/libvulkan.so.1

# 方法三：更新ldconfig
echo "/path/to/mali/lib" | sudo tee /etc/ld.so.conf.d/mali.conf
sudo ldconfig
```

### 4. 运行测试

#### 运行应用
```bash
# 图形界面模式
./vulkanCapsViewer

# 保存报告模式
./vulkanCapsViewer -s report.json

# 上传报告模式
./vulkanCapsViewer --upload --deviceindex 0 --submitter "Your Name"

# 启用日志
./vulkanCapsViewer --log
```

#### 检查Vulkan支持
```bash
# 检查Vulkan实例扩展
vulkaninfo --summary

# 检查设备信息
vulkaninfo | head -50

# 列出所有GPU
vulkaninfo | grep "Device Name"
```

#### 测试检查项
- [ ] 应用正常启动
- [ ] Vulkan实例创建成功
- [ ] 检测到GPU设备
- [ ] 显示完整的设备属性
- [ ] 显示所有支持的扩展
- [ ] 格式支持信息正确
- [ ] 队列族信息正确
- [ ] 报告保存功能正常
- [ ] 网络上传功能正常（如需要）

---

## 依赖库配置

### Mali GPU驱动配置

#### Android平台

Mali库通常由设备厂商预装，位置：
- **ARM32**: `/vendor/lib/egl/libGLES_mali.so`
- **ARM64**: `/vendor/lib64/egl/libGLES_mali.so`

如果缺少，需要：
1. 从设备厂商获取驱动包
2. 通过系统更新安装
3. 使用设备特定的驱动安装工具

#### Linux平台

Mali库可能位于多个位置：

| 路径 | 说明 |
|------|------|
| `/lib/libmali.so` | 系统库目录 (32位) |
| `/lib64/libmali.so` | 系统库目录 (64位) |
| `/usr/lib/libmali.so` | 用户库目录 (32位) |
| `/usr/lib64/libmali.so` | 用户库目录 (64位) |

### 驱动安装示例

#### ARM Mali GPU (Rockchip平台示例)
```bash
# 安装Mali驱动包
sudo apt install mali-fbdev
# 或
sudo apt install libmali-rockchip

# 配置环境
export MALI_PLATFORM=rk3399
```

#### ARM Mali GPU (Allwinner平台示例)
```bash
# 安装sunxi-mali驱动
sudo apt install sunxi-mali

# 配置
export MALI_PLATFORM=sun50i
```

### Vulkan Loader配置

确保系统安装了Vulkan Loader：

```bash
# Debian/Ubuntu
sudo apt install libvulkan1 vulkan-tools

# Fedora/RHEL
sudo dnf install vulkan-loader vulkan-tools

# Arch Linux
sudo pacman -S vulkan-icd-loader vulkan-tools
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
1. 检查GPU驱动是否正确安装
2. 确认GPU支持Vulkan API
3. 检查Vulkan ICD配置文件

```bash
# 检查ICD配置
ls /etc/vulkan/icd.d/
cat /etc/vulkan/icd.d/*.json

# 如果没有ICD配置，创建一个
sudo tee /etc/vulkan/icd.d/mali.json << EOF
{
    "file_format_version" : "1.0.0",
    "ICD": {
        "library_path": "/usr/lib/libmali.so",
        "api_version" : "1.3"
    }
}
EOF
```

### 3. Android应用闪退

**症状**: 应用启动后立即关闭

**解决方案**:
```bash
# 查看详细日志
adb logcat -s AndroidRuntime:E

# 检查Vulkan支持
adb shell dumpsys gpu

# 检查库依赖
adb shell readelf -d /data/app/de.saschawillems.vulkancapsviewer/lib/arm64/libvulkanCapsViewer.so
```

### 4. 无法检测到GPU

**症状**: 设备列表为空

**解决方案**:
1. 确认设备有支持Vulkan的GPU
2. 检查驱动版本是否支持Vulkan
3. 更新GPU驱动

```bash
# Linux检查
lspci | grep -i vga
glxinfo | grep "OpenGL renderer"

# Android检查
adb shell dumpsys SurfaceFlinger | grep -i gpu
```

### 5. 报告上传失败

**症状**: 上传报告时出现网络错误

**解决方案**:
1. 检查网络连接
2. 确认数据库服务器可访问
3. 检查代理设置

```bash
# 测试数据库连接
curl -I https://vulkan.gpuinfo.org/

# 配置代理（如需要）
export http_proxy=http://proxy:port
export https_proxy=http://proxy:port
```

### 6. Qt库版本问题

**症状**: `version 'Qt_6' not found`

**解决方案**:
```bash
# 确保Qt版本匹配
qmake6 --version

# 设置Qt库路径
export LD_LIBRARY_PATH=/opt/Qt/6.8.3/lib:$LD_LIBRARY_PATH
```

---

## 测试报告模板

### 设备信息
- 设备型号:
- 操作系统:
- CPU架构:
- GPU型号:

### 测试结果

| 测试项 | 结果 | 备注 |
|--------|------|------|
| 应用启动 | ☐ 通过 ☐ 失败 | |
| GPU检测 | ☐ 通过 ☐ 失败 | |
| 属性显示 | ☐ 通过 ☐ 失败 | |
| 特性显示 | ☐ 通过 ☐ 失败 | |
| 扩展列表 | ☐ 通过 ☐ 失败 | |
| 格式列表 | ☐ 通过 ☐ 失败 | |
| 报告保存 | ☐ 通过 ☐ 失败 | |
| 报告上传 | ☐ 通过 ☐ 失败 | |

### 问题描述
(如有问题，请详细描述)

---

*文档版本: 1.0*
*更新时间: 2026-06-05*
