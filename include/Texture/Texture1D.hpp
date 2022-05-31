#ifndef TEXTURE1D_HPP
#define TEXTURE1D_HPP

#include <glad/glad.h>
#include <string>

struct Texture1D {
    Texture1D();

    void Active(GLint texture_idx = GL_TEXTURE0) const;
    void Bind() const;
    void UnBind() const;
    void Destroy() const;
    void Generate(GLint internal_format, GLenum format, int image_width, const float* data);

    void SetWrapParameters(GLint wrap_s) const;
    void SetFilterParameters(GLint min_filter, GLint mag_filter) const;

    unsigned int m_id;
};

#endif
