#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <iostream>
#include <stb.h>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <nameof.hpp>
#include <optional>
#include <set>
#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm> // Necessary for std::min/std::max

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