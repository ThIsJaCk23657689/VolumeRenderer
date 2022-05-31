#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glad/glad.h>

#include <vector>

#include "Model/Vertex.hpp"

constexpr float PI = 3.14159265359f;

enum Attributes : GLuint {
    Position = 0,
    Normal = 1,
    Texture = 2
};

struct Geometry {
    GLuint m_vao, m_vbo, m_ebo;

    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    Geometry() = default;
    ~Geometry();

    void Initialize();
    void UpdateVertices();
    void Bind() const;
    void UnBind() const;
    void DrawOnly() const;
    void Draw() const;
    void Destroy();

protected:
    virtual void GenerateVertices() = 0;
    virtual void BufferInitialize();
    void Clear();
};
#endif