#pragma once

#include <iostream>
#include <string>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#ifdef _DEBUG
#define ASSERT_VULKAN(val)                                                                                             \
    if (val != VK_SUCCESS)                                                                                             \
    {                                                                                                                  \
        __debugbreak();                                                                                                \
        std::cout << "An Error occured:" << std::endl;                                                                 \
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
    static void printVkPhysicalDeviceInfo(const VkPhysicalDevice &device);

  public:
    void init();
    void run();
    void shutdown() const;

  private:
    VkInstance instance;
    VkDevice device;
    GLFWwindow *window;
    VkSurfaceKHR surface;
};