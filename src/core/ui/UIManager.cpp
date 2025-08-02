#include "UIManager.hpp"

void UIManager::add(const std::string &name, std::unique_ptr<Primitives::Primitive> element) {
    if (elements.count(name)) {
        throw std::runtime_error("UIManager Error: An element with the name '" + name + "' already exists.");
    }
    elements[name] = std::move(element);
}

Primitives::Primitive *UIManager::get(const std::string &name) {
    try {
        return elements.at(name).get();
    } catch (const std::out_of_range &) {
        throw std::runtime_error("UIManager Error: Element with name '" + name + "' not found.");
    }
}

const std::unordered_map<std::string, std::unique_ptr<Primitives::Primitive>> &UIManager::getElements() const {
    return elements;
}
