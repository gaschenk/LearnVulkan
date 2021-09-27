# CMakeList.txt : CMake project for LearnVulkan, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_OBJCXX_STANDARD 23)
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
# Add source to this project's executable.
add_executable (LearnVulkan "LearnVulkan.cpp" "Game.cpp")

target_link_libraries(LearnVulkan PRIVATE glfw glm::glm Vulkan::Vulkan fmt::fmt)

# TODO: Add tests and install targets if needed.
