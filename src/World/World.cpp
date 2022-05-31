#include "World/World.hpp"

#include "State.hpp"
#include "Texture/TextureManager.hpp"

void World::Create() {
    // Geometry Initialize
    my_cube = std::make_unique<Cube>();
    view_volume = std::make_unique<Cube>();
    my_screen = std::make_unique<Screen>();

    // Volume Loader
    // my_volume = std::make_unique<Volume>("engine.toml");

    // Load Images and Create Textures
    TextureManager::Initialize();

    // Entity Initialize
    camera = Entity(glm::vec3(0.0f, 10.0f, 430.0f), glm::vec3(0.0f), glm::vec3(8.0f, 5.0f, 10.0f), glm::vec3(0.2f));
    camera.movable = true;

    // Player Initialize
    me = std::make_unique<Player>(&camera);

    // Camera Initialize
    // my_camera = std::make_unique<Camera>(true);
    my_camera = std::make_unique<Camera>(glm::vec3(0.0, 0.0, 400.0), glm::vec3(0.0, 0.0, 0.0), true);
    my_camera->HookEntity(camera);

    // Light Initialize
    my_point_light = std::make_unique<Light>(my_camera->position, true);
    my_point_light->UpdateColor(glm::vec3(1.0f, 1.0f, 1.0f));
    my_point_light->HookCamera(my_camera.get());
    my_point_light->linear = 0.0f;
    my_point_light->quadratic = 0.0f;

    // Volume Initialize
    ScanVolumeDataFolder();
    // my_volume->GenerateTFTexture(state.ui->m_transfer_function);
}

void World::Destroy() {
    TextureManager::Destroy();
}

void World::ScanVolumeDataFolder() {
    volume_data_files.clear();
    for (const auto& entry : std::filesystem::directory_iterator(volume_data_folder_path)) {
        if (entry.path().extension() == ".toml") {
            volume_data_files.push_back(entry.path().filename().string());
        }
    }
}