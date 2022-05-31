#ifndef VOLUME_HPP
#define VOLUME_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>

#include "Geometry/Geometry.hpp"
#include "Maths/IntegerVector.hpp"
#include "Texture/Texture3D.hpp"
#include "Texture/Texture1D.hpp"
#include "GUI/TransferFunctionWidget.hpp"

enum class Endianness : unsigned int {
    Little,
    Big,
};

enum class SampleType : unsigned int {
    UnsignedChar,
    UnsignedShort,
    Short,
    Float
};

struct Volume {
    struct Info {
        std::string info_file_path;
        std::string raw_file_path;
        Endianness endian;
        SampleType sample_type;
        Maths::ivec3 resolution;
        glm::vec3 voxel_size;
        std::string voxel_unit;
    } m_info;
    std::vector<float> m_data;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec4> m_texture_data;
    Texture3D m_texture;
    Texture1D m_transfer_texture;

    GLuint m_vao, m_vbo, m_ebo;
    std::vector<VolumeVertex> m_vertices;
    std::vector<GLuint> m_indices;

    std::chrono::duration<double> m_loading_cost;

    explicit Volume(const std::string& info_file, const std::string& raw_file = "");
    ~Volume();

    void Initialize();
    void Bind() const;
    void UnBind() const;
    void DrawOnly() const;
    void Draw() const;
    void Destroy();
    void GenerateTFTexture(const TransferFunctionWidget& tf_widget);

    int GetIndex(const int& i, const int& j, const int& k) const;
    float GetVoxelVal(const int& i, const int& j, const int& k) const;
    void ShowMe();
    std::string ShowSampleType() const;
    std::string ShowEndianness() const;

protected:
    void ComputeNormals();
    void GenerateTextureData();
    void GenerateVertices();
    void BufferInitialize();
    void Clear();

private:
    void LoadInfo();
    void LoadRaw();

    /**
     * Network byte order(Big-Endian) convert to host byte order(Little-Endian)
     */
    template<typename T>
    T ntoh(const T& value) {
        constexpr std::size_t byte_count = sizeof(T);

        T result;

        const char* const big = reinterpret_cast<const char*>(&value);
        char* little = reinterpret_cast<char*>(&result);

        for (std::size_t i = 0; i < byte_count; i++) {
            little[i] = big[byte_count - (i + 1)];
        }

        return result;
    }
};

#endif
