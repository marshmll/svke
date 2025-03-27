#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "App.hpp"

int main()
{
    vk::App app = vk::App();

    try
    {
        app.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
