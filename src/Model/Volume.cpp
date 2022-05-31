#include "Model/Volume.hpp"

#include <toml++/toml.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <thread>

#include "Maths/Gradient.hpp"
#include "Utility/Logger.hpp"

Volume::Volume(const std::string& info_file, const std::string& raw_file) :
    m_vao(0), m_vbo(0), m_ebo(0) {
    m_info.info_file_path = info_file;
    if (!raw_file.empty()) {
        m_info.raw_file_path = raw_file;
    }
    Initialize();
}

Volume::~Volume() {
    Destroy();
}

void Volume::Initialize() {

    auto start = std::chrono::steady_clock::now();

    LoadInfo();
    LoadRaw();
    ComputeNormals();

    GenerateVertices();
    BufferInitialize();

    auto end = std::chrono::steady_clock::now();
    m_loading_cost = end - start;

    ShowMe();
//    std::vector<int> counts(256, 0);
//    for (int i = 0; i < m_data.size(); i++) {
//        counts[m_data[i]]++;
//    }
//
//    for (int i = 0; i < 256; i++) {
//        std::cout << "m_data(value, counts): (" << i << ", " << counts[i] << ")\n";
//    }
//    auto max = std::max_element(m_distance.cbegin(), m_distance.cend());
//    auto min = std::min_element(m_distance.cbegin(), m_distance.cend());
//    std::cout << "Max: " << *max << std::endl;
//    std::cout << "Min: " << *min << std::endl;
}

void Volume::Bind() const {
    glBindVertexArray(m_vao);
}

void Volume::UnBind() const {
    glBindVertexArray(0);
}

void Volume::DrawOnly() const {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
}

void Volume::Draw() const {
    Bind();
    DrawOnly();
    UnBind();
}

void Volume::Destroy() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    Clear();
}

void Volume::GenerateTFTexture(const TransferFunctionWidget& tf_widget) {
    const std::vector<float>& colormap = tf_widget.GetColorData();
    const size_t texel_count = colormap.size() / sizeof(float);

    // Make it into 1D texture
    m_transfer_texture.Bind();
    m_transfer_texture.Generate(GL_RGBA, GL_RGBA, texel_count, colormap.data());
    m_transfer_texture.UnBind();
}

int Volume::GetIndex(const int &i, const int &j, const int &k) const {
    return k * (m_info.resolution.y * m_info.resolution.x) + (j * m_info.resolution.x) + i;
}

float Volume::GetVoxelVal(const int &i, const int &j, const int &k) const {
    int index = GetIndex(i, j, k);
    assert(index >= 0 && index < m_data.size());
    return m_data[GetIndex(i, j, k)];
}

void Volume::ShowMe() {
    Maths::ivec3& res = m_info.resolution;
    glm::vec3& size = m_info.voxel_size;

    Logger::Separator();
    Logger::Message(LogLevel::Debug, "Info File: " + m_info.info_file_path);
    Logger::Message(LogLevel::Debug, "Raw File: " + m_info.raw_file_path);
    Logger::Message(LogLevel::Debug, "Resolution: (" + std::to_string(res.x) + ", " + std::to_string(res.y) + ", " + std::to_string(res.z) + ")");
    Logger::Message(LogLevel::Debug, "Voxel Size: (" + std::to_string(size.x) + ", " + std::to_string(size.y) + ", " + std::to_string(size.z) + ")");
    Logger::Message(LogLevel::Debug, "Sample Type: " + ShowSampleType());
    Logger::Message(LogLevel::Debug, "Endianness: " + ShowEndianness());
    Logger::Message(LogLevel::Debug, "Size of Raw Data: " + std::to_string(m_data.size()));
    Logger::Message(LogLevel::Debug, "Cost Time: " + std::to_string(m_loading_cost.count()) + " seconds.");
    Logger::Spacing();
}

