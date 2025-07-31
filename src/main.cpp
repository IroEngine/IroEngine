#include "core/Engine.hpp"
#include <iostream>
#include <stdexcept>

#ifdef __linux__
#include <fontconfig/fontconfig.h>
#endif

int main() {
    #ifdef __linux__
    // Initialize fontconfig on Linux to prevent runtime warnings.
    FcInit();
    #endif

    // The Engine class encapsulates the application's lifecycle.
    Engine engine {};

    try {
        engine.run();
    } catch (const std::exception &e) {
        // Catch all unrecoverable errors from the engine's setup or main loop.
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
