#ifndef CUBICBEZIER_HPP
#define CUBICBEZIER_HPP

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <vector>
#include <array>

struct CubicBezier {
    static std::vector<float> m_coefficient;
    static std::vector<ImVec2> BezierTable(const std::array<ImVec2, 4>& control_points, const unsigned int& sample_points);
    static float BezierValue(const float& t, const std::array<ImVec2, 2>& points);

};
#endif
