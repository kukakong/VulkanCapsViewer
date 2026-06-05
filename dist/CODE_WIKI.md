# Vulkan Hardware Capability Viewer - Code Wiki

## 目录
- [项目概述](#项目概述)
- [整体架构](#整体架构)
- [主要模块职责](#主要模块职责)
- [关键类与函数说明](#关键类与函数说明)
- [依赖关系](#依赖关系)
- [项目运行方式](#项目运行方式)
- [数据流程](#数据流程)
- [测试指南](#测试指南)

---

## 项目概述

**Vulkan Hardware Capability Viewer** 是一个用于显示支持 Vulkan API 的 GPU 硬件实现细节的客户端应用程序。该项目由 Sascha Willems 开发，基于 GNU LGPL 3.0 许可证开源。

### 核心功能
- 显示 Vulkan 兼容设备的硬件能力信息
- 支持将硬件报告提交到公共在线数据库
- 支持多种操作系统平台
- 提供图形化界面查看设备属性、特性、格式、扩展等信息

### 支持平台
- Windows (x64)
- Linux (x64)
- Android (包括 Android TV)
- macOS
- iOS

### 版本信息
- 应用版本：定义在 `vulkanCapsViewer.h` 中
- 报告版本：用于数据库提交格式
- Vulkan Headers 版本：349

---

## 整体架构

### 架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                        VulkanCapsViewer                         │
│                      (主窗口 QMainWindow)                        │
└─────────────────────────────────────────────────────────────────┘
                                │
                ┌───────────────┼───────────────┐
                │               │               │
                ▼               ▼               ▼
┌──────────────────┐  ┌─────────────────┐  ┌──────────────────┐
│  VulkanContext   │  │  VulkanDatabase │  │     Settings     │
│  (Vulkan上下文)   │  │  (数据库通信)    │  │    (设置管理)    │
└──────────────────┘  └─────────────────┘  └──────────────────┘
        │                      │
        ▼                      │
┌──────────────────────────────┼──────────────────────────────┐
│                    VulkanDeviceInfo                          │
│                   (设备信息收集核心)                           │
└──────────────────────────────────────────────────────────────┘
        │
        ├─────────────────────────────────────┐
        │                                     │
        ▼                                     ▼
┌──────────────────────────┐    ┌────────────────────────────┐
│ VulkanDeviceInfoExtensions│    │     VulkanResources        │
│   (扩展特性/属性读取)       │    │    (资源转换工具)           │
└──────────────────────────┘    └────────────────────────────┘
```

### 架构层次

| 层次 | 组件 | 职责 |
|------|------|------|
| **UI层** | `vulkanCapsViewer`, `settingsDialog`, `submitDialog` | 用户界面交互 |
| **业务逻辑层** | `vulkanDeviceInfo`, `vulkanDatabase` | 核心业务处理 |
| **数据访问层** | `vulkanContext`, `vulkanAndroid` | Vulkan API 访问 |
| **工具层** | `vulkanResources`, `CustomFilterProxyModel` | 辅助功能 |

---

## 主要模块职责

### 1. 主程序模块 (main.cpp)

**职责**：应用程序入口点，初始化 Qt 应用程序和命令行解析

**关键功能**：
- 命令行参数解析
- 日志系统初始化
- 数据库连接配置加载
- 非可视化报告生成模式支持

**命令行选项**：
| 选项 | 说明 |
|------|------|
| `-s` | 保存报告到文件（无GUI） |
| `--upload` | 上传报告到数据库 |
| `--deviceindex` | 设置设备索引 |
| `--submitter` | 设置提交者名称 |
| `--comment` | 设置报告注释 |
| `-d` | 从 .ini 文件加载数据库连接信息 |
| `--log` | 启用日志文件记录 |
| `--noproxy` | 禁用代理 |

---

### 2. 主窗口模块 (vulkanCapsViewer)

**职责**：主应用程序窗口，协调所有子模块

**核心成员**：
```cpp
class VulkanCapsViewer : public QMainWindow {
    std::vector<VulkanDeviceInfo> vulkanGPUs;        // GPU设备列表
    std::vector<VulkanLayerInfo> instanceLayers;     // 实例层列表
    std::vector<VkExtensionProperties> instanceExtensions; // 实例扩展
    VulkanDatabase database;                         // 数据库操作对象
    // ...
};
```

**主要功能**：
- Vulkan 实例初始化
- GPU 设备枚举和信息收集
- 设备信息显示（属性、特性、格式、扩展等）
- 报告保存和上传
- 数据库状态检查

---

### 3. 设备信息模块 (vulkanDeviceInfo)

**职责**：收集和存储 Vulkan 物理设备的所有能力信息

**核心数据结构**：

```cpp
class VulkanDeviceInfo : public VulkanDeviceInfoExtensions {
    uint32_t id;                              // 设备ID
    VkPhysicalDeviceProperties props;         // 设备属性
    VkPhysicalDeviceMemoryProperties memoryProperties; // 内存属性
    VkPhysicalDeviceFeatures deviceFeatures;  // 设备特性
    
    QVariantMap properties;                   // Core 1.0 属性
    QVariantMap core11Properties;             // Core 1.1 属性
    QVariantMap core12Properties;             // Core 1.2 属性
    QVariantMap core13Properties;             // Core 1.3 属性
    QVariantMap core14Properties;             // Core 1.4 属性
    
    QVariantMap features;                     // Core 1.0 特性
    QVariantMap core11Features;               // Core 1.1 特性
    QVariantMap core12Features;               // Core 1.2 特性
    QVariantMap core13Features;               // Core 1.3 特性
    QVariantMap core14Features;               // Core 1.4 特性
    
    std::vector<VulkanQueueFamilyInfo> queueFamilies; // 队列族
    std::vector<VulkanFormatInfo> formats;    // 格式支持
    VulkanSurfaceInfo surfaceInfo;            // 表面信息
    std::vector<VulkanProfileInfo> profiles;  // Profiles
    OSInfo os;                                // 操作系统信息
};
```

**主要方法**：
| 方法 | 说明 |
|------|------|
| `readExtensions()` | 读取设备扩展 |
| `readLayers()` | 读取设备层 |
| `readSupportedFormats()` | 读取支持的格式 |
| `readQueueFamilies()` | 读取队列族信息 |
| `readPhysicalProperties()` | 读取物理设备属性 |
| `readPhysicalFeatures()` | 读取物理设备特性 |
| `readSurfaceInfo()` | 读取表面信息 |
| `toJson()` | 转换为 JSON 格式 |

---

### 4. 扩展信息模块 (vulkanDeviceInfoExtensions)

**职责**：处理 Vulkan 扩展特性和属性的读取

**支持厂商扩展**：
- **KHR** (Khronos): 86 个扩展
- **EXT**: 108 个扩展
- **AMD**: 5 个扩展
- **NV**: 44 个扩展
- **ARM**: 12 个扩展
- **QCOM**: 14 个扩展
- **VALVE**: 5 个扩展
- **HUAWEI**: 4 个扩展
- 其他厂商扩展...

**数据结构**：
```cpp
struct Feature2 {
    std::string name;        // 特性名称
    VkBool32 supported;      // 是否支持
    const char* extension;   // 所属扩展
};

struct Property2 {
    std::string name;        // 属性名称
    QVariant value;         // 属性值
    const char* extension;   // 所属扩展
};
```

---

### 5. 数据库模块 (vulkanDatabase)

**职责**：与在线数据库进行通信

**主要功能**：
- 检查报告是否已存在于数据库
- 上传新报告到数据库
- 更新现有报告
- 检查服务器连接状态

**配置参数**：
```cpp
static QString username;      // 数据库用户名
static QString password;      // 数据库密码
static QString databaseUrl;   // 数据库URL
```

---

### 6. Vulkan 上下文模块 (vulkanContext)

**职责**：管理 Vulkan 实例和表面上下文

```cpp
struct VulkanContext {
    VkInstance instance;                              // Vulkan实例
    VkSurfaceKHR surface;                             // 表面
    std::string surfaceExtension;                     // 表面扩展名
    PFN_vkGetPhysicalDeviceFeatures2KHR ...;          // 函数指针
    PFN_vkGetPhysicalDeviceProperties2KHR ...;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR ...;
};
```

---

### 7. 设置模块 (settings)

**职责**：管理应用程序设置

```cpp
class Settings {
    QString submitterName;        // 提交者名称
    QString proxyDns;             // 代理DNS
    QString proxyPort;            // 代理端口
    QString proxyUserName;        // 代理用户名
    QString proxyUserPassword;    // 代理密码
    bool proxyEnabled;            // 是否启用代理
};
```

---

### 8. Android 平台模块 (vulkanAndroid)

**职责**：在 Android 平台上动态加载 Vulkan 函数

**关键函数指针**：
- `vkCreateInstance`, `vkDestroyInstance`
- `vkEnumeratePhysicalDevices`
- `vkGetPhysicalDeviceProperties`
- `vkCreateAndroidSurfaceKHR`
- 等等...

---

### 9. 资源工具模块 (vulkanResources)

**职责**：提供 Vulkan 枚举类型到字符串的转换工具

**主要函数**：
| 函数 | 说明 |
|------|------|
| `formatString()` | VkFormat 转字符串 |
| `physicalDeviceTypeString()` | 设备类型转字符串 |
| `presentModeKHRString()` | 呈现模式转字符串 |
| `colorSpaceKHRString()` | 颜色空间转字符串 |
| `driverIdKHRString()` | 驱动ID转字符串 |
| `toHexString()` | 数值转十六进制字符串 |

---

### 10. 过滤代理模型 (CustomFilterProxyModel, treeproxyfilter)

**职责**：为 Qt 视图提供过滤功能

- `CustomFilterProxyModel`: 保持子项的过滤代理
- `TreeProxyFilter`: 树形结构的过滤代理

---

## 关键类与函数说明

### VulkanCapsViewer 类

#### initVulkan()
```cpp
bool initVulkan();
```
初始化 Vulkan 实例，设置必要的扩展和层。

#### getGPUs()
```cpp
void getGPUs();
```
枚举所有物理设备并收集信息。

#### displayDevice()
```cpp
void displayDevice(int index);
```
显示指定索引设备的信息。

#### saveReport()
```cpp
bool saveReport(QString fileName, QString submitter, QString comment);
```
保存报告到 JSON 文件。

#### uploadReportNonVisual()
```cpp
int uploadReportNonVisual(int deviceIndex, QString submitter, QString comment);
```
非可视化模式下上传报告。

---

### VulkanDeviceInfo 类

#### toJson()
```cpp
QJsonObject toJson(QString submitter, QString comment);
```
将设备信息转换为 JSON 对象，用于报告保存和上传。

#### readPhysicalProperties()
```cpp
void readPhysicalProperties();
```
读取物理设备的所有属性，包括 Core 1.0 到 1.4 的属性。

#### readPhysicalFeatures()
```cpp
void readPhysicalFeatures();
```
读取物理设备的所有特性，包括 Core 1.0 到 1.4 的特性。

---

### VulkanDatabase 类

#### checkReportPresent()
```cpp
bool checkReportPresent(VulkanDeviceInfo device, int &reportId);
```
检查报告是否已存在于数据库。

#### uploadReport()
```cpp
bool uploadReport(QJsonObject json, QString& message);
```
上传报告到数据库。

---

## 依赖关系

### 外部依赖

| 依赖 | 版本 | 用途 |
|------|------|------|
| **Vulkan SDK** | 1.3+ | Vulkan API |
| **Qt** | 6.8.3 | GUI框架 |
| **OpenSSL** | - | 网络安全 (Android) |

### Qt 模块依赖

- **Qt6::Core** - 核心功能
- **Qt6::Gui** - GUI组件
- **Qt6::Widgets** - 窗口部件
- **Qt6::Network** - 网络通信

### 平台特定依赖

| 平台 | 依赖 |
|------|------|
| Linux X11 | Qt6::x11extras, XCB |
| Linux Wayland | Qt6::WaylandClient |
| macOS | Cocoa, QuartzCore |
| Windows | Win32 API |
| Android | Android NDK, Native Activity |

### 模块依赖图

```
main.cpp
    └── vulkanCapsViewer
            ├── vulkanContext
            ├── vulkanDeviceInfo
            │       ├── vulkanDeviceInfoExtensions
            │       ├── vulkanResources
            │       └── vulkanContext
            ├── vulkanDatabase
            │       └── vulkanDeviceInfo
            ├── settings
            ├── settingsDialog
            │       └── settings
            └── submitDialog
```

---

## 项目运行方式

### 构建方式

#### 1. 使用 qmake (推荐)

**Linux (X11)**:
```bash
qmake -config release DEFINES+=X11
make
```

**Linux (Wayland)**:
```bash
qmake -config release DEFINES+=WAYLAND
make
```

**Windows**:
```bash
qmake vulkanCapsViewer.pro
nmake release
```

**macOS**:
```bash
qmake -config release
make
```

#### 2. 使用 CMake

```bash
cmake .. -DVULKAN_LOADER_INSTALL_DIR=/path/to/vulkan/sdk
cmake --build . --config Release
```

#### 3. 使用 Qt Creator

打开 `vulkanCapsViewer.pro` 项目文件，配置编译套件后构建。

---

### 运行方式

#### 图形界面模式
```bash
./vulkanCapsViewer
```

#### 保存报告模式
```bash
./vulkanCapsViewer -s report.json
```

#### 上传报告模式
```bash
./vulkanCapsViewer --upload --deviceindex 0 --submitter "Name" --comment "Comment"
```

#### 启用日志
```bash
./vulkanCapsViewer --log
```

---

### Android 构建

1. 安装 Android SDK 和 NDK
2. 使用 Qt Creator 打开项目
3. 选择 Android 编译套件
4. 构建并部署到设备

**注意**: Android 构建需要 Qt 6.9.2 或更新版本以避免部件渲染问题。

---

## 数据流程

### 设备信息收集流程

```
┌─────────────┐
│ 启动应用    │
└──────┬──────┘
       │
       ▼
┌─────────────────┐
│ 创建Vulkan实例  │
└──────┬──────────┘
       │
       ▼
┌─────────────────────┐
│ 枚举物理设备        │
│ vkEnumeratePhysical │
│ Devices             │
└──────┬──────────────┘
       │
       ▼
┌─────────────────────┐
│ 对每个设备:         │
│ - 读取属性          │
│ - 读取特性          │
│ - 读取扩展          │
│ - 读取格式          │
│ - 读取队列族        │
│ - 读取表面信息      │
└──────┬──────────────┘
       │
       ▼
┌─────────────────────┐
│ 显示设备信息        │
└─────────────────────┘
```

### 报告上传流程

```
┌─────────────┐
│ 用户请求    │
│ 上传报告    │
└──────┬──────┘
       │
       ▼
┌─────────────────┐
│ 收集设备信息    │
│ toJson()        │
└──────┬──────────┘
       │
       ▼
┌─────────────────────┐
│ 检查数据库连接      │
└──────┬──────────────┘
       │
       ▼
┌─────────────────────┐
│ 检查报告是否存在    │
└──────┬──────────────┘
       │
       ├─── 存在 ──▶ 检查是否可更新
       │                    │
       │                    ▼
       │              更新报告
       │
       └─── 不存在 ──▶ 上传新报告
```

---

## 扩展支持列表

项目支持 **289 个 Vulkan 扩展**，涵盖以下厂商：

| 厂商 | 扩展数量 |
|------|----------|
| KHR | 86 |
| EXT | 108 |
| NV | 44 |
| ARM | 12 |
| QCOM | 14 |
| AMD | 5 |
| VALVE | 5 |
| HUAWEI | 4 |
| 其他 | 11 |

完整扩展列表请参见 [extensionlist.md](../extensionlist.md)

---

## 许可证

本项目采用 **GNU Lesser General Public License Version 3** 许可证。

```
Copyright (C) 2016-2025 by Sascha Willems (www.saschawillems.de)

This code is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License version 3 as published by the Free Software Foundation.
```

---

## 测试指南

详细的 Android 和 Linux ARM 平台测试说明请参见：

- **测试指南文档**: [TEST_GUIDE.md](./TEST_GUIDE.md)
- **测试指南HTML**: [TEST_GUIDE.html](./TEST_GUIDE.html)

### 快速开始

#### Android 构建
```bash
cd dist
chmod +x build_android.sh
./build_android.sh
```

#### Linux ARM 构建
```bash
cd dist
chmod +x build_linux_arm.sh
./build_linux_arm.sh
```

### 支持的架构

| 平台 | 架构 | 输出文件 |
|------|------|----------|
| Android | arm64-v8a | VulkanCapsViewer-arm64.apk |
| Android | armeabi-v7a | VulkanCapsViewer-arm32.apk |
| Linux | aarch64 | vulkanCapsViewer-arm64 |
| Linux | armhf | vulkanCapsViewer-arm32 |

### 依赖库位置

#### Android
- ARM32: `/vendor/lib/egl/libGLES_mali.so`
- ARM64: `/vendor/lib64/egl/libGLES_mali.so`

#### Linux
- `/lib/libmali.so` 或 `/lib64/libmali.so`
- `/usr/lib/libmali.so` 或 `/usr/lib64/libmali.so`

---

## 相关链接

- **项目主页**: [GitHub](https://github.com/SaschaWillems/VulkanCapsViewer)
- **在线数据库**: [vulkan.gpuinfo.org](https://vulkan.gpuinfo.org/)
- **下载页面**: [vulkan.gpuinfo.org/download.php](https://vulkan.gpuinfo.org/download.php)
- **Vulkan SDK**: [vulkan.lunarg.com](https://vulkan.lunarg.com/)
- **Qt**: [qt.io](https://www.qt.io/)

---

*文档生成时间: 2026-06-05*
