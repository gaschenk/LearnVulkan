#include "Game.h"

std::vector<char> readFile(const std::string &fileName)
{
    std::ifstream inputFileStream(fileName, std::ios::binary | std::ios::ate);

    if (inputFileStream)
    {
        size_t fileSize = static_cast<size_t>(inputFileStream.tellg());
        std::vector<char> fileBuffer(fileSize);
        inputFileStream.seekg(0);
        inputFileStream.read(fileBuffer.data(), fileSize);
        inputFileStream.close();
        return fileBuffer;
    }
    throw std::runtime_error("Failed to open file");
}
VkPhysicalDeviceProperties Game::getDeviceProperties(const VkPhysicalDevice &device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    return properties;
}

std::vector<VkSurfaceFormatKHR> Game::getSurfaceFormats(const VkPhysicalDevice &physical_device)
{
    uint32_t amountOfFormats;
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &amountOfFormats, nullptr);
    ASSERT_VULKAN(result);
    std::vector<VkSurfaceFormatKHR> formats(amountOfFormats);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &amountOfFormats, formats.data());
    ASSERT_VULKAN(result);

    std::cout << "Amount of Formats: " << amountOfFormats << std::endl;
    for (auto format : formats)
    {
        std::cout << "\t->" << format.format << std::endl;
        std::cout << "\t|->" << nameof::nameof_enum(format.colorSpace) << std::endl;
    }

    return formats;
}

int Game::getBestDeviceId(std::vector<VkPhysicalDevice> &devices)
{
    std::vector<int> bestDeviceIds = {};
    for (unsigned int i = 0; i < devices.size(); i++)
    {
        auto physicalDevice = devices[i];
        VkPhysicalDeviceProperties properties = getDeviceProperties(physicalDevice);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            bestDeviceIds.push_back(i);

        // TODO: Check for Double/Triple buffering support and other stuff
        // such as min & max image size.
        getSurfaceCapabilities(physicalDevice);
        getSurfaceFormats(physicalDevice);
        getPresentModes(physicalDevice);

#ifdef _DEBUG
        printVkPhysicalDeviceInfo(physicalDevice, properties);
#endif
        getQueueFamilyProperties(physicalDevice);
    }
    if (bestDeviceIds.size() == 0)
        return {0};

    return bestDeviceIds[0];
}

std::vector<VkPresentModeKHR> Game::getPresentModes(const VkPhysicalDevice physicalDevice)
{
    uint32_t presentModesCount;
    auto result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);
    ASSERT_VULKAN(result);
    std::vector<VkPresentModeKHR> presentModes(presentModesCount);
    result =
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, presentModes.data());
    ASSERT_VULKAN(result);

    std::cout << "Amount of presentation modes: " << presentModesCount << std::endl;
    for (auto present_mode : presentModes)
    {
        std::cout << "\t->" << nameof::nameof_enum(present_mode) << std::endl;
    }
    return presentModes;
}

VkSurfaceCapabilitiesKHR Game::getSurfaceCapabilities(const VkPhysicalDevice physicalDevice)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    ASSERT_VULKAN(result);
#ifdef _DEBUG
    printDeviceSurfaceCapabilities(surfaceCapabilities);
#endif
    return surfaceCapabilities;
}