std::string Volume::ShowSampleType() const {
    switch (m_info.sample_type) {
        case SampleType::UnsignedChar:
            return "Unsigned Char";
        case SampleType::UnsignedShort:
            return "Unsigned Short";
        case SampleType::Short:
            return "Short";
        case SampleType::Float:
            return "Float";
        default:
            return "";
    }
}

std::string Volume::ShowEndianness() const {
    switch (m_info.endian) {
        case Endianness::Little:
            return "Little-Endian";
        case Endianness::Big:
            return "Big-Endian";
        default:
            return "";
    }
}

void Volume::ComputeNormals() {
    for (int k = 0; k < m_info.resolution.z; k++) {
        for (int j = 0; j < m_info.resolution.y; j++) {
            for (int i = 0; i < m_info.resolution.x; i++) {
                glm::vec3 norm = Maths::Gradient::Compute(i, j, k, *this);

                // 不要 normalize，不然會出現方格塊狀
                // norm = glm::normalize(norm);
                m_normals.push_back(norm);
            }
        }
    }
}

void Volume::GenerateTextureData() {
    const auto& temp_data = m_data;

    // 1. Generate a new data (r, g, b, a) and sent into GPU rgb as normal and a as voxel value;
    const float& max_value = *std::max_element(temp_data.cbegin(), temp_data.cend());

    for (int i = 0; i < temp_data.size(); i++) {
        glm::vec3 norm = m_normals[i];
        float val = temp_data[i] / max_value;
        m_texture_data.emplace_back(norm, val);
    }

    // 2. Creating a 3D texture.
    m_texture.Generate(GL_RGBA32F, GL_RGBA,
                       m_info.resolution.x, m_info.resolution.y, m_info.resolution.z,
                       reinterpret_cast<const float *>(m_texture_data.data()));
}

void Volume::GenerateVertices() {
    GenerateTextureData();

    // Creating a cube with texture coordinate.
    float res_x = static_cast<float>(m_info.resolution.x) * m_info.voxel_size.x;
    float res_y = static_cast<float>(m_info.resolution.y) * m_info.voxel_size.y;
    float res_z = static_cast<float>(m_info.resolution.z) * m_info.voxel_size.z;

    m_vertices = {
        {res_x, res_y, 0.0, 1.0, 1.0, 0.0},
        {res_x, 0.0, 0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, res_y, 0.0, 0.0, 1.0, 0.0},
        {res_x, res_y, res_z, 1.0, 1.0, 1.0},
        {res_x, 0.0, res_z, 1.0, 0.0, 1.0},
        {0.0, 0.0, res_z, 0.0, 0.0, 1.0},
        {0.0, res_y, res_z, 0.0, 1.0, 1.0},
    };

    m_indices = {
        0, 1, 2,
        0, 2, 3,
        3, 2, 6,
        3, 6, 7,
        7, 6, 5,
        7, 5, 4,
        4, 5, 1,
        4, 1, 0,
        4, 0, 3,
        4, 3, 7,
        6, 2, 1,
        6, 1, 5
    };
}

void Volume::BufferInitialize() {
    // 建立 VAO VBO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(Attributes::Position);
    glEnableVertexAttribArray(Attributes::Texture);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VolumeVertex), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Attributes::Position, 3, GL_FLOAT, GL_FALSE, sizeof(VolumeVertex), reinterpret_cast<const void*>(offsetof(VolumeVertex, position)));
    glVertexAttribPointer(Attributes::Texture, 3, GL_FLOAT, GL_FALSE, sizeof(VolumeVertex), reinterpret_cast<const void*>(offsetof(VolumeVertex, texture_coordinate)));

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Volume::Clear() {
    m_data.clear();
    m_normals.clear();
    m_vertices.clear();
    m_indices.clear();
}

