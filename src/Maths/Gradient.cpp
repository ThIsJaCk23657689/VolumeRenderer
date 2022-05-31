#include "Maths/Gradient.hpp"

namespace Maths {

    glm::vec3 Gradient::Compute(const int &i, const int &j, const int &k, const Volume& volume) {
        const ivec3& resolution = volume.m_info.resolution;
        auto norm = glm::vec3(0.0f);

        // x-axis
        if (i + 1 >= resolution.x) {
            // Backward Difference
            norm.x = (volume.GetVoxelVal(i, j, k) - volume.GetVoxelVal(i - 1, j, k)) / static_cast<float>(resolution.x);
        } else if (i - 1 < 0) {
            // Forward Difference
            norm.x = (volume.GetVoxelVal(i + 1, j, k) - volume.GetVoxelVal(i, j, k)) / static_cast<float>(resolution.x);
        } else {
            // Central Difference
            norm.x = (volume.GetVoxelVal(i + 1, j, k) - volume.GetVoxelVal(i - 1, j, k)) / (2 * static_cast<float>(resolution.x));
        }

        // y-axis
        if (j + 1 >= resolution.y) {
            // Backward Difference
            norm.y = (volume.GetVoxelVal(i, j, k) - volume.GetVoxelVal(i, j - 1, k)) / static_cast<float>(resolution.y);
        } else if (j - 1 < 0) {
            // Forward Difference
            norm.y = (volume.GetVoxelVal(i, j + 1, k) - volume.GetVoxelVal(i, j, k)) / static_cast<float>(resolution.y);
        } else {
            // Central Difference
            norm.y = (volume.GetVoxelVal(i, j + 1, k) - volume.GetVoxelVal(i, j - 1, k)) / (2 * static_cast<float>(resolution.y));
        }

        // z-axis
        if (k + 1 >= resolution.z) {
            // Backward Difference
            norm.z = (volume.GetVoxelVal(i, j, k) - volume.GetVoxelVal(i, j, k - 1)) / static_cast<float>(resolution.z);
        } else if (k - 1 < 0) {
            // Forward Difference
            norm.z = (volume.GetVoxelVal(i, j, k + 1) - volume.GetVoxelVal(i, j, k)) / static_cast<float>(resolution.z);
        } else {
            // Central Difference
            norm.z = (volume.GetVoxelVal(i, j, k + 1) - volume.GetVoxelVal(i, j, k - 1)) / (2 * static_cast<float>(resolution.z));
        }

        return norm;
    }
}
