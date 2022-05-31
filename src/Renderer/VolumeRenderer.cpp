#include "Renderer/VolumeRenderer.hpp"

VolumeRenderer::VolumeRenderer(VolumeShader* shader) : m_shader(shader) {

}

void VolumeRenderer::Prepare(const std::unique_ptr<Camera>& camera) {
    m_shader->Start();
    m_shader->SetBool("useLighting", state.world->use_lighting);
    m_shader->SetBool("useNormalColor", state.world->use_normal_color);
    m_shader->SetInt("volume", 0);
    m_shader->SetInt("transfer_function", 1);
    m_shader->SetFloat("sample_rate", state.world->sample_rate);
    m_shader->SetVec3("background_color", state.world->background_color);

    m_shader->SetFloat("bloomThreshold", state.world->bloom_threshold);

    // Load Lights
    m_shader->SetPointLight(state.world->my_point_light);
    m_shader->SetFloat("shininess", 256.0f);

    // Load View and Projection Matrix
    m_shader->SetViewAndProj(camera);
    m_shader->SetVec3("viewPos", camera->position);
}

void VolumeRenderer::Render(const Volume* volume) {
    // Prepare Geometry (Bind VAO)
    volume->Bind();

    // For Reflection
    // CubeMap skybox = TextureManager::GetCubeMap("SkyBox");
    // skybox.Active(GL_TEXTURE1);
    // skybox.Bind();

    volume->m_texture.Active(GL_TEXTURE0);
    volume->m_texture.Bind();
    volume->m_transfer_texture.Active(GL_TEXTURE1);
    volume->m_transfer_texture.Bind();


    // Prepare Material (Only Color)
    m_shader->SetVec3("volume_resolution", volume->m_info.resolution.GetVec3());
    m_shader->SetVec3("volume_ratio", volume->m_info.voxel_size);

    // Prepare Instance
    glm::vec3 resolution = glm::vec3(volume->m_info.resolution.x, volume->m_info.resolution.y, volume->m_info.resolution.z);
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, resolution * volume->m_info.voxel_size * -0.5f);
    m_shader->SetMat4("model", model_matrix);

    // Draw
    volume->DrawOnly();

    // Unbind VAO and Texture
    volume->UnBind();
    // entity.material.texture->UnBind();
}
