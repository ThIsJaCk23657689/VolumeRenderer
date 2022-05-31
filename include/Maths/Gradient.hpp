#ifndef GRADIENT_HPP
#define GRADIENT_HPP

#include <glm/glm.hpp>
#include <vector>
#include "Maths/IntegerVector.hpp"
#include "Model/Volume.hpp"

namespace Maths {
    struct Gradient {
        static glm::vec3 Compute(const int& i, const int& j, const int& k, const Volume& volume);
    };
}

#endif
