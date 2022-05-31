#include "Texture/Texture1D.hpp"

Texture1D::Texture1D() : m_id(0) {
    glGenTextures(1, &m_id);
}

void Texture1D::Active(GLint texture_idx) const {
    glActiveTexture(texture_idx);
}

void Texture1D::Bind() const {
    glBindTexture(GL_TEXTURE_1D, m_id);
}

void Texture1D::UnBind() const {
    glBindTexture(GL_TEXTURE_1D, 0);
}

void Texture1D::Destroy() const {
    glDeleteTextures(1, &m_id);
}

void Texture1D::SetWrapParameters(GLint wrap_s) const {
    Bind();
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrap_s);
    UnBind();
}

void Texture1D::SetFilterParameters(GLint min_filter, GLint mag_filter) const {
    Bind();
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, mag_filter);
    UnBind();
}

void Texture1D::Generate(GLint internal_format, GLenum format, int image_width, const float* data) {
    Bind();
    glTexImage1D(GL_TEXTURE_1D, 0, internal_format, image_width, 0, format, GL_FLOAT, data);

    SetWrapParameters(GL_CLAMP_TO_EDGE);
    SetFilterParameters(GL_LINEAR, GL_LINEAR);
    UnBind();
}