void Volume::LoadInfo() {
    // 1. Load the Info file first (a TOML File)
    toml::table tbl;
    try {
        tbl = toml::parse_file(m_info.info_file_path);
    } catch (const toml::parse_error& err) {
        Logger::Message(LogLevel::Error, "Failed to parsing info file (TOML) at file: " + m_info.info_file_path + "\n reason: \n" + std::string(err.description()) + "\n");
        exit(-1);
    }

    const auto& resolution_node = tbl["resolution"];
    const auto& voxel_node = tbl["voxel"];

    // Relative Path to the info file
    if (m_info.raw_file_path.empty()) {
        auto parent_dir = std::filesystem::path(m_info.info_file_path).parent_path();
        auto raw_name = tbl["raw_file"].value_or(
            std::filesystem::path(m_info.info_file_path).filename().replace_extension(".raw").string()
        );
        m_info.raw_file_path = (parent_dir / raw_name).string();
    }

    // Loading the attributes of the volume file
    m_info.resolution.x = resolution_node["x"].value_or<int>(0);
    m_info.resolution.y = resolution_node["y"].value_or<int>(0);
    m_info.resolution.z = resolution_node["z"].value_or<int>(0);

    m_info.voxel_size.x = voxel_node["size"]["x"].value_or<float>(0);
    m_info.voxel_size.y = voxel_node["size"]["y"].value_or<float>(0);
    m_info.voxel_size.z = voxel_node["size"]["z"].value_or<float>(0);

    const std::string endian_string = voxel_node["endianness"].value_or("little");
    if (endian_string == "big") {
        m_info.endian = Endianness::Big;
    } else {
        m_info.endian = Endianness::Little;
    }

    const std::string sample_type_string = voxel_node["sample_type"].value_or("");
    if (sample_type_string == "unsigned char") {
        m_info.sample_type = SampleType::UnsignedChar;
    } else if (sample_type_string == "unsigned short") {
        m_info.sample_type = SampleType::UnsignedShort;
    } else if (sample_type_string == "short") {
        m_info.sample_type = SampleType::Short;
    } else if (sample_type_string == "float") {
        m_info.sample_type = SampleType::Float;
    }

    // Assertions in Debugs mode
    assert(m_info.resolution.x > 0 && m_info.resolution.y > 0 && m_info.resolution.z > 0);
    assert(m_info.voxel_size.x > 0.0f && m_info.voxel_size.y > 0.0f && m_info.voxel_size.z > 0.0f);
    assert(!sample_type_string.empty());
}

void Volume::LoadRaw() {
    // 1. Load the RAW file
    std::ifstream file(m_info.raw_file_path, std::ios::binary);
    if (file.fail()) {
        Logger::Message(LogLevel::Error, "Failed to load the RAW file, file path: " + m_info.raw_file_path);
        exit(-1);
    }
    size_t file_size = std::filesystem::file_size(m_info.raw_file_path);

    switch (m_info.sample_type) {
        case SampleType::UnsignedChar: {
            std::vector<uint8_t> buffer(file_size / sizeof(uint8_t));
            file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
            m_data.assign(buffer.begin(), buffer.end());
            break;
        }
        case SampleType::UnsignedShort: {
            std::vector<uint16_t> buffer(file_size / sizeof(uint16_t));
            file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
            if (m_info.endian == Endianness::Big) {
                for (auto& single : buffer) {
                    single = ntoh(single);
                }
            }
            m_data.assign(buffer.begin(), buffer.end());
            break;
        }
        case SampleType::Short: {
            std::vector<int16_t> buffer(file_size / sizeof(int16_t));
            file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
            if (m_info.endian == Endianness::Big) {
                for (auto& single : buffer) {
                    single = ntoh(single);
                }
            }
            m_data.assign(buffer.begin(), buffer.end());
            break;
        }
        case SampleType::Float: {
            std::vector<float> buffer(file_size / sizeof(float));
            file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(file_size));
            if (m_info.endian == Endianness::Big) {
                for (auto& single : buffer) {
                    single = ntoh(single);
                }
            }
            m_data.assign(buffer.begin(), buffer.end());
            break;
        }
    }
    file.close();
}