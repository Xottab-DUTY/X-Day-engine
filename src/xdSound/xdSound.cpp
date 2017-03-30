#include <iostream>

#include "xdSound.hpp"

extern "C"
{
    XDSOUND_API void funcToExport()
    {
        std::cout << "Function imported and called!" << std::endl;
    }
}
