#ifndef CUBICBEZIERWIDGET_HPP
#define CUBICBEZIERWIDGET_HPP

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <chrono>
#include <array>
#include <vector>
#include <string>

#include "CubicBezierPresets.hpp"

struct CubicBezierWidget {

    CubicBezierWidget();
    void DrawUI();

    struct WidgetConfig {
        int smoothness = 64;
        int curve_width = 4;
        int line_width = 1;
        int grab_radius = 8;
        int grab_border = 2;
        int area_width = 128;
        bool area_constrained = true;
    } m_widget_config;

    bool m_reload;
    std::string m_label;
    std::array<float, 5> m_points;

private:
    void DrawPresetSelector();
    bool DrawCanvas();
};

#endif
