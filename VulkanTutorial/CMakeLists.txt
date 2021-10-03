# CMakeList.txt : CMake project for LearnVulkan, include source and define
# project specific logic here.
#

set(EXECUTABLE_NAME "VulkanTutorial")
set(CMAKE_CXX_STANDARD_REQUIRED 23)
set(CMAKE_CXX_STANDARD 23)
cmake_minimum_required (VERSION 3.8)

#find required include dirs
find_path(STB_INCLUDE_DIRS "stb.h")


#find required packages
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nameof CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (${EXECUTABLE_NAME} "VulkanTutorial.cpp" "VulkanTutorial.h" "HelloTriangleApplication.cpp" "HelloTriangleApplication.h" "Vertex.h")

#add include dirs
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${STB_INCLUDE_DIRS})

#link required packages
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glfw glm::glm Vulkan::Vulkan nameof::nameof)

# Symlink content folder to output dir
add_custom_command(
    TARGET ${EXECUTABLE_NAME}
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
    create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/content $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/content)

# TODO: Add tests and install targets if needed.