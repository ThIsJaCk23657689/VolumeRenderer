#include "Shader/VolumeShader.hpp"

const std::string VolumeShader::VERTEX_FILE = "assets/shaders/volume.vert";
const std::string VolumeShader::FRAGMENT_FILE = "assets/shaders/volume.frag";

VolumeShader::VolumeShader() : Shader(VERTEX_FILE, FRAGMENT_FILE) {}

void VolumeShader::SetPointLight(const std::unique_ptr<Light>& point_light) {
    SetVec3("light.position", point_light->entity.position);
    SetVec3("light.color", point_light->color);
}
