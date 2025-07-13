#include "core/Engine.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    // The Engine class encapsulates the application's entire lifecycle.
    // It's instantiated here in the main function, which serves as the entry point.
    Engine engine{};

    try {
        engine.run();
    } catch (const std::exception &e) {
        // Any unrecoverable error during the engine's setup or main loop
        // will throw an exception caught here.
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
