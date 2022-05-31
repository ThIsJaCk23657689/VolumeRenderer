#include "Geometry/Geometry.hpp"

Geometry::~Geometry() {
    Destroy();
}

void Geometry::Initialize() {
    m_vao = 0, m_vbo = 0, m_ebo = 0;
    GenerateVertices();
    BufferInitialize();
}

void Geometry::UpdateVertices() {
    // 更新只需要改 vbo 就好，ebo 完全不需要去動到
    // 因為幾乎每一幀數都會更新資料，建議將 glBuffData() 的最後一個參數改為 GL_DYNAMIC_DRAW
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(Attributes::Position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glVertexAttribPointer(Attributes::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(Attributes::Texture, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, texture_coordinate)));
    glBindVertexArray(0);
}

void Geometry::Bind() const {
    glBindVertexArray(m_vao);
}

void Geometry::UnBind() const {
    glBindVertexArray(0);
}

void Geometry::DrawOnly() const {
    if (m_ebo == 0) {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertices.size()));
    } else {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    }
}

void Geometry::Draw() const {
    Bind();
    DrawOnly();
    UnBind();
}

void Geometry::Destroy() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Geometry::BufferInitialize() {
    // 建立 VAO VBO EBO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(Attributes::Position);
    glEnableVertexAttribArray(Attributes::Normal);
    glEnableVertexAttribArray(Attributes::Texture);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Attributes::Position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glVertexAttribPointer(Attributes::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));
    glVertexAttribPointer(Attributes::Texture, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, texture_coordinate)));

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Geometry::Clear() {
    m_vertices.clear();
    m_indices.clear();
}