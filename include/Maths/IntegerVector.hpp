#ifndef INTEGERVECTOR_HPP
#define INTEGERVECTOR_HPP

#include "glm/glm.hpp"

namespace Maths {
    struct ivec2 {
        ivec2(int _x, int _y);
        explicit ivec2(int scalar = 0);

        int x;
        int y;
    };

    struct ivec3 {
        ivec3(int _x, int _y, int _z);
        explicit ivec3(int scalar = 0);

        int x;
        int y;
        int z;

        const glm::vec3 GetVec3() const;
    };
}

#endif
