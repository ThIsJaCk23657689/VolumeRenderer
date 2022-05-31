#include "Maths/IntegerVector.hpp"

namespace Maths {
    ivec2::ivec2(int _x, int _y) : x(_x), y(_y)  {

    }

    ivec2::ivec2(int scalar) : x(scalar), y(scalar) {

    }

    ivec3::ivec3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {

    }

    ivec3::ivec3(int scalar) : x(scalar), y(scalar), z(scalar) {

    }

    const glm::vec3 ivec3::GetVec3() const {
        return {x, y, z};
    }
}