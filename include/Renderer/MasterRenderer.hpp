#ifndef MASTERRENDERER_HPP
#define MASTERRENDERER_HPP

#include <memory>
#include <vector>

#include "Camera.hpp"

#include "Shader/BasicShader.hpp"
#include "Shader/ScreenShader.hpp"
#include "Shader/GaussianBlurShader.hpp"
#include "Shader/VolumeShader.hpp"

#include "Renderer/AxesRenderer.hpp"
#include "Renderer/ScreenRenderer.hpp"
#include "Renderer/GaussianBlurRenderer.hpp"
#include "Renderer/VolumeRenderer.hpp"

#include "World/Entity.hpp"

struct MasterRenderer {
    MasterRenderer();

    void Initialize();
    void Render(const std::unique_ptr<Camera>& camera);
    void Destroy();

    void GaussianBlur(bool is_horizontal, bool first_iteration);
    void RenderScreen();

private:
    // Shaders
    std::unique_ptr<BasicShader> basic_shader = nullptr;
    std::unique_ptr<ScreenShader> screen_shader = nullptr;
    std::unique_ptr<GaussianBlurShader> gaussian_blur_shader = nullptr;
    std::unique_ptr<VolumeShader> volume_shader = nullptr;

    // Renderers
    std::unique_ptr<AxesRenderer> axes_renderer = nullptr;
    std::unique_ptr<ScreenRenderer> screen_renderer = nullptr;
    std::unique_ptr<GaussianBlurRenderer> gaussian_blur_renderer = nullptr;
    std::unique_ptr<VolumeRenderer> volume_renderer = nullptr;
};

#endif