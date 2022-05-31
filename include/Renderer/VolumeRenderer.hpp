#ifndef VOLUMERENDERER_HPP
#define VOLUMERENDERER_HPP

#include "Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Shader/VolumeShader.hpp"
#include "Model/Volume.hpp"

struct VolumeRenderer : public Renderer {
    VolumeRenderer(VolumeShader* shader);
    void Prepare(const std::unique_ptr<Camera>& camera) override;
    void Render(const Volume* volume);

private:
    VolumeShader* m_shader;

};

#endif