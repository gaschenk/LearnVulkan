#include "Game.h"

void Game::printVkPhysicalDeviceInfo(const VkPhysicalDevice &device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    std::cout << "Type				Value		" << std::endl;
    std::cout.width(75);
    std::cout.fill('-');
    std::cout << "" << std::endl;
    std::cout << "Device Name:   			" << properties.deviceName << std::endl;
    const auto apiVersion = properties.apiVersion;
    const auto driverVersion = properties.driverVersion;
    std::cout << "API version:   			" << VK_VERSION_MAJOR(apiVersion) << "." << VK_VERSION_MINOR(apiVersion)
              << "." << VK_VERSION_PATCH(apiVersion) << std::endl;
    std::cout << "Driver version:			" << driverVersion << std::endl;
    std::cout << "Vendor ID:    			" << properties.vendorID << std::endl;
    std::cout << "Device ID:    			" << properties.deviceID << std::endl;
    std::cout << "Device type:   			" << properties.deviceType << std::endl;

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

    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, nullptr);

    std::vector<VkQueueFamilyProperties> familyProperties(amountOfQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties.data());

    std::cout << "Amount of queue families: 	" << amountOfQueueFamilies << std::endl;

    for (uint32_t i = 0; i < amountOfQueueFamilies; ++i)
    {
        std::cout << std::endl;
        std::cout << "Queue family #" << i << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT     	" << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                  << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT      	" << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
                  << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT     	" << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
                  << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT	"
                  << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
        std::cout << "Queue count:              	" << familyProperties[i].queueCount << std::endl;
        std::cout << "Timestamp valid bits:        	" << familyProperties[i].timestampValidBits << std::endl;
        const uint32_t width = familyProperties[i].minImageTransferGranularity.width;
        const uint32_t height = familyProperties[i].minImageTransferGranularity.height;
        const uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
        std::cout << "minImageTransferGranularity: 	" << width << "," << height << "," << depth << std::endl;
    }

    std::cout << std::endl;
}

void Game::initializeGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, false);
    window = glfwCreateWindow(400, 500, "Learn Vulkan", nullptr, nullptr);
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
    vkEnumerateInstanceLayerProperties(&amountOfLayers, layers.data());
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
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);

    uint32_t amountOfExtensions = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);

    std::vector<VkExtensionProperties> extensions(amountOfExtensions);
    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions.data());

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
    instanceInfo.enabledExtensionCount = amountOfGlfwExtensions;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;

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

    for (unsigned int i = 0; i < amountOfPhysicalDevices; i++)
    {
        auto physicalDevice = physicalDevices[i];
        printVkPhysicalDeviceInfo(physicalDevice);
    }

    float queuePriorities[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                               1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0;
    deviceQueueCreateInfo.queueCount = 16;
    deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceCreateInfo;
    VkPhysicalDeviceFeatures usedFeatures = {};

    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    result = vkCreateDevice(physicalDevices[1], &deviceCreateInfo, nullptr, &device);
    ASSERT_VULKAN(result);

    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);
}

void Game::run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

void Game::shutdownVulkan() const
{
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Game::shutdownGLFW() const
{
    glfwDestroyWindow(window);
}

void Game::init()
{
    initializeGLFW();
    initializeVulkan();
}

void Game::shutdown() const
{
    shutdownVulkan();
    shutdownGLFW();
}