void Game::printDeviceSurfaceCapabilities(VkSurfaceCapabilitiesKHR &surfaceCapabilities)
{
    std::cout << "Surface Capabilities: " << std::endl;
    std::cout << "\tminImageCount: " << surfaceCapabilities.minImageCount << std::endl;
    std::cout << "\tmaxImageCount: " << surfaceCapabilities.maxImageCount << std::endl;
    std::cout << "\tcurrentExtent: " << surfaceCapabilities.currentExtent.width << "/"
              << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "\tminImageExtent: " << surfaceCapabilities.minImageExtent.width << "/"
              << surfaceCapabilities.minImageExtent.height << std::endl;
    std::cout << "\tmaxImageExtent: " << surfaceCapabilities.maxImageExtent.width << "/"
              << surfaceCapabilities.maxImageExtent.height << std::endl;
    std::cout << "\tmaxImageArrayLayers: " << surfaceCapabilities.maxImageArrayLayers << std::endl;
    std::cout << "\tsupportedTransforms: " << surfaceCapabilities.supportedTransforms << std::endl;
    std::cout << "\tcurrentTransform: " << surfaceCapabilities.currentTransform << std::endl;
    std::cout << "\tsupportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
    /*
    VkImageUsageFlagBits flags = static_cast<VkImageUsageFlagBits>(surfaceCapabilities.supportedUsageFlags);
    std::cout << "\tsupportedUsageFlags: " << nameof::nameof_enum_flag(flags) << std::endl;
     */
    std::cout << "\tsupportedUsageFlags: " << surfaceCapabilities.supportedUsageFlags << std::endl;
}

std::vector<VkQueueFamilyProperties> Game::getQueueFamilyProperties(const VkPhysicalDevice &device)
{
    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, nullptr);

    std::vector<VkQueueFamilyProperties> familyProperties(amountOfQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties.data());
#ifdef _DEBUG
    printQueueFamilyInfo(familyProperties);
#endif
    return familyProperties;
}

