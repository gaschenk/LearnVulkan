#pragma once

#include <iostream>
#include <nameof.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
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

class Game
{
    void initializeGLFW();
    void initializeVulkan();
    void shutdownVulkan() const;
    void shutdownGLFW() const;
    VkPhysicalDeviceProperties getDeviceProperties(const VkPhysicalDevice &device);
    std::vector<VkSurfaceFormatKHR> getSurfaceFormats(const VkPhysicalDevice &physical_device);
    int getBestDeviceId(std::vector<VkPhysicalDevice> &devices);

    std::vector<VkPresentModeKHR> getPresentModes(const VkPhysicalDevice physicalDevice);

    VkSurfaceCapabilitiesKHR getSurfaceCapabilities(const VkPhysicalDevice physicalDevice);

    void printDeviceSurfaceCapabilities(VkSurfaceCapabilitiesKHR &surfaceCapabilities);

    std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(const VkPhysicalDevice &device);

    void printVkPhysicalDeviceInfo(const VkPhysicalDevice &device, VkPhysicalDeviceProperties properties);

    void printQueueFamilyInfo(const std::vector<VkQueueFamilyProperties> properties);
    void printPropertyInfo(VkPhysicalDeviceProperties &properties);

  public:
    void init();
    void run();
    void shutdown() const;

  private:
    std::vector<VkImageView> imageViews;
    VkSwapchainKHR swapchain;
    VkInstance instance;
    VkDevice device;
    GLFWwindow *window;
    VkSurfaceKHR surface;
};