#pragma once

#include "Primitives.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Manages a collection of named UI elements for easy access and iteration.
class UIManager {

private:
    std::unordered_map<std::string, std::unique_ptr<Primitives::Primitive>> elements;


public:
    // Adds a UI element to the manager with a unique name.
    void add(const std::string &name, std::unique_ptr<Primitives::Primitive> element);

    // Retrieves a raw pointer to a UI element by its name for modification.
    Primitives::Primitive *get(const std::string &name);

    // Provides read-only access to the underlying map of elements for rendering.
    const std::unordered_map<std::string, std::unique_ptr<Primitives::Primitive>> &getElements() const;

};
