#include "Renderer/MasterRenderer.hpp"

#include "Texture/TextureManager.hpp"

MasterRenderer::MasterRenderer() {
    // Game Class 初始化的時候就會執行

    // 建立 Shaders
    basic_shader = std::make_unique<BasicShader>();
    screen_shader = std::make_unique<ScreenShader>();
    gaussian_blur_shader = std::make_unique<GaussianBlurShader>();
    volume_shader = std::make_unique<VolumeShader>();

    // 建立 Renderer
    axes_renderer = std::make_unique<AxesRenderer>(basic_shader.get());
    screen_renderer = std::make_unique<ScreenRenderer>(screen_shader.get());
    gaussian_blur_renderer = std::make_unique<GaussianBlurRenderer>(gaussian_blur_shader.get());
    volume_renderer = std::make_unique<VolumeRenderer>(volume_shader.get());

    // 設定 gl
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MasterRenderer::Initialize() {
    // 在每一次的 Game loop 都會執行，且在分割畫面之前

    // 是否開啟 Back Face Culling
    if (state.world->culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }

    // 清除快取
    glClearColor(state.world->background_color.r, state.world->background_color.g, state.world->background_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void MasterRenderer::Render(const std::unique_ptr<Camera>& camera) {
    // Viewport settings
    camera->SetViewPort();

    // 繪製 Volume
    if (state.world->my_volume) {
        volume_renderer->Prepare(camera);
        volume_renderer->Render(state.world->my_volume.get());
    }

    glDisable(GL_DEPTH_TEST);
    // 繪製 xyz 三軸
    if (state.world->draw_axes) {
        axes_renderer->Prepare(camera);
        axes_renderer->Render(5.0f);
        axes_renderer->Render(state.world->camera, 10.0f);
    }
    glEnable(GL_DEPTH_TEST);
}

void MasterRenderer::Destroy() {
    basic_shader->Destroy();
    screen_shader->Destroy();
    gaussian_blur_shader->Destroy();
    volume_shader->Destroy();
}

void MasterRenderer::GaussianBlur(bool is_horizontal, bool first_iteration) {
    gaussian_blur_renderer->Prepare(is_horizontal);
    if (first_iteration) {
        gaussian_blur_renderer->Render(&TextureManager::GetTexture2D("Bloom"), state.world->my_screen.get());
    } else {
        gaussian_blur_renderer->Render(&TextureManager::GetTexture2D("GaussianBlur" + std::to_string(!is_horizontal)), state.world->my_screen.get());
    }
}

void MasterRenderer::RenderScreen() {
    // Call By Application，在每一次 main loop 的結尾執行
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    screen_renderer->Prepare();
    screen_renderer->Render(&TextureManager::GetTexture2D("PostProcessing"), &TextureManager::GetTexture2D("GaussianBlur0"), state.world->my_screen.get());
}