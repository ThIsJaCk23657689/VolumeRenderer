#include "GUI/CubicBezier.hpp"

std::vector<float> CubicBezier::m_coefficient;

std::vector<ImVec2> CubicBezier::BezierTable(const std::array<ImVec2, 4>& control_points, const unsigned int& sample_points) {
    std::vector<ImVec2> results;

    if (m_coefficient.size() != (sample_points + 1) * 4) {
        m_coefficient.resize((sample_points + 1) * 4, 0);
        for (unsigned int i = 0; i <= sample_points; i++) {
            float t = i / static_cast<float>(sample_points);

            // Bernstein Polynomial Form
            m_coefficient[i * 4 + 0] = (1 - t) * (1 - t) * (1 - t);
            m_coefficient[i * 4 + 1] = 3 * (1 - t) * (1 - t) * t;
            m_coefficient[i * 4 + 2] = 3 * (1 - t) * t * t;
            m_coefficient[i * 4 + 3] = t * t * t;
        }
    }

    auto& K = m_coefficient;
    auto& P = control_points;
    for (unsigned int i = 0; i <= sample_points; i++) {
        ImVec2 point = {
            K[i * 4 + 0] * P[0].x + K[i * 4 + 1] * P[1].x + K[i * 4 + 2] * P[2].x + K[i * 4 + 3] * P[3].x,
            K[i * 4 + 0] * P[0].y + K[i * 4 + 1] * P[1].y + K[i * 4 + 2] * P[2].y + K[i * 4 + 3] * P[3].y
        };
        results.push_back(point);
    }

    return results;
}

float CubicBezier::BezierValue(const float& t, const std::array<ImVec2, 2>& points) {
    float steps = 256;
    std::array<ImVec2, 4> control_pts {{ {1, 2}, points[0], points[1], {1, 1} }};
    std::vector<ImVec2> results = BezierTable(control_pts, steps);
    return results[static_cast<int>(t < 0 ? 0 : (t > 1 ? 1 : t)) * steps].y;
}