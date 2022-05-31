#include "GUI/TransferFunctionWidget.hpp"

#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#include <cassert>

// Some helper function
template <typename T>
inline T clamp(T x, T min, T max) {
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }
    return x;
}

// Define the color presets
static const struct {
    const ImColor WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
    const ImColor RED = {1.0f, 0.0f, 0.0f, 1.0f};
    const ImColor GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
    const ImColor BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
    const ImColor GREY = {0.5f, 0.5f, 0.5f, 1.0f};
    const ImColor DARKRED = {0.5f, 0.0f, 0.0f, 1.0f};
    const ImColor DARKGREEN = {0.0f, 0.5f, 0.0f, 1.0f};
    const ImColor DARKBLUE = {0.0f, 0.0f, 0.5f, 1.0f};
    const ImColor DARKGREY = {0.4f, 0.4f, 0.4f, 1.0f};
} IMCOLOR;

static const std::array<ImColor, 4> ControlPointsColor = {
    IMCOLOR.RED, IMCOLOR.GREEN, IMCOLOR.BLUE, IMCOLOR.GREY
};

static const std::array<ImColor, 4> ControlPointsDarkColor = {
    IMCOLOR.DARKRED, IMCOLOR.DARKGREEN, IMCOLOR.DARKBLUE, IMCOLOR.DARKGREY
};

TransferFunctionWidget::TransferFunctionWidget() :
    m_domain(256),
    m_colormap_change(false),
    m_is_handle_captured(false),
    m_current_channel(static_cast<std::size_t>(Channel::Alpha)),
    m_current_control_pt(0) {
    InitialControlPoints();
}

bool TransferFunctionWidget::DrawUI(const std::string& label, const int& domain) {
    m_label = label;
    m_domain = domain;

    ImGui::Text("%s", m_label.c_str());
    ImGui::TextWrapped(
        "Left click to add a point, right click remove."
        "Left click + drag to move points."
    );

    const ImVec2 padding(20.0f, 20.0f);
    const auto& avail = ImGui::GetContentRegionAvail() - padding;
    m_canvas_size = avail;

    // thumbnail for transfer function
    // UpdateThumb();
    // size_t thumb;
    // float thumb_height = 16.0f;
    // ImGui::Image(reinterpret_cast<void*>(thumb), ImVec2(m_canvas_size.x * m_config.scaling, thumb_height));
    // m_canvas_size.y -= thumb_height + 4.0f;

    DrawCanvas();

    return HandleEvents();
}

std::vector<float> TransferFunctionWidget::GetColorData() const {
    std::vector<float> results(m_domain * 4, 0.0f);
    for (std::size_t channel = 0; channel < m_control_pts.size(); channel++) {
        auto current_control_pt = m_control_pts[channel].begin();
        for (size_t i = 0; i < m_domain; i++) {
            float x = static_cast<float>(i) / static_cast<float>(m_domain);
            auto upper = current_control_pt + 1;
            if (x > upper->x) {
                current_control_pt++;
                upper++;
            }

            // Doing lerp
            float t = (x - current_control_pt->x) / (upper->x - current_control_pt->x);
            float value = (1.0f - t) * current_control_pt->y + t * upper->y;
            results[i * 4 + channel] = clamp(value, 0.0f, 1.0f);
        }
    }

    return results;
}

void TransferFunctionWidget::InitialControlPoints() {
    // Initialize these control points on every color channels with (0.0, 0.0) and (1.0, 1.0)
    m_control_pts[static_cast<size_t>(Channel::Red)].emplace_back(0.0f, 0.0f);
    m_control_pts[static_cast<size_t>(Channel::Red)].emplace_back(0.83f, 0.89f);
    m_control_pts[static_cast<size_t>(Channel::Red)].emplace_back(1.0f, 0.2f);

    m_control_pts[static_cast<size_t>(Channel::Green)].emplace_back(0.0f, 0.0f);
    m_control_pts[static_cast<size_t>(Channel::Green)].emplace_back(1.0f, 0.5f);

    m_control_pts[static_cast<size_t>(Channel::Blue)].emplace_back(0.0f, 0.45f);
    m_control_pts[static_cast<size_t>(Channel::Blue)].emplace_back(0.21f, 0.98f);
    m_control_pts[static_cast<size_t>(Channel::Blue)].emplace_back(0.78f, 0.12f);
    m_control_pts[static_cast<size_t>(Channel::Blue)].emplace_back(1.0f, 0.8f);

    m_control_pts[static_cast<size_t>(Channel::Alpha)].emplace_back(0.0f, 0.0f);
    m_control_pts[static_cast<size_t>(Channel::Alpha)].emplace_back(1.0f, 1.0f);
}

void TransferFunctionWidget::DrawCanvas() {
    const auto& style = ImGui::GetStyle();
    auto* draw_list = ImGui::GetWindowDrawList();
    auto* window = ImGui::GetCurrentWindow();

    // Prepare Canvas
    const ImVec2 canvas(m_canvas_size * m_config.scaling);
    m_origin = window->DC.CursorPos + m_config.margin;

    ImRect bb(m_origin, m_origin + canvas - (m_config.margin * 2));
    ImRect hoverable(bb.Min - m_config.margin, bb.Max + m_config.margin);
    draw_list->PushClipRect(hoverable.Min, hoverable.Max);

    ImGui::BeginGroup();
    // draw_list->AddRect(bb.Min, bb.Max, ImColor(180, 180, 180, 255));
    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 0.8), true, style.FrameRounding);
    ImGui::InvisibleButton("tfn_canvas", hoverable.GetSize());

    // Draw control lines but except the active one.
    for (std::size_t i = 0; i < m_control_pts.size(); i++) {
        if (i == m_current_channel) {
            continue;
        }

        const auto& current_pts = m_control_pts[i];

        // Draw lines
        // draw_list->AddPolyline(current_pts.data(), static_cast<int>(current_pts.size()), IMCOLOR.WHITE, false, m_config.control_line_width);
        for (std::size_t p = 0; p < current_pts.size() - 1; p++) {
            draw_list->AddLine(ScreenCoord(current_pts[p]), ScreenCoord(current_pts[p + 1]), ImGui::GetColorU32(ImGuiCol_TextDisabled), m_config.control_line_width);
        }

        // Draw control points
        for (const auto& point : current_pts) {
            const ImVec2 point_ab = ScreenCoord(point);
            draw_list->AddCircleFilled(point_ab, m_config.control_pt_radius, ControlPointsDarkColor[i]);
            draw_list->AddCircleFilled(point_ab, m_config.control_pt_radius - m_config.control_pt_border, ControlPointsDarkColor[i]);
        }
    }

    // Draw tje active one at the end to keep it always on the top layer.
    const std::vector<ImVec2>& active = m_control_pts[m_current_channel];
    for (std::size_t p = 0; p < active.size() - 1; p++) {
        draw_list->AddLine(ScreenCoord(active[p]), ScreenCoord(active[p + 1]), IMCOLOR.WHITE, m_config.control_line_width);
    }
    for (const auto& point : active) {
        const auto active_radius = m_config.control_pt_radius * 1.2f;
        const auto active_color = ChangeColorAlpha(ControlPointsColor[m_current_channel], 0.6f);
        const ImVec2 point_ab = ScreenCoord(point);
        draw_list->AddCircleFilled(point_ab, active_radius, IMCOLOR.WHITE);
        draw_list->AddCircleFilled(point_ab, active_radius - m_config.control_pt_border, active_color);
    }
    ImGui::EndGroup();

    draw_list->PopClipRect();
}

bool TransferFunctionWidget::HandleEvents() {
    m_colormap_change = false;

    // Skip event-handing if the mouse is not even hovering over the canvas.
    if (!ImGui::IsItemHovered() && !m_is_handle_captured) {
        return false;
    }

    const bool is_left_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool is_right_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
    const bool is_left_released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    const bool is_right_released = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
    const bool is_left_dragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
    const bool is_clicked = is_left_clicked || is_right_clicked;
    const bool is_released = is_left_released || is_right_released;

    const ImVec2 mouse_position = ImGui::GetIO().MousePos;

    // This radius is based on distance square
    const float reach_radius = 2 * m_config.control_pt_radius * m_config.control_pt_radius;

    float dist_square_min = std::numeric_limits<float>::max();
    auto near_channel = m_current_channel;
    auto near_control_pt = m_current_control_pt;

    // The active channel has first priority to respond to events;
    std::vector<ImVec2>& active = m_control_pts[m_current_channel];
    for (std::size_t i = 0; i < active.size(); i++) {
        const ImVec2& diff = mouse_position - ScreenCoord(active[i]);
        const float dist_square = diff.x * diff.x + diff.y * diff.y;
        if (dist_square < dist_square_min) {
            dist_square_min = dist_square;
            near_control_pt = i;
        }
    }

    // If the closest control point from the current active channel is reachable.
    if (dist_square_min < reach_radius) {
        ImGui::SetTooltip("(%.0f, %.4f)", static_cast<float>(m_domain) * active[near_control_pt].x, active[near_control_pt].y);
        if (is_clicked) {
            // capture the control point
            m_current_control_pt = near_control_pt;
            m_is_handle_captured = true;
        }
    } else {
        // If none of the closest control point from the active channel is reached,
        // We check the other channels
        for (std::size_t i = 0; i < m_control_pts.size(); i++) {
            if (m_current_channel == i) {
                continue;
            }
            for (std::size_t j = 0; j < m_control_pts[i].size(); j++) {
                const ImVec2& diff = mouse_position - ScreenCoord(m_control_pts[i][j]);
                const float dist_square = diff.x * diff.x + diff.y * diff.y;
                if (dist_square < dist_square_min) {
                    dist_square_min = dist_square;
                    near_channel = i;
                    near_control_pt = j;
                }
            }
        }

        if (dist_square_min < reach_radius) {
            ImGui::SetTooltip("(%.0f, %.4f)",
                              static_cast<float>(m_domain) * m_control_pts[near_channel][near_control_pt].x,
                              m_control_pts[near_channel][near_control_pt].y);
            if (is_clicked) {
                if (near_channel == m_current_channel) {
                    m_current_control_pt = near_control_pt;
                    m_is_handle_captured = true;
                } else {
                    if (is_left_clicked) {
                        m_current_channel = near_channel;
                        m_current_control_pt = near_control_pt;
                        m_is_handle_captured = true;
                    }
                }
            }
        }
    }

    if (is_released) {
        m_is_handle_captured = false;
    }

    // Move, Insert, Erase
    if (m_is_handle_captured) {
        if (is_right_clicked) {
            if (m_current_control_pt != 0 && m_current_control_pt != active.size() - 1) {
                active.erase(active.begin() + m_current_control_pt);
                m_is_handle_captured = false;
                m_colormap_change = true;
            }
        } else if (is_left_dragging) {
            ImVec2 pos_next = CanvasCoord(mouse_position);
            float x_max = 1.0f;
            float x_min = 0.0f;

            if (m_current_control_pt > 0) {
                x_min = active[m_current_control_pt - 1].x;
            }
            if (m_current_control_pt < active.size() - 1) {
                x_max = active[m_current_control_pt + 1].x;
            }

            pos_next.x = clamp(pos_next.x, x_min, x_max);
            pos_next.y = clamp(pos_next.y, 0.0f, 1.0f);

            if (m_current_control_pt == 0) {
                pos_next.x = 0;
            }
            if (m_current_control_pt == active.size() - 1) {
                pos_next.x = 1;
            }

            active[m_current_control_pt] = pos_next;
            m_colormap_change = true;
        }
    } else if (is_right_clicked && active.size() < m_config.control_pt_count_max) {
        const auto& mouse_pos = CanvasCoord(mouse_position);
        if (mouse_pos.x > 0.0f && mouse_pos.x < 1.0f && mouse_pos.y > 0.0f && mouse_pos.y < 1.0f) {
            active.emplace_back(mouse_pos);
            std::sort(active.begin(), active.end(), [](const auto& a, const auto& b) { return a.x < b.x; });
        }
        m_colormap_change = true;
    }

    return m_colormap_change;
}

ImColor TransferFunctionWidget::ChangeColorAlpha(const ImColor &color, const float &alpha) const {
    assert(alpha >= 0.0f && alpha <= 1.0f);
    return ImColor { color.Value.x, color.Value.y, color.Value.z, alpha };
}

/**
 * A helper function that maps canvas coordinates to screen coordinates
 *
 * 專門把在 canvas 上的座標位置轉換成螢幕上的絕對位置，注意這邊的 m_canvas_size 必須要乘上 scaling
 *
 * @param pos Position in canvas coordinates
 * @return The corresponding positon in screen coordinates
 */
inline ImVec2 TransferFunctionWidget::ScreenCoord(const ImVec2 &pos) const {
    const auto& scaling = m_config.scaling;
    const auto& canvas_size = m_canvas_size * scaling - (m_config.margin * 2);
    return ImVec2(pos.x, 1 - pos.y) * canvas_size + m_origin;
}


/**
 * A helper function that maps screen coordinates to canvas coordinates
 *
 * 專門把螢幕上的絕對座標位置轉換成在 canvas 內部的座標位置，其範圍將會是 0 ~ 1之間
 * 注意這邊的 m_canvas_size 必須要乘上 scaling；其原點在左下角
 *
 * @param pos Position in canvas coordinates
 * @return The corresponding positon in screen coordinates
 */
inline ImVec2 TransferFunctionWidget::CanvasCoord(const ImVec2 &pos) const {
    const auto& scaling = m_config.scaling;
    const auto& canvas_size = m_canvas_size * scaling - (m_config.margin * 2);
    ImVec2 temp = (pos - m_origin) / canvas_size;
    return {temp.x, 1 - temp.y};
}