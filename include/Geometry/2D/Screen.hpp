#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <glad/glad.h>

#include <vector>

#include "Geometry/Geometry.hpp"

struct Screen {
    GLuint m_vao, m_vbo, m_ebo;
    std::vector<GLfloat> m_vertices;
    std::vector<GLuint> m_indices;

    Screen();
    ~Screen();

    void Initialize();
    void Draw() const;

protected:
    void GenerateVertices();
    void BufferInitialize();
    void Clear();

};
#endif