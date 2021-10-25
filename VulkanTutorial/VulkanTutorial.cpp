#include "VulkanTutorial.h"
#include <SFML/Network.hpp>
int main()
{
    HelloTriangleApplication app(800, 600);

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}