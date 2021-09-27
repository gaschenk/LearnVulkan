// LearnVulkan.cpp : Defines the entry point for the application.
//

#include "LearnVulkan.h"

#include "Game.h"
#include <vector>

int main()
{
    Game game;
    game.init();
    game.run();
    game.shutdown();
    return 0;
}