void Game::printVkPhysicalDeviceInfo(const VkPhysicalDevice &device, VkPhysicalDeviceProperties properties)
{
    std::cout << "Type				Value		" << std::endl;
    std::cout.width(75);
    std::cout.fill('-');
    std::cout << "" << std::endl;

    printPropertyInfo(properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    VkPhysicalDeviceMemoryProperties memProp;
    vkGetPhysicalDeviceMemoryProperties(device, &memProp);

    for (uint32_t i = 0; i < memProp.memoryHeapCount; ++i)
    {
        const VkMemoryHeap heap = memProp.memoryHeaps[i];
        const unsigned long long heapSizeInMB = heap.size / 1024 / 1024;
        std::cout << "Memory[" << i << "]:  			" << heapSizeInMB << "MB" << std::endl;
    }
}

void Game::printQueueFamilyInfo(const std::vector<VkQueueFamilyProperties> properties)
{
    size_t amountOfQueueFamilies = properties.size();
    std::cout << "Amount of queue families: 	" << amountOfQueueFamilies << std::endl;

    for (size_t i = 0; i < amountOfQueueFamilies; ++i)
    {
        std::cout << std::endl;
        std::cout << "Queue family #" << i << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT     	" << ((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                  << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT      	" << ((properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
                  << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT     	" << ((properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
                  << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT	" << ((properties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)
                  << std::endl;
        std::cout << "Queue count:              	" << properties[i].queueCount << std::endl;
        std::cout << "Timestamp valid bits:        	" << properties[i].timestampValidBits << std::endl;
        const uint32_t width = properties[i].minImageTransferGranularity.width;
        const uint32_t height = properties[i].minImageTransferGranularity.height;
        const uint32_t depth = properties[i].minImageTransferGranularity.depth;
        std::cout << "minImageTransferGranularity: 	" << width << "," << height << "," << depth << std::endl;
    }
}

void Game::printPropertyInfo(VkPhysicalDeviceProperties &properties)
{
    std::cout << "Device Name:   			" << properties.deviceName << std::endl;
    const auto apiVersion = properties.apiVersion;
    const auto driverVersion = properties.driverVersion;
    std::cout << "API version:   			" << VK_VERSION_MAJOR(apiVersion) << "." << VK_VERSION_MINOR(apiVersion)
              << "." << VK_VERSION_PATCH(apiVersion) << std::endl;
    std::cout << "Driver version:			" << driverVersion << std::endl;
    std::cout << "Vendor ID:    			" << properties.vendorID << std::endl;
    std::cout << "Device ID:    			" << properties.deviceID << std::endl;
    std::cout << "Device type:   			" << properties.deviceType << std::endl;
}

void Game::initializeVulkan()
{
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Learn Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "Learn Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    uint32_t amountOfLayers = 0;
    VkResult result = vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
    ASSERT_VULKAN(result);

    std::vector<VkLayerProperties> layers(amountOfLayers);
    result = vkEnumerateInstanceLayerProperties(&amountOfLayers, layers.data());
    ASSERT_VULKAN(result);

    std::cout << "Amount of instance layers:	" << amountOfLayers << std::endl;

    for (uint32_t i = 0; i < amountOfLayers; ++i)
    {
        auto layer = layers[i];
        std::cout << "Layer name:	            	" << layer.layerName << std::endl;
        std::cout << "Description:	            	" << layer.description << std::endl;
        std::cout << "Specification version:		" << layer.specVersion << std::endl;
        std::cout << "Implementation version:		" << layer.implementationVersion << std::endl;
        std::cout.width(75);
        std::cout.fill('~');
        std::cout << "" << std::endl;
    }

    std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    uint32_t amountOfGlfwExtensions;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);
    std::vector<const char *> enabledExtensions(glfwExtensions, glfwExtensions + amountOfGlfwExtensions);

    uint32_t amountOfExtensions = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
    ASSERT_VULKAN(result);

    std::vector<VkExtensionProperties> extensions(amountOfExtensions);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions.data());
    ASSERT_VULKAN(result);

    std::cout << std::endl << std::endl << "Extensions" << std::endl;
    for (uint32_t i = 0; i < amountOfExtensions; ++i)
    {
        auto extension = extensions[i];
        std::cout << "Layer name:	            	" << extension.extensionName << std::endl;
        std::cout << "Specification version:		" << extension.specVersion << std::endl;
        std::cout.width(75);
        std::cout.fill('~');
        std::cout << "" << std::endl;
    }

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = validationLayers.size();
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    instanceInfo.enabledExtensionCount = enabledExtensions.size();
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();

    result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    ASSERT_VULKAN(result);

    result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    ASSERT_VULKAN(result);

    uint32_t amountOfPhysicalDevices = 0;
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
    ASSERT_VULKAN(result);

    std::cout << "Amount of devices: " << amountOfPhysicalDevices << std::endl;

    std::vector<VkPhysicalDevice> physicalDevices(amountOfPhysicalDevices);
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());
    ASSERT_VULKAN(result);

    int bestDeviceId = getBestDeviceId(physicalDevices);

    // TODO: Select proper queueCount and queueFamilIndex and dynamically generate queuePriorities
    // float queuePriorities[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    //                          1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0;
    deviceQueueCreateInfo.queueCount = getQueueFamilyProperties(physicalDevices[bestDeviceId])[0].queueCount;
    std::vector<float> queuePriorities(deviceQueueCreateInfo.queueCount, 1.0f);
    deviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo deviceCreateInfo;
    VkPhysicalDeviceFeatures usedFeatures = {};

    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    // TODO: Select proper physical device.
    result = vkCreateDevice(physicalDevices[bestDeviceId], &deviceCreateInfo, nullptr, &device);
    ASSERT_VULKAN(result);
    std::cout << "Best Device Id:   " << bestDeviceId << std::endl;

    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);

    auto surfaceCapabilities = getSurfaceCapabilities(physicalDevices[bestDeviceId]);
    auto surfaceFormats = getSurfaceFormats(physicalDevices[bestDeviceId]);
    auto presentModes = getPresentModes(physicalDevices[bestDeviceId]);

    VkBool32 surfaceSupport = false;
    result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[bestDeviceId], 0, surface, &surfaceSupport);
    ASSERT_VULKAN(result);

    if (!surfaceSupport)
        __debugbreak();

    auto selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (std::count(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR))
    {
        selectedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    }
    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount;
    swapchainCreateInfo.imageFormat = surfaceFormats.data()[0].format;
    swapchainCreateInfo.imageColorSpace = surfaceFormats.data()[0].colorSpace;
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = selectedPresentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
    ASSERT_VULKAN(result);

    uint32_t amountOfImagesInSwapchain = 0;
    result = vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, nullptr);
    ASSERT_VULKAN(result);
    std::vector<VkImage> imagesInSwapchain(amountOfImagesInSwapchain);
    result = vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, imagesInSwapchain.data());
    ASSERT_VULKAN(result);

    imageViews.resize(amountOfImagesInSwapchain);
    // imageViews = std::vector<VkImageView>(amountOfImagesInSwapchain);
    for (size_t i = 0; i < amountOfImagesInSwapchain; i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = imagesInSwapchain[0];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = surfaceFormats.data()[0].format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);
        ASSERT_VULKAN(result);
    }

    auto shaderCodeVert = readFile("content/vert.spv");
    auto shaderCodeFrag = readFile("content/frag.spv");
#ifdef _DEBUG
    std::cout << "File sizes: " << std::endl;
    std::cout << "\tvert.spv " << shaderCodeVert.size() << "bytes" << std::endl;
    std::cout << "\tfrag.spv " << shaderCodeFrag.size() << "bytes" << std::endl;
#endif

    CreateShaderModule(shaderCodeVert, &shaderModuleVert);
    CreateShaderModule(shaderCodeFrag, &shaderModuleFrag);

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert;
    shaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoVert.pNext = nullptr;
    shaderStageCreateInfoVert.flags = 0;
    shaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfoVert.module = shaderModuleVert;
    shaderStageCreateInfoVert.pName = "main";
    shaderStageCreateInfoVert.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoFrag;
    shaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoFrag.pNext = nullptr;
    shaderStageCreateInfoFrag.flags = 0;
    shaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfoFrag.module = shaderModuleFrag;
    shaderStageCreateInfoFrag.pName = "main";
    shaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderCreateInfos{shaderStageCreateInfoFrag,
                                                                           shaderStageCreateInfoVert};

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo;
    pipelineVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputCreateInfo.pNext = nullptr;
    pipelineVertexInputCreateInfo.flags = 0;
    pipelineVertexInputCreateInfo.vertexBindingDescriptionCount = 0;
    pipelineVertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
    pipelineVertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
    pipelineInputAssemblyStateCreateInfo.flags = 0;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = false;

    VkViewport viewPort;
    viewPort.x = 0;
    viewPort.y = 0;
    viewPort.width = surfaceCapabilities.currentExtent.width;
    viewPort.height = surfaceCapabilities.currentExtent.height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;

    VkRect2D scissorRect;
    scissorRect.offset = {0, 0};
    scissorRect.extent = {surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height};

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
    pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfo.pNext = nullptr;
    pipelineViewportStateCreateInfo.flags = 0;
    pipelineViewportStateCreateInfo.viewportCount = 1;
    pipelineViewportStateCreateInfo.pViewports = &viewPort;
    pipelineViewportStateCreateInfo.scissorCount = 1;
    pipelineViewportStateCreateInfo.pScissors = &scissorRect;

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
    pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizationStateCreateInfo.pNext = nullptr;
    pipelineRasterizationStateCreateInfo.flags = 0;
    pipelineRasterizationStateCreateInfo.depthClampEnable = false;
    pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = false;
    pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    pipelineRasterizationStateCreateInfo.depthBiasEnable = false;
    pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
}

void Game::CreateShaderModule(std::vector<char> &code, VkShaderModule *shaderModule)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;
    shaderModuleCreateInfo.flags = 0;
    shaderModuleCreateInfo.codeSize = code.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t *>(code.data());
    auto result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, shaderModule);
    ASSERT_VULKAN(result);
}

void Game::shutdownVulkan() const
{
    vkDeviceWaitIdle(device);
    vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
    vkDestroyShaderModule(device, shaderModuleVert, nullptr);
    for (auto image_view : imageViews)
    {
        vkDestroyImageView(device, image_view, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Game::init()
{
    initializeGLFW();
    initializeVulkan();
}

void Game::initializeGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, false);
    window = glfwCreateWindow(500, 440, "Learn Vulkan", nullptr, nullptr);
}

void Game::shutdownGLFW() const
{
    glfwDestroyWindow(window);
}

void Game::shutdown() const
{
    shutdownVulkan();
    shutdownGLFW();
}

void Game::run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}
