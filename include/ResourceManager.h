#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

class ResourceManager {
public:
    static ResourceManager& getInstance();

    // Texture management
    bool loadTexture(const std::string& name, const std::string& filename);
    sf::Texture* getTexture(const std::string& name);

    // Font management
    bool loadFont(const std::string& name, const std::string& filename);
    sf::Font* getFont(const std::string& name);

    // Sound management
    bool loadSoundBuffer(const std::string& name, const std::string& filename);
    sf::SoundBuffer* getSoundBuffer(const std::string& name);

    // Create sound with loaded buffer
    std::unique_ptr<sf::Sound> createSound(const std::string& bufferName);

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
};

#endif // RESOURCE_MANAGER_H