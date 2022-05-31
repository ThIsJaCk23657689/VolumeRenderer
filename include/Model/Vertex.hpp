#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <glm/glm.hpp>

struct Vertex2Attrib {
    struct Position {
        float x, y, z;
        float* ptr() { return reinterpret_cast<float*>(this); }
    };

    struct Normal {
        float x, y, z;
        float* ptr() { return reinterpret_cast<float*>(this); }
    };

    Position position;
    Normal normal;
};

struct VolumeVertex {
    struct Position {
        float x, y, z;
        float* ptr() { return reinterpret_cast<float*>(this); }
    };

    struct TexCoord3D {
        float u, v, w;
        float* ptr() { return reinterpret_cast<float*>(this); }
    };

    Position position;
    TexCoord3D texture_coordinate;
};

struct Vertex3Attrib {
    using Position = glm::vec3;
    using Normal = glm::vec3;
    using TexCoord = glm::vec2;

    Position position;
    Normal normal;
    TexCoord texture_coordinate;
};

typedef Vertex3Attrib Vertex;
#endif