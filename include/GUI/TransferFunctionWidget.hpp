#ifndef TRANSFERFUNCTIONWIDGET_HPP
#define TRANSFERFUNCTIONWIDGET_HPP

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <array>
#include <vector>
#include <string>
#include <cstddef>

enum class Channel : unsigned int {
    Red = 0, Green = 1, Blue = 2, Alpha = 3
};

struct TransferFunctionWidget {
public:
    TransferFunctionWidget();
    bool DrawUI(const std::string& label, const int& domain);
    std::vector<float> GetColorData() const;

private:
    struct WidgetConfig {
        float scaling = 1.0f;
        float control_line_width = 2.0f * scaling;
        float control_pt_radius = 10.0f * scaling;
        float control_pt_border = 2.0f * scaling;
        ImVec2 margin = {2 * control_pt_radius, 2 * control_pt_radius};
        std::size_t control_pt_count_max = 20;
    } m_config;

    int m_domain;
    bool m_colormap_change;
    std::string m_label;
    std::size_t m_current_channel;
    std::size_t m_current_control_pt;
    std::array<std::vector<ImVec2>, 4> m_control_pts;

    std::vector<float> m_colormap;

    ImVec2 m_canvas_size;
    ImVec2 m_origin;

    bool m_is_handle_captured;

    void InitialControlPoints();
    void DrawCanvas();
    bool HandleEvents();

    inline ImColor ChangeColorAlpha(const ImColor& color, const float& alpha) const;
    inline ImVec2 ScreenCoord(const ImVec2& pos) const;
    inline ImVec2 CanvasCoord(const ImVec2& pos) const;
};

#endif