#include "ResourceManager.h"
#include <iostream>

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& filename) {
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filename)) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }
    
    textures[name] = std::move(texture);
    return true;
}

sf::Texture* ResourceManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    return (it != textures.end()) ? it->second.get() : nullptr;
}

bool ResourceManager::loadFont(const std::string& name, const std::string& filename) {
    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(filename)) {
        std::cerr << "Failed to load font: " << filename << std::endl;
        return false;
    }
    
    fonts[name] = std::move(font);
    return true;
}

sf::Font* ResourceManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    return (it != fonts.end()) ? it->second.get() : nullptr;
}

bool ResourceManager::loadSoundBuffer(const std::string& name, const std::string& filename) {
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filename)) {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return false;
    }
    
    soundBuffers[name] = std::move(buffer);
    return true;
}

sf::SoundBuffer* ResourceManager::getSoundBuffer(const std::string& name) {
    auto it = soundBuffers.find(name);
    return (it != soundBuffers.end()) ? it->second.get() : nullptr;
}

std::unique_ptr<sf::Sound> ResourceManager::createSound(const std::string& bufferName) {
    auto buffer = getSoundBuffer(bufferName);
    if (!buffer) {
        std::cerr << "Sound buffer not found: " << bufferName << std::endl;
        return nullptr;
    }
    
    auto sound = std::make_unique<sf::Sound>();
    sound->setBuffer(*buffer);
    return sound;
}