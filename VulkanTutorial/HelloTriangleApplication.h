#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <algorithm> // Necessary for std::min/std::max
#include <cstdint>   // Necessary for UINT32_MAX
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nameof.hpp>
#include <optional>
#include <set>
#include <stb.h>
#include <stdexcept>
#include <vector>

#ifdef _DEBUG
#define ASSERT_VULKAN(val)                                                                                             \
    if (val != VK_SUCCESS)                                                                                             \
    {                                                                                                                  \
        std::cout << "An Error occured: " << nameof::nameof_enum(val) << std::endl;                                    \
        __debugbreak();                                                                                                \
    }
#else
#define ASSERT_VULKAN(val)
#endif


class HelloTriangleApplication
{
  public:
    HelloTriangleApplication(uint32_t width, uint32_t height);

    void run();

  private:
    bool framebufferResized = false;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkCommandBuffer> commandBuffers;
    VkCommandPool commandPool;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkPipeline graphicsPipeline;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    VkSwapchainKHR swapChain;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkQueue graphicsQueue;
    VkDevice device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    GLFWwindow *window;
    const uint32_t Width;
    const uint32_t Height;
    VkInstance instance;
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkDebugUtilsMessengerEXT debugMessenger;

    void recreateSwapChain();

    bool checkValidationLayerSupport();

    void createInstance();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);

    void setupDebugMessenger();

    std::vector<const char *> getRequiredExtensions();

    void initVulkan();

    void createSyncObjects();

    void createCommandBuffers();

    void createCommandPool();

    void createFramebuffers();

    void createGraphicsPipeline();

    void createRenderPass();

    VkShaderModule createShaderModule(const std::vector<char> &code);

    void createImageViews();

    void createSwapChain();

    void createSurface();

    void createLogicalDevice();

    void pickPhysicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete();
    };

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void initWindow();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    void mainLoop();

    void drawFrame();

    void cleanupSwapChain();

    void cleanup();

    static std::vector<char> readFile(const std::string &filename);
};