#ifndef WORLD_HPP
#define WORLD_HPP

#include <glm/glm.hpp>

#include <array>
#include <memory>

#include "Geometry/3D/Cube.hpp"
#include "Geometry/2D/Screen.hpp"

#include "Model/Volume.hpp"

#include "Entity.hpp"
#include "Material/Material.hpp"
#include "Player.hpp"

#include "Camera.hpp"
#include "Light/Light.hpp"

#include "Texture/Texture1D.hpp"

enum PostEffect : unsigned int {
    NORMAL = 0,
    INVERSION = 1,
    GRAYSCALE = 2,
    NARCOTIC = 3,
    BLUR = 4,
    EDGE_DETECTION = 5,
};

enum HDRMode : unsigned int {
    REINHARD = 0,
    EXPOSURE = 1,
};

struct World {
    // Geometry Shapes
    std::unique_ptr<Cube> my_cube = nullptr;
    std::unique_ptr<Cube> view_volume = nullptr;
    std::unique_ptr<Screen> my_screen = nullptr;

    // Voxel
    std::unique_ptr<Volume> my_volume = nullptr;

    // Entity (For movement)
    Entity camera;

    // Player
    std::unique_ptr<Player> me = nullptr;

    // Camera
    std::unique_ptr<Camera> my_camera = nullptr;

    // Lighting
    std::unique_ptr<Light> my_point_light = nullptr;

    // Screen
    PostEffect current_screen_mode = PostEffect::NORMAL;

    // Settings
    bool draw_axes = false;
    bool culling = true;
    bool mouse_control = false;

    // Bloom
    bool use_bloom = true;
    float bloom_intensity = 1.0f;
    float bloom_threshold = 0.7f;
    int bloom_strength = 20;

    // Gamma Correction
    bool use_gamma_correction = true;
    float gamma_value = 2.2f;

    // HDR
    bool use_hdr = true;
    HDRMode current_hdr_mode = HDRMode::EXPOSURE;
    float hdr_exposure = 1.0f;

    // volume rendering settings
    char volume_data_folder_path[1024] = "assets/volumes/";
    std::string current_volume_data = "Please select a volume file (.toml)";
    std::vector<std::string> volume_data_files;
    bool use_lighting = true;
    bool use_normal_color = false;
    float sample_rate = 0.5f;
    glm::vec3 background_color = glm::vec3(0.01f, 0.01f, 0.01f);

    void Create();
    void Destroy();

    void ScanVolumeDataFolder();
};

#endif