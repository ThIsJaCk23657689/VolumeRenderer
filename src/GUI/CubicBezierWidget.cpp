#include "GUI/CubicBezierWidget.hpp"
#include "GUI/CubicBezier.hpp"
#include <iostream>

CubicBezierWidget::CubicBezierWidget() :
    m_reload(false),
    m_label("easeInExpo"),
    m_points({0.95f, 0.05f, 0.795f, 0.035f}) {
}

void CubicBezierWidget::DrawUI() {
    DrawPresetSelector();
    DrawCanvas();
}

void CubicBezierWidget::DrawPresetSelector() {
    ImGui::PushID(m_label.c_str());

    // Preset Left Arrow
    if (ImGui::ArrowButton("##lt", ImGuiDir_Left)) {
        if (--m_points[4] >= 0) {
            m_reload = true;
        } else {
            m_points[4]++;
        }
    }
    ImGui::SameLine();

    // Preset
    if (ImGui::Button("Presets")) {
        ImGui::OpenPopup("!Presets");
    }
    if (ImGui::BeginPopup("!Presets")) {
        for (int i = 0; i < presets.size(); i++) {
            if (i == 1 || i == 9 || i == 17) {
                ImGui::Separator();
            }
            if (ImGui::MenuItem(presets[i].m_name.c_str(), NULL, m_points[4] == i)) {
                m_points[4] = i;
                m_reload = true;
            }
        }
        ImGui::EndPopup();
    }
    ImGui::SameLine();

    // Preset Right Arrow
    if (ImGui::ArrowButton("##rt", ImGuiDir_Right)) {
        if (++m_points[4] < presets.size()) {
            m_reload = true;
        } else {
            m_points[4]--;
        }
    }
    ImGui::PopID();

    if (m_reload) {
        memcpy(m_points.data(), presets[static_cast<std::size_t>(m_points[4])].m_points.data(), sizeof(float) * 4);
    }
}

bool CubicBezierWidget::DrawCanvas() {
    const auto& style = ImGui::GetStyle();
    const auto& io = ImGui::GetIO();
    auto* DrawList = ImGui::GetWindowDrawList();
    auto* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return false;
    }

    // Header and Spacing
    int changed = ImGui::SliderFloat4(m_label.c_str(), m_points.data(), 0, 1, "%.3f", 1.0f);
    int hovered = ImGui::IsItemActive() || ImGui::IsItemHovered();
    ImGui::Dummy(ImVec2(0, 3));

    // Prepare Canvas
    const float avail = ImGui::GetContentRegionAvail().x;
    const float dim = m_widget_config.area_width > 0 ? static_cast<float>(m_widget_config.area_width) : avail;
    ImVec2 canvas(dim, dim);

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + canvas);
//    std::cout << "bb.Min: (" << window->DC.CursorPos.x << ", " << window->DC.CursorPos.y << ")" << std::endl;
//    std::cout << "bb.Max: (" << window->DC.CursorPos.x + canvas.x << ", " << window->DC.CursorPos.y + canvas.y << ")" << std::endl;
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0)) {
        return changed;
    }

    const ImGuiID id = window->GetID(m_label.c_str());
    hovered |= 0 != ImGui::ItemHoverable(ImRect(bb.Min, bb.Min + ImVec2(avail, dim)), id);

    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);

    // Background Grid
    for (int i = 0; i <= canvas.x; i += static_cast<int>(canvas.x / 4)) {
        DrawList->AddLine(
            ImVec2(bb.Min.x + i, bb.Min.y),
            ImVec2(bb.Min.x + i, bb.Max.y),
            ImGui::GetColorU32(ImGuiCol_TextDisabled));
    }
    for (int i = 0; i <= canvas.y; i += static_cast<int>(canvas.y / 4)) {
        DrawList->AddLine(
            ImVec2(bb.Min.x, bb.Min.y + i),
            ImVec2(bb.Max.x, bb.Min.y + i),
            ImGui::GetColorU32(ImGuiCol_TextDisabled));
    }

    // Evaluate Curve
    std::array<ImVec2, 4> control_pts = {{{0, 0}, {m_points[0], m_points[1]}, {m_points[2], m_points[3]}, {1, 1}}};
    std::vector<ImVec2> results = CubicBezier::BezierTable(control_pts, static_cast<int>(m_widget_config.smoothness));

    // Control Points: 2 Lines and 2 Circles
    ImVec2 mouse = ImGui::GetIO().MousePos;
    ImVec2 pos[2];
    float distance[2];

    for (int i = 0; i < 2; i++) {
        pos[i] = ImVec2(m_points[i * 2 + 0], 1 - m_points[i * 2 + 1]) * (bb.Max - bb.Min) + bb.Min;
        distance[i] = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) + (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
    }

    int selected = distance[0] < distance[1] ? 0 : 1;
    if (distance[selected] < static_cast<float>(4 * m_widget_config.grab_radius * 4 * m_widget_config.grab_radius)) {
        ImGui::SetTooltip("(%4.3f, %4.3f)", m_points[selected * 2 + 0], m_points[selected * 2 + 1]);

        if ((ImGui::IsMouseClicked(0) || ImGui::IsMouseDragging(0))) {
            float& px = m_points[selected * 2 + 0] += ImGui::GetIO().MouseDelta.x / canvas.x;
            float& py = m_points[selected * 2 + 1] -= ImGui::GetIO().MouseDelta.y / canvas.y;

            // Clipping
            if (m_widget_config.area_constrained) {
                px = px < 0 ? 0 : (px > 1 ? 1 : px);
                py = py < 0 ? 0 : (py > 1 ? 1 : py);
            }

            changed = true;
        }
    }

    // Draw Curve
    ImColor color(style.Colors[ImGuiCol_PlotLines]);
    for (int i = 0; i < m_widget_config.smoothness; i++) {
        ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
        ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
        ImVec2 r = p * (bb.Max - bb.Min) + bb.Min;
        ImVec2 s = q * (bb.Max - bb.Min) + bb.Min;
        DrawList->AddLine(r, s, color, m_widget_config.curve_width);
    }

    // Draw Control Lines and Grabbers
    ImVec4 white(style.Colors[ImGuiCol_Text]);
    float luma = ImGui::IsItemActive() || ImGui::IsItemHovered() ? 0.5f : 1.0f;
    ImVec4 pink(1.0f, 0.0f, 0.75f, luma), cyan(0.0f, 0.75f, 1.0f, luma);
    ImVec2 p1 = ImVec2(m_points[0], 1 - m_points[1]) * (bb.Max - bb.Min) + bb.Min;
    ImVec2 p2 = ImVec2(m_points[2], 1 - m_points[3]) * (bb.Max - bb.Min) + bb.Min;
    DrawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), static_cast<float>(m_widget_config.line_width));
    DrawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), static_cast<float>(m_widget_config.line_width));
    DrawList->AddCircleFilled(p1, static_cast<float>(m_widget_config.grab_radius), ImColor(white));
    DrawList->AddCircleFilled(p1, static_cast<float>(m_widget_config.grab_radius - m_widget_config.grab_border), ImColor(pink));
    DrawList->AddCircleFilled(p2, static_cast<float>(m_widget_config.grab_radius), ImColor(white));
    DrawList->AddCircleFilled(p2, static_cast<float>(m_widget_config.grab_radius - m_widget_config.grab_border), ImColor(cyan));
    return true;
}