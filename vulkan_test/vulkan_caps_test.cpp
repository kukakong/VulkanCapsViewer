#include <vulkan/vulkan.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cinttypes>
#include <memory>

// ============================================================================
// Helper utilities
// ============================================================================

static const char* deviceTypeString(VkPhysicalDeviceType type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:          return "OTHER";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "INTEGRATED_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "DISCRETE_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "VIRTUAL_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "CPU";
        default:                                      return "UNKNOWN";
    }
}

static const char* formatString(VkFormat fmt) {
    switch (fmt) {
        case VK_FORMAT_R8G8B8A8_UNORM:            return "R8G8B8A8_UNORM";
        case VK_FORMAT_B8G8R8A8_UNORM:            return "B8G8R8A8_UNORM";
        case VK_FORMAT_R8G8B8A8_SRGB:             return "R8G8B8A8_SRGB";
        case VK_FORMAT_B8G8R8A8_SRGB:             return "B8G8R8A8_SRGB";
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:     return "A8B8G8R8_UNORM_PACK32";
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:  return "A2B10G10R10_UNORM_PACK32";
        case VK_FORMAT_R16G16B16A16_SFLOAT:        return "R16G16B16A16_SFLOAT";
        case VK_FORMAT_R32G32B32A32_SFLOAT:        return "R32G32B32A32_SFLOAT";
        case VK_FORMAT_R16G16_SFLOAT:              return "R16G16_SFLOAT";
        case VK_FORMAT_R32G32_SFLOAT:              return "R32G32_SFLOAT";
        case VK_FORMAT_D16_UNORM:                  return "D16_UNORM";
        case VK_FORMAT_D24_UNORM_S8_UINT:          return "D24_UNORM_S8_UINT";
        case VK_FORMAT_D32_SFLOAT:                 return "D32_SFLOAT";
        case VK_FORMAT_D32_SFLOAT_S8_UINT:         return "D32_SFLOAT_S8_UINT";
        case VK_FORMAT_S8_UINT:                    return "S8_UINT";
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:        return "BC1_RGB_UNORM_BLOCK";
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:         return "BC1_RGB_SRGB_BLOCK";
        case VK_FORMAT_BC2_UNORM_BLOCK:            return "BC2_UNORM_BLOCK";
        case VK_FORMAT_BC3_UNORM_BLOCK:            return "BC3_UNORM_BLOCK";
        case VK_FORMAT_BC4_UNORM_BLOCK:            return "BC4_UNORM_BLOCK";
        case VK_FORMAT_BC5_UNORM_BLOCK:            return "BC5_UNORM_BLOCK";
        case VK_FORMAT_BC7_UNORM_BLOCK:            return "BC7_UNORM_BLOCK";
        case VK_FORMAT_BC7_SRGB_BLOCK:             return "BC7_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:    return "ETC2_R8G8B8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:     return "ETC2_R8G8B8_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:  return "ETC2_R8G8B8A1_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:   return "ETC2_R8G8B8A1_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:  return "ETC2_R8G8B8A8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:   return "ETC2_R8G8B8A8_SRGB_BLOCK";
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:        return "EAC_R11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:        return "EAC_R11_SNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:     return "EAC_R11G11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:     return "EAC_R11G11_SNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:       return "ASTC_4x4_UNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:        return "ASTC_4x4_SRGB_BLOCK";
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:       return "ASTC_5x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:        return "ASTC_5x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:       return "ASTC_6x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:        return "ASTC_6x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:       return "ASTC_8x8_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:        return "ASTC_8x8_SRGB_BLOCK";
        case VK_FORMAT_R5G6B5_UNORM_PACK16:        return "R5G6B5_UNORM_PACK16";
        case VK_FORMAT_R8G8B8_UNORM:               return "R8G8B8_UNORM";
        case VK_FORMAT_R8G8B8A8_SNORM:             return "R8G8B8A8_SNORM";
        case VK_FORMAT_R8_UNORM:                   return "R8_UNORM";
        case VK_FORMAT_R8G8_UNORM:                 return "R8G8_UNORM";
        default:                                    return "UNKNOWN";
    }
}

static std::string versionString(uint32_t version) {
    uint32_t major = VK_VERSION_MAJOR(version);
    uint32_t minor = VK_VERSION_MINOR(version);
    uint32_t patch = VK_VERSION_PATCH(version);
    char buf[64];
    snprintf(buf, sizeof(buf), "%u.%u.%u", major, minor, patch);
    return std::string(buf);
}

static const char* boolStr(VkBool32 v) {
    return v ? "true" : "false";
}

static std::string escapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 4);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

// ============================================================================
// Output abstraction: plain text or JSON
// ============================================================================

class OutputWriter {
public:
    virtual ~OutputWriter() = default;
    virtual void beginDocument() = 0;
    virtual void endDocument() = 0;
    virtual void beginDevice(int index, const VkPhysicalDeviceProperties& props) = 0;
    virtual void endDevice() = 0;
    virtual void writeExtensions(uint32_t count, const VkExtensionProperties* exts) = 0;
    virtual void writeLayers(uint32_t count, const VkLayerProperties* layers) = 0;
    virtual void writeMemoryProperties(const VkPhysicalDeviceMemoryProperties& mem) = 0;
    virtual void writeQueueFamilies(uint32_t count, const VkQueueFamilyProperties* families) = 0;
    virtual void writeFeatures(const VkPhysicalDeviceFeatures& features) = 0;
    virtual void writeFormatProperties(VkFormat format, const VkFormatProperties& props) = 0;
    virtual void writeSurfaceCapabilities(const VkSurfaceCapabilitiesKHR& caps,
                                          uint32_t formatCount, const VkSurfaceFormatKHR* formats,
                                          uint32_t presentModeCount, const VkPresentModeKHR* modes) = 0;
};

// ============================================================================
// Plain text output
// ============================================================================

class TextOutput : public OutputWriter {
    std::ostream& os_;
    int deviceIndex_ = 0;

public:
    explicit TextOutput(std::ostream& os) : os_(os) {}

    void beginDocument() override {
        os_ << "============================================================\n";
        os_ << "       Vulkan Capability Report\n";
        os_ << "============================================================\n\n";
    }

    void endDocument() override {
        os_ << "\n============================================================\n";
        os_ << "  Report complete.\n";
        os_ << "============================================================\n";
    }

    void beginDevice(int index, const VkPhysicalDeviceProperties& props) override {
        deviceIndex_ = index;
        os_ << "------------------------------------------------------------\n";
        os_ << "Device " << index << ": " << props.deviceName << "\n";
        os_ << "------------------------------------------------------------\n";
        os_ << "  Device Name:    " << props.deviceName << "\n";
        os_ << "  Device Type:     " << deviceTypeString(props.deviceType)
            << " (" << props.deviceType << ")\n";
        os_ << "  API Version:    " << versionString(props.apiVersion) << "\n";
        os_ << "  Driver Version: " << versionString(props.driverVersion)
            << " (0x" << std::hex << props.driverVersion << std::dec << ")\n";
        os_ << "  Vendor ID:      0x" << std::hex << props.vendorID << std::dec
            << " (" << props.vendorID << ")\n";
        os_ << "  Device ID:      0x" << std::hex << props.deviceID << std::dec
            << " (" << props.deviceID << ")\n";
        os_ << "\n";
    }

    void endDevice() override {
        os_ << "\n";
    }

    void writeExtensions(uint32_t count, const VkExtensionProperties* exts) override {
        os_ << "  Supported Extensions (" << count << "):\n";
        for (uint32_t i = 0; i < count; ++i) {
            os_ << "    [" << i << "] " << exts[i].extensionName
                << " (spec " << versionString(exts[i].specVersion) << ")\n";
        }
        os_ << "\n";
    }

    void writeLayers(uint32_t count, const VkLayerProperties* layers) override {
        os_ << "  Supported Layers (" << count << "):\n";
        for (uint32_t i = 0; i < count; ++i) {
            os_ << "    [" << i << "] " << layers[i].layerName
                << " (spec " << versionString(layers[i].specVersion)
                << ", impl " << layers[i].implementationVersion << ")\n";
            if (layers[i].description[0] != '\0') {
                os_ << "        " << layers[i].description << "\n";
            }
        }
        os_ << "\n";
    }

    void writeMemoryProperties(const VkPhysicalDeviceMemoryProperties& mem) override {
        os_ << "  Memory Heaps (" << mem.memoryHeapCount << "):\n";
        for (uint32_t i = 0; i < mem.memoryHeapCount; ++i) {
            os_ << "    Heap " << i << ": "
                << mem.memoryHeaps[i].size / (1024 * 1024) << " MB";
            if (mem.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                os_ << " [DEVICE_LOCAL]";
            os_ << "\n";
        }
        os_ << "  Memory Types (" << mem.memoryTypeCount << "):\n";
        for (uint32_t i = 0; i < mem.memoryTypeCount; ++i) {
            os_ << "    Type " << i << ": heapIndex=" << mem.memoryTypes[i].heapIndex
                << " flags=0x" << std::hex << mem.memoryTypes[i].propertyFlags << std::dec;
            if (mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                os_ << " DEVICE_LOCAL";
            if (mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
                os_ << " HOST_VISIBLE";
            if (mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                os_ << " HOST_COHERENT";
            if (mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
                os_ << " HOST_CACHED";
            if (mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
                os_ << " LAZILY_ALLOCATED";
            os_ << "\n";
        }
        os_ << "\n";
    }

    void writeQueueFamilies(uint32_t count, const VkQueueFamilyProperties* families) override {
        os_ << "  Queue Families (" << count << "):\n";
        for (uint32_t i = 0; i < count; ++i) {
            os_ << "    Queue Family " << i << ":\n";
            os_ << "      Queue Count:  " << families[i].queueCount << "\n";
            os_ << "      Flags:       0x" << std::hex << families[i].queueFlags << std::dec;
            if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)       os_ << " GRAPHICS";
            if (families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)        os_ << " COMPUTE";
            if (families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)        os_ << " TRANSFER";
            if (families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)  os_ << " SPARSE_BINDING";
            if (families[i].queueFlags & VK_QUEUE_PROTECTED_BIT)       os_ << " PROTECTED";
            os_ << "\n";
            os_ << "      Min Image Transfer Granularity: "
                << families[i].minImageTransferGranularity.width << "x"
                << families[i].minImageTransferGranularity.height << "x"
                << families[i].minImageTransferGranularity.depth << "\n";
            os_ << "      Timestamp Valid Bits: " << families[i].timestampValidBits << "\n";
        }
        os_ << "\n";
    }

    void writeFeatures(const VkPhysicalDeviceFeatures& f) override {
        os_ << "  Core 1.0 Features:\n";
        // All VkBool32 fields from VkPhysicalDeviceFeatures
        os_ << "    robustBufferAccess                      : " << boolStr(f.robustBufferAccess) << "\n";
        os_ << "    fullDrawIndexUint32                      : " << boolStr(f.fullDrawIndexUint32) << "\n";
        os_ << "    imageCubeArray                           : " << boolStr(f.imageCubeArray) << "\n";
        os_ << "    independentBlend                          : " << boolStr(f.independentBlend) << "\n";
        os_ << "    geometryShader                            : " << boolStr(f.geometryShader) << "\n";
        os_ << "    tessellationShader                       : " << boolStr(f.tessellationShader) << "\n";
        os_ << "    sampleRateShading                         : " << boolStr(f.sampleRateShading) << "\n";
        os_ << "    dualSrcBlend                              : " << boolStr(f.dualSrcBlend) << "\n";
        os_ << "    logicOp                                  : " << boolStr(f.logicOp) << "\n";
        os_ << "    multiDrawIndirect                        : " << boolStr(f.multiDrawIndirect) << "\n";
        os_ << "    drawIndirectFirstInstance                : " << boolStr(f.drawIndirectFirstInstance) << "\n";
        os_ << "    depthClamp                               : " << boolStr(f.depthClamp) << "\n";
        os_ << "    depthBiasClamp                           : " << boolStr(f.depthBiasClamp) << "\n";
        os_ << "    fillModeNonSolid                         : " << boolStr(f.fillModeNonSolid) << "\n";
        os_ << "    depthBounds                              : " << boolStr(f.depthBounds) << "\n";
        os_ << "    wideLines                                : " << boolStr(f.wideLines) << "\n";
        os_ << "    largePoints                              : " << boolStr(f.largePoints) << "\n";
        os_ << "    alphaToOne                               : " << boolStr(f.alphaToOne) << "\n";
        os_ << "    multiViewport                            : " << boolStr(f.multiViewport) << "\n";
        os_ << "    samplerAnisotropy                        : " << boolStr(f.samplerAnisotropy) << "\n";
        os_ << "    textureCompressionETC2                   : " << boolStr(f.textureCompressionETC2) << "\n";
        os_ << "    textureCompressionASTC_LDR              : " << boolStr(f.textureCompressionASTC_LDR) << "\n";
        os_ << "    textureCompressionBC                     : " << boolStr(f.textureCompressionBC) << "\n";
        os_ << "    occlusionQueryPrecise                    : " << boolStr(f.occlusionQueryPrecise) << "\n";
        os_ << "    pipelineStatisticsQuery                  : " << boolStr(f.pipelineStatisticsQuery) << "\n";
        os_ << "    vertexPipelineStoresAndAtomics           : " << boolStr(f.vertexPipelineStoresAndAtomics) << "\n";
        os_ << "    fragmentStoresAndAtomics                 : " << boolStr(f.fragmentStoresAndAtomics) << "\n";
        os_ << "    shaderTessellationAndGeometryPointSize   : " << boolStr(f.shaderTessellationAndGeometryPointSize) << "\n";
        os_ << "    shaderImageGatherExtended                : " << boolStr(f.shaderImageGatherExtended) << "\n";
        os_ << "    shaderStorageImageExtendedFormats        : " << boolStr(f.shaderStorageImageExtendedFormats) << "\n";
        os_ << "    shaderStorageImageMultisample            : " << boolStr(f.shaderStorageImageMultisample) << "\n";
        os_ << "    shaderStorageImageReadWithoutFormat      : " << boolStr(f.shaderStorageImageReadWithoutFormat) << "\n";
        os_ << "    shaderStorageImageWriteWithoutFormat     : " << boolStr(f.shaderStorageImageWriteWithoutFormat) << "\n";
        os_ << "    shaderUniformBufferArrayDynamicIndexing  : " << boolStr(f.shaderUniformBufferArrayDynamicIndexing) << "\n";
        os_ << "    shaderSampledImageArrayDynamicIndexing   : " << boolStr(f.shaderSampledImageArrayDynamicIndexing) << "\n";
        os_ << "    shaderStorageBufferArrayDynamicIndexing  : " << boolStr(f.shaderStorageBufferArrayDynamicIndexing) << "\n";
        os_ << "    shaderStorageImageArrayDynamicIndexing   : " << boolStr(f.shaderStorageImageArrayDynamicIndexing) << "\n";
        os_ << "    shaderClipDistance                       : " << boolStr(f.shaderClipDistance) << "\n";
        os_ << "    shaderCullDistance                       : " << boolStr(f.shaderCullDistance) << "\n";
        os_ << "    shaderFloat64                            : " << boolStr(f.shaderFloat64) << "\n";
        os_ << "    shaderInt64                              : " << boolStr(f.shaderInt64) << "\n";
        os_ << "    shaderInt16                              : " << boolStr(f.shaderInt16) << "\n";
        os_ << "    shaderResourceResidency                  : " << boolStr(f.shaderResourceResidency) << "\n";
        os_ << "    shaderResourceMinLod                     : " << boolStr(f.shaderResourceMinLod) << "\n";
        os_ << "    sparseBinding                            : " << boolStr(f.sparseBinding) << "\n";
        os_ << "    sparseResidencyBuffer                    : " << boolStr(f.sparseResidencyBuffer) << "\n";
        os_ << "    sparseResidencyImage2D                  : " << boolStr(f.sparseResidencyImage2D) << "\n";
        os_ << "    sparseResidencyImage3D                  : " << boolStr(f.sparseResidencyImage3D) << "\n";
        os_ << "    sparseResidency2Samples                 : " << boolStr(f.sparseResidency2Samples) << "\n";
        os_ << "    sparseResidency4Samples                 : " << boolStr(f.sparseResidency4Samples) << "\n";
        os_ << "    sparseResidency8Samples                 : " << boolStr(f.sparseResidency8Samples) << "\n";
        os_ << "    sparseResidency16Samples                : " << boolStr(f.sparseResidency16Samples) << "\n";
        os_ << "    sparseResidencyAliased                   : " << boolStr(f.sparseResidencyAliased) << "\n";
        os_ << "    variableMultisampleRate                  : " << boolStr(f.variableMultisampleRate) << "\n";
        os_ << "    inheritedQueries                         : " << boolStr(f.inheritedQueries) << "\n";
        os_ << "\n";
    }

    void writeFormatProperties(VkFormat format, const VkFormatProperties& props) override {
        os_ << "    " << formatString(format) << ":\n";
        os_ << "      linearTilingFeatures:   0x" << std::hex << props.linearTilingFeatures << std::dec << "\n";
        os_ << "      optimalTilingFeatures:  0x" << std::hex << props.optimalTilingFeatures << std::dec << "\n";
        os_ << "      bufferFeatures:         0x" << std::hex << props.bufferFeatures << std::dec << "\n";
    }

    void writeSurfaceCapabilities(const VkSurfaceCapabilitiesKHR& caps,
                                   uint32_t formatCount, const VkSurfaceFormatKHR* formats,
                                   uint32_t presentModeCount, const VkPresentModeKHR* modes) override {
        os_ << "  Surface Capabilities:\n";
        os_ << "    minImageCount:           " << caps.minImageCount << "\n";
        os_ << "    maxImageCount:           " << caps.maxImageCount << "\n";
        os_ << "    currentExtent:           " << caps.currentExtent.width << "x" << caps.currentExtent.height << "\n";
        os_ << "    minImageExtent:          " << caps.minImageExtent.width << "x" << caps.minImageExtent.height << "\n";
        os_ << "    maxImageExtent:          " << caps.maxImageExtent.width << "x" << caps.maxImageExtent.height << "\n";
        os_ << "    maxImageArrayLayers:     " << caps.maxImageArrayLayers << "\n";
        os_ << "    supportedTransforms:     0x" << std::hex << caps.supportedTransforms << std::dec << "\n";
        os_ << "    currentTransform:        0x" << std::hex << caps.currentTransform << std::dec << "\n";
        os_ << "    supportedCompositeAlpha: 0x" << std::hex << caps.supportedCompositeAlpha << std::dec << "\n";
        os_ << "    supportedUsageFlags:      0x" << std::hex << caps.supportedUsageFlags << std::dec << "\n";
        os_ << "    Surface Formats (" << formatCount << "):\n";
        for (uint32_t i = 0; i < formatCount; ++i) {
            os_ << "      [" << i << "] format=" << formats[i].format
                << " colorspace=" << formats[i].colorSpace << "\n";
        }
        os_ << "    Present Modes (" << presentModeCount << "):\n";
        for (uint32_t i = 0; i < presentModeCount; ++i) {
            const char* modeStr = "UNKNOWN";
            switch (modes[i]) {
                case VK_PRESENT_MODE_IMMEDIATE_KHR:    modeStr = "IMMEDIATE"; break;
                case VK_PRESENT_MODE_MAILBOX_KHR:       modeStr = "MAILBOX"; break;
                case VK_PRESENT_MODE_FIFO_KHR:          modeStr = "FIFO"; break;
                case VK_PRESENT_MODE_FIFO_RELAXED_KHR:  modeStr = "FIFO_RELAXED"; break;
                case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR: modeStr = "SHARED_DEMAND_REFRESH"; break;
                case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: modeStr = "SHARED_CONTINUOUS_REFRESH"; break;
                default: break;
            }
            os_ << "      [" << i << "] " << modeStr << "\n";
        }
        os_ << "\n";
    }
};

// ============================================================================
// JSON output
// ============================================================================

class JsonOutput : public OutputWriter {
    std::ostream& os_;
    bool firstDevice_ = true;
    bool firstFormat_ = true;

    void writeCommaNewline() { os_ << ",\n"; }

public:
    explicit JsonOutput(std::ostream& os) : os_(os) {}

    void beginDocument() override {
        os_ << "{\n";
        os_ << "  \"vulkan_report\": {\n";
        os_ << "    \"devices\": [";
    }

    void endDocument() override {
        os_ << "\n    ]\n";
        os_ << "  }\n";
        os_ << "}\n";
    }

    void beginDevice(int index, const VkPhysicalDeviceProperties& props) override {
        if (!firstDevice_) os_ << ",";
        firstDevice_ = true;
        firstFormat_ = true;
        os_ << "\n      {\n";
        os_ << "        \"index\": " << index << ",\n";
        os_ << "        \"device_name\": \"" << escapeJson(props.deviceName) << "\",\n";
        os_ << "        \"device_type\": \"" << deviceTypeString(props.deviceType) << "\",\n";
        os_ << "        \"device_type_id\": " << props.deviceType << ",\n";
        os_ << "        \"api_version\": \"" << versionString(props.apiVersion) << "\",\n";
        os_ << "        \"api_version_raw\": " << props.apiVersion << ",\n";
        os_ << "        \"driver_version\": \"" << versionString(props.driverVersion) << "\",\n";
        os_ << "        \"driver_version_raw\": " << props.driverVersion << ",\n";
        os_ << "        \"vendor_id\": " << props.vendorID << ",\n";
        os_ << "        \"device_id\": " << props.deviceID << ",\n";
    }

    void endDevice() override {
        os_ << "\n      }";
        firstDevice_ = false;
    }

    void writeExtensions(uint32_t count, const VkExtensionProperties* exts) override {
        os_ << "        \"extensions\": {\n";
        os_ << "          \"count\": " << count << ",\n";
        os_ << "          \"list\": [\n";
        for (uint32_t i = 0; i < count; ++i) {
            os_ << "            {\"name\": \"" << escapeJson(exts[i].extensionName)
                << "\", \"spec_version\": " << exts[i].specVersion << "}";
            if (i + 1 < count) os_ << ",";
            os_ << "\n";
        }
        os_ << "          ]\n";
        os_ << "        },\n";
    }

    void writeLayers(uint32_t count, const VkLayerProperties* layers) override {
        os_ << "        \"layers\": {\n";
        os_ << "          \"count\": " << count << ",\n";
        os_ << "          \"list\": [\n";
        for (uint32_t i = 0; i < count; ++i) {
            os_ << "            {\"name\": \"" << escapeJson(layers[i].layerName)
                << "\", \"spec_version\": " << layers[i].specVersion
                << ", \"implementation_version\": " << layers[i].implementationVersion
                << ", \"description\": \"" << escapeJson(layers[i].description) << "\"}";
            if (i + 1 < count) os_ << ",";
            os_ << "\n";
        }
        os_ << "          ]\n";
        os_ << "        },\n";
    }

    void writeMemoryProperties(const VkPhysicalDeviceMemoryProperties& mem) override {
        os_ << "        \"memory_properties\": {\n";
        os_ << "          \"heaps\": [\n";
        for (uint32_t i = 0; i < mem.memoryHeapCount; ++i) {
            os_ << "            {\"index\": " << i
                << ", \"size_mb\": " << mem.memoryHeaps[i].size / (1024 * 1024)
                << ", \"size_bytes\": " << mem.memoryHeaps[i].size
                << ", \"device_local\": " << ((mem.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "true" : "false")
                << "}";
            if (i + 1 < mem.memoryHeapCount) os_ << ",";
            os_ << "\n";
        }
        os_ << "          ],\n";
        os_ << "          \"types\": [\n";
        for (uint32_t i = 0; i < mem.memoryTypeCount; ++i) {
            os_ << "            {\"index\": " << i
                << ", \"heap_index\": " << mem.memoryTypes[i].heapIndex
                << ", \"property_flags\": " << mem.memoryTypes[i].propertyFlags
                << ", \"device_local\": " << ((mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ? "true" : "false")
                << ", \"host_visible\": " << ((mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ? "true" : "false")
                << ", \"host_coherent\": " << ((mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ? "true" : "false")
                << ", \"host_cached\": " << ((mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) ? "true" : "false")
                << ", \"lazily_allocated\": " << ((mem.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) ? "true" : "false")
                << "}";
            if (i + 1 < mem.memoryTypeCount) os_ << ",";
            os_ << "\n";
        }
        os_ << "          ]\n";
        os_ << "        },\n";
    }

    void writeQueueFamilies(uint32_t count, const VkQueueFamilyProperties* families) override {
        os_ << "        \"queue_families\": [\n";
        for (uint32_t i = 0; i < count; ++i) {
            os_ << "          {\n";
            os_ << "            \"index\": " << i << ",\n";
            os_ << "            \"queue_count\": " << families[i].queueCount << ",\n";
            os_ << "            \"queue_flags\": " << families[i].queueFlags << ",\n";
            os_ << "            \"graphics\": " << ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "true" : "false") << ",\n";
            os_ << "            \"compute\": " << ((families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) ? "true" : "false") << ",\n";
            os_ << "            \"transfer\": " << ((families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) ? "true" : "false") << ",\n";
            os_ << "            \"sparse_binding\": " << ((families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? "true" : "false") << ",\n";
            os_ << "            \"protected\": " << ((families[i].queueFlags & VK_QUEUE_PROTECTED_BIT) ? "true" : "false") << ",\n";
            os_ << "            \"timestamp_valid_bits\": " << families[i].timestampValidBits << ",\n";
            os_ << "            \"min_image_transfer_granularity\": {"
                << "\"width\":" << families[i].minImageTransferGranularity.width
                << ",\"height\":" << families[i].minImageTransferGranularity.height
                << ",\"depth\":" << families[i].minImageTransferGranularity.depth << "}\n";
            os_ << "          }";
            if (i + 1 < count) os_ << ",";
            os_ << "\n";
        }
        os_ << "        ],\n";
    }

    void writeFeatures(const VkPhysicalDeviceFeatures& f) override {
        os_ << "        \"features\": {\n";
#define WRITE_BOOL(name) os_ << "          \"" #name "\": " << boolStr(f.name)
        WRITE_BOOL(robustBufferAccess); os_ << ",\n";
        WRITE_BOOL(fullDrawIndexUint32); os_ << ",\n";
        WRITE_BOOL(imageCubeArray); os_ << ",\n";
        WRITE_BOOL(independentBlend); os_ << ",\n";
        WRITE_BOOL(geometryShader); os_ << ",\n";
        WRITE_BOOL(tessellationShader); os_ << ",\n";
        WRITE_BOOL(sampleRateShading); os_ << ",\n";
        WRITE_BOOL(dualSrcBlend); os_ << ",\n";
        WRITE_BOOL(logicOp); os_ << ",\n";
        WRITE_BOOL(multiDrawIndirect); os_ << ",\n";
        WRITE_BOOL(drawIndirectFirstInstance); os_ << ",\n";
        WRITE_BOOL(depthClamp); os_ << ",\n";
        WRITE_BOOL(depthBiasClamp); os_ << ",\n";
        WRITE_BOOL(fillModeNonSolid); os_ << ",\n";
        WRITE_BOOL(depthBounds); os_ << ",\n";
        WRITE_BOOL(wideLines); os_ << ",\n";
        WRITE_BOOL(largePoints); os_ << ",\n";
        WRITE_BOOL(alphaToOne); os_ << ",\n";
        WRITE_BOOL(multiViewport); os_ << ",\n";
        WRITE_BOOL(samplerAnisotropy); os_ << ",\n";
        WRITE_BOOL(textureCompressionETC2); os_ << ",\n";
        WRITE_BOOL(textureCompressionASTC_LDR); os_ << ",\n";
        WRITE_BOOL(textureCompressionBC); os_ << ",\n";
        WRITE_BOOL(occlusionQueryPrecise); os_ << ",\n";
        WRITE_BOOL(pipelineStatisticsQuery); os_ << ",\n";
        WRITE_BOOL(vertexPipelineStoresAndAtomics); os_ << ",\n";
        WRITE_BOOL(fragmentStoresAndAtomics); os_ << ",\n";
        WRITE_BOOL(shaderTessellationAndGeometryPointSize); os_ << ",\n";
        WRITE_BOOL(shaderImageGatherExtended); os_ << ",\n";
        WRITE_BOOL(shaderStorageImageExtendedFormats); os_ << ",\n";
        WRITE_BOOL(shaderStorageImageMultisample); os_ << ",\n";
        WRITE_BOOL(shaderStorageImageReadWithoutFormat); os_ << ",\n";
        WRITE_BOOL(shaderStorageImageWriteWithoutFormat); os_ << ",\n";
        WRITE_BOOL(shaderUniformBufferArrayDynamicIndexing); os_ << ",\n";
        WRITE_BOOL(shaderSampledImageArrayDynamicIndexing); os_ << ",\n";
        WRITE_BOOL(shaderStorageBufferArrayDynamicIndexing); os_ << ",\n";
        WRITE_BOOL(shaderStorageImageArrayDynamicIndexing); os_ << ",\n";
        WRITE_BOOL(shaderClipDistance); os_ << ",\n";
        WRITE_BOOL(shaderCullDistance); os_ << ",\n";
        WRITE_BOOL(shaderFloat64); os_ << ",\n";
        WRITE_BOOL(shaderInt64); os_ << ",\n";
        WRITE_BOOL(shaderInt16); os_ << ",\n";
        WRITE_BOOL(shaderResourceResidency); os_ << ",\n";
        WRITE_BOOL(shaderResourceMinLod); os_ << ",\n";
        WRITE_BOOL(sparseBinding); os_ << ",\n";
        WRITE_BOOL(sparseResidencyBuffer); os_ << ",\n";
        WRITE_BOOL(sparseResidencyImage2D); os_ << ",\n";
        WRITE_BOOL(sparseResidencyImage3D); os_ << ",\n";
        WRITE_BOOL(sparseResidency2Samples); os_ << ",\n";
        WRITE_BOOL(sparseResidency4Samples); os_ << ",\n";
        WRITE_BOOL(sparseResidency8Samples); os_ << ",\n";
        WRITE_BOOL(sparseResidency16Samples); os_ << ",\n";
        WRITE_BOOL(sparseResidencyAliased); os_ << ",\n";
        WRITE_BOOL(variableMultisampleRate); os_ << ",\n";
        WRITE_BOOL(inheritedQueries); os_ << "\n";
#undef WRITE_BOOL
        os_ << "        },\n";
    }

    void writeFormatProperties(VkFormat format, const VkFormatProperties& props) override {
        if (firstFormat_) {
            os_ << "        \"format_properties\": [\n";
            firstFormat_ = false;
        } else {
            os_ << ",\n";
        }
        os_ << "          {\"format\": \"" << formatString(format)
            << "\", \"format_id\": " << format
            << ", \"linear_tiling_features\": " << props.linearTilingFeatures
            << ", \"optimal_tiling_features\": " << props.optimalTilingFeatures
            << ", \"buffer_features\": " << props.bufferFeatures
            << "}";
    }

    void writeSurfaceCapabilities(const VkSurfaceCapabilitiesKHR& caps,
                                   uint32_t formatCount, const VkSurfaceFormatKHR* formats,
                                   uint32_t presentModeCount, const VkPresentModeKHR* modes) override {
        // Close format_properties array if it was opened
        if (!firstFormat_) {
            os_ << "\n        ],\n";
            firstFormat_ = true; // reset for potential reuse
        }
        os_ << "        \"surface_capabilities\": {\n";
        os_ << "          \"min_image_count\": " << caps.minImageCount << ",\n";
        os_ << "          \"max_image_count\": " << caps.maxImageCount << ",\n";
        os_ << "          \"current_extent\": {\"width\": " << caps.currentExtent.width
            << ", \"height\": " << caps.currentExtent.height << "},\n";
        os_ << "          \"min_image_extent\": {\"width\": " << caps.minImageExtent.width
            << ", \"height\": " << caps.minImageExtent.height << "},\n";
        os_ << "          \"max_image_extent\": {\"width\": " << caps.maxImageExtent.width
            << ", \"height\": " << caps.maxImageExtent.height << "},\n";
        os_ << "          \"max_image_array_layers\": " << caps.maxImageArrayLayers << ",\n";
        os_ << "          \"supported_transforms\": " << caps.supportedTransforms << ",\n";
        os_ << "          \"current_transform\": " << caps.currentTransform << ",\n";
        os_ << "          \"supported_composite_alpha\": " << caps.supportedCompositeAlpha << ",\n";
        os_ << "          \"supported_usage_flags\": " << caps.supportedUsageFlags << ",\n";
        os_ << "          \"surface_formats\": [\n";
        for (uint32_t i = 0; i < formatCount; ++i) {
            os_ << "            {\"format\": " << formats[i].format
                << ", \"color_space\": " << formats[i].colorSpace << "}";
            if (i + 1 < formatCount) os_ << ",";
            os_ << "\n";
        }
        os_ << "          ],\n";
        os_ << "          \"present_modes\": [\n";
        for (uint32_t i = 0; i < presentModeCount; ++i) {
            os_ << "            " << modes[i];
            if (i + 1 < presentModeCount) os_ << ",";
            os_ << "\n";
        }
        os_ << "          ]\n";
        os_ << "        },\n";
    }
};

// ============================================================================
// Format list to test
// ============================================================================

static const VkFormat g_testFormats[] = {
    // Common renderable formats
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_B8G8R8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_B8G8R8A8_SRGB,
    VK_FORMAT_A8B8G8R8_UNORM_PACK32,
    VK_FORMAT_A2B10G10R10_UNORM_PACK32,
    VK_FORMAT_R16G16B16A16_SFLOAT,
    VK_FORMAT_R32G32B32A32_SFLOAT,
    VK_FORMAT_R16G16_SFLOAT,
    VK_FORMAT_R32G32_SFLOAT,
    // Depth/stencil formats
    VK_FORMAT_D16_UNORM,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_S8_UINT,
    // BC compressed formats (desktop)
    VK_FORMAT_BC1_RGB_UNORM_BLOCK,
    VK_FORMAT_BC1_RGB_SRGB_BLOCK,
    VK_FORMAT_BC2_UNORM_BLOCK,
    VK_FORMAT_BC3_UNORM_BLOCK,
    VK_FORMAT_BC4_UNORM_BLOCK,
    VK_FORMAT_BC5_UNORM_BLOCK,
    VK_FORMAT_BC7_UNORM_BLOCK,
    VK_FORMAT_BC7_SRGB_BLOCK,
    // ETC/EAC compressed formats (mobile/Mali)
    VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
    VK_FORMAT_EAC_R11_UNORM_BLOCK,
    VK_FORMAT_EAC_R11_SNORM_BLOCK,
    VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
    VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
    // ASTC compressed formats (mobile/Mali)
    VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
    VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
    VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
    VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
    VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
    VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
    // Other common formats
    VK_FORMAT_R5G6B5_UNORM_PACK16,
    VK_FORMAT_R8G8B8_UNORM,
    VK_FORMAT_R8G8B8A8_SNORM,
    VK_FORMAT_R8_UNORM,
    VK_FORMAT_R8G8_UNORM,
};
static const uint32_t g_testFormatCount = sizeof(g_testFormats) / sizeof(g_testFormats[0]);

// ============================================================================
// Main program
// ============================================================================

static void printUsage(const char* prog) {
    fprintf(stderr,
        "Usage: %s [OPTIONS]\n"
        "\n"
        "Vulkan Capability Test - enumerate and report Vulkan device capabilities.\n"
        "Useful for testing Vulkan on ARM devices with Mali GPUs.\n"
        "\n"
        "Options:\n"
        "  --json       Output results in JSON format\n"
        "  -o <file>    Save report to file\n"
        "  --help       Show this help message\n"
        "\n"
        "Examples:\n"
        "  %s                    Print report to stdout (plain text)\n"
        "  %s --json             Print report to stdout (JSON)\n"
        "  %s --json -o report.json  Save JSON report to file\n"
        "  %s -o report.txt          Save text report to file\n",
        prog, prog, prog, prog, prog);
}

int main(int argc, char* argv[]) {
    bool jsonOutput = false;
    std::string outputFile;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--json") {
            jsonOutput = true;
        } else if (arg == "-o") {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -o requires a filename argument\n");
                return 1;
            }
            outputFile = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", arg.c_str());
            printUsage(argv[0]);
            return 1;
        }
    }

    // Set up output stream
    std::ofstream fileStream;
    std::ostream* outStream = &std::cout;
    if (!outputFile.empty()) {
        fileStream.open(outputFile, std::ios::out | std::ios::trunc);
        if (!fileStream.is_open()) {
            fprintf(stderr, "Error: Cannot open output file '%s'\n", outputFile.c_str());
            return 1;
        }
        outStream = &fileStream;
    }

    // Create output writer
    std::unique_ptr<OutputWriter> writer;
    if (jsonOutput) {
        writer = std::make_unique<JsonOutput>(*outStream);
    } else {
        writer = std::make_unique<TextOutput>(*outStream);
    }

    // Initialize Vulkan
    VkInstance instance = VK_NULL_HANDLE;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Caps Test";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Check for available instance extensions (for surface)
    uint32_t instExtCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, nullptr);
    std::vector<VkExtensionProperties> instExts(instExtCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, instExts.data());

    bool hasKHRSurface = false;
    bool hasKHRPlatformSurface = false;
    std::string platformSurfaceExtName;

    for (uint32_t i = 0; i < instExtCount; ++i) {
        if (strcmp(instExts[i].extensionName, VK_KHR_SURFACE_EXTENSION_NAME) == 0) {
            hasKHRSurface = true;
        }
#ifdef VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
        if (strcmp(instExts[i].extensionName, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME) == 0) {
            hasKHRPlatformSurface = true;
            platformSurfaceExtName = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
        }
#endif
#ifdef VK_KHR_XLIB_SURFACE_EXTENSION_NAME
        if (strcmp(instExts[i].extensionName, VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0) {
            if (platformSurfaceExtName.empty()) {
                hasKHRPlatformSurface = true;
                platformSurfaceExtName = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
            }
        }
#endif
#ifdef VK_KHR_XCB_SURFACE_EXTENSION_NAME
        if (strcmp(instExts[i].extensionName, VK_KHR_XCB_SURFACE_EXTENSION_NAME) == 0) {
            if (platformSurfaceExtName.empty()) {
                hasKHRPlatformSurface = true;
                platformSurfaceExtName = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
            }
        }
#endif
#ifdef VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
        if (strcmp(instExts[i].extensionName, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME) == 0) {
            if (platformSurfaceExtName.empty()) {
                hasKHRPlatformSurface = true;
                platformSurfaceExtName = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
            }
        }
#endif
    }

    // Build instance extension list for surface support
    std::vector<const char*> enabledInstExts;
    if (hasKHRSurface) {
        enabledInstExts.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    }
    if (hasKHRPlatformSurface && !platformSurfaceExtName.empty()) {
        enabledInstExts.push_back(platformSurfaceExtName.c_str());
    }

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (!enabledInstExts.empty()) {
        createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstExts.size());
        createInfo.ppEnabledExtensionNames = enabledInstExts.data();
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Error: Failed to create Vulkan instance (VkResult = %d)\n", result);
        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            fprintf(stderr, "  No compatible Vulkan driver found. Is a Vulkan driver installed?\n");
        }
        return 1;
    }

    // Enumerate physical devices
    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Error: Failed to enumerate physical devices (VkResult = %d)\n", result);
        vkDestroyInstance(instance, nullptr);
        return 1;
    }

    if (deviceCount == 0) {
        fprintf(stderr, "Error: No Vulkan-capable devices found\n");
        vkDestroyInstance(instance, nullptr);
        return 1;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Error: Failed to enumerate physical devices (VkResult = %d)\n", result);
        vkDestroyInstance(instance, nullptr);
        return 1;
    }

    // Load surface extension function pointers if available
    // These are loaded for future use when a surface is available
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
    PFN_vkDestroySurfaceKHR fpDestroySurfaceKHR = nullptr;

    if (hasKHRSurface) {
        fpGetPhysicalDeviceSurfaceSupportKHR =
            (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        fpGetPhysicalDeviceSurfaceCapabilitiesKHR =
            (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        fpGetPhysicalDeviceSurfaceFormatsKHR =
            (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        fpGetPhysicalDeviceSurfacePresentModesKHR =
            (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        fpDestroySurfaceKHR =
            (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR");
        (void)fpGetPhysicalDeviceSurfaceSupportKHR;
        (void)fpDestroySurfaceKHR;
    }

    // Begin report
    writer->beginDocument();

    for (uint32_t devIdx = 0; devIdx < deviceCount; ++devIdx) {
        VkPhysicalDevice device = devices[devIdx];

        // Device properties
        VkPhysicalDeviceProperties props = {};
        vkGetPhysicalDeviceProperties(device, &props);

        writer->beginDevice(static_cast<int>(devIdx), props);

        // Extensions
        uint32_t extCount = 0;
        result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
        if (result == VK_SUCCESS && extCount > 0) {
            std::vector<VkExtensionProperties> exts(extCount);
            result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, exts.data());
            if (result == VK_SUCCESS) {
                writer->writeExtensions(extCount, exts.data());
            } else {
                fprintf(stderr, "Warning: Failed to enumerate extensions for device %u (VkResult = %d)\n", devIdx, result);
            }
        } else {
            VkExtensionProperties emptyExt = {};
            writer->writeExtensions(0, &emptyExt);
        }

        // Layers
        uint32_t layerCount = 0;
        result = vkEnumerateDeviceLayerProperties(device, &layerCount, nullptr);
        if (result == VK_SUCCESS && layerCount > 0) {
            std::vector<VkLayerProperties> layers(layerCount);
            result = vkEnumerateDeviceLayerProperties(device, &layerCount, layers.data());
            if (result == VK_SUCCESS) {
                writer->writeLayers(layerCount, layers.data());
            } else {
                fprintf(stderr, "Warning: Failed to enumerate layers for device %u (VkResult = %d)\n", devIdx, result);
            }
        } else {
            VkLayerProperties emptyLayer = {};
            writer->writeLayers(0, &emptyLayer);
        }

        // Memory properties
        VkPhysicalDeviceMemoryProperties memProps = {};
        vkGetPhysicalDeviceMemoryProperties(device, &memProps);
        writer->writeMemoryProperties(memProps);

        // Queue family properties
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        if (queueFamilyCount > 0) {
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
            writer->writeQueueFamilies(queueFamilyCount, queueFamilies.data());
        } else {
            VkQueueFamilyProperties emptyQF = {};
            writer->writeQueueFamilies(0, &emptyQF);
        }

        // Features
        VkPhysicalDeviceFeatures features = {};
        vkGetPhysicalDeviceFeatures(device, &features);
        writer->writeFeatures(features);

        // Format support
        for (uint32_t fi = 0; fi < g_testFormatCount; ++fi) {
            VkFormatProperties fmtProps = {};
            vkGetPhysicalDeviceFormatProperties(device, g_testFormats[fi], &fmtProps);
            writer->writeFormatProperties(g_testFormats[fi], fmtProps);
        }

        // Surface capabilities
        if (hasKHRSurface && fpGetPhysicalDeviceSurfaceCapabilitiesKHR &&
            fpGetPhysicalDeviceSurfaceFormatsKHR && fpGetPhysicalDeviceSurfacePresentModesKHR) {

            // Try to create a minimal surface for querying capabilities
            // On Android, we need a window; on Linux we need a display connection.
            // Since we don't have a real window, we'll skip surface creation and
            // just note that surface extensions are available.
            // A real application would create a surface from its window system.

            // For headless testing, we check if the device extension is supported
            // and report that surface queries are available but require a window.
            bool deviceSupportsSurface = false;
            uint32_t devExtCount = 0;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &devExtCount, nullptr);
            std::vector<VkExtensionProperties> devExts(devExtCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &devExtCount, devExts.data());
            for (uint32_t ei = 0; ei < devExtCount; ++ei) {
                if (strcmp(devExts[ei].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                    deviceSupportsSurface = true;
                    break;
                }
            }

            if (!jsonOutput) {
                *outStream << "  Surface: KHR_surface extension available.\n";
                if (deviceSupportsSurface) {
                    *outStream << "  Surface: KHR_swapchain device extension available.\n";
                }
                if (!platformSurfaceExtName.empty()) {
                    *outStream << "  Surface: Platform surface extension '" << platformSurfaceExtName << "' available.\n";
                }
                *outStream << "  Surface: Full surface capabilities require a window/surface.\n";
                *outStream << "           Run with a windowing system for complete results.\n\n";
            }
        }

        writer->endDevice();
    }

    writer->endDocument();

    // Cleanup
    vkDestroyInstance(instance, nullptr);

    if (fileStream.is_open()) {
        fileStream.close();
        fprintf(stdout, "Report saved to '%s'\n", outputFile.c_str());
    }

    return 0;
}
