#ifndef VOLUMESHADER_HPP
#define VOLUMESHADER_HPP

#include <string>
#include <memory>

#include "Shader.hpp"
#include "Light/Light.hpp"

struct VolumeShader : public Shader {
    VolumeShader();
    void SetPointLight(const std::unique_ptr<Light>& point_light);

private:
    static const std::string VERTEX_FILE;
    static const std::string FRAGMENT_FILE;
};

#endif
