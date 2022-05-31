#include "GUI/GUI.hpp"

#include <glad/glad.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <glm/gtc/type_ptr.hpp>

#include "State.hpp"

GUI::GUI(SDL_Window* window, SDL_GLContext glContext) :
    WindowHandler(window),
    GLContext(glContext) {
    Create();
}

void GUI::Create() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(WindowHandler, GLContext);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.FrameBorderSize = 1.0f;

    // Loading Font
    const ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("assets/fonts/Fantasque Sans Mono Nerd Font.ttf", 16.0f);
}

void GUI::Destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void GUI::Render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(WindowHandler);
    ImGui::NewFrame();

    if (!state.world->mouse_control) {
        MenuBarRender();
        WindowsRender();
        // ErrorModalRender();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::ProcessEvent(const SDL_Event &event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    const ImGuiIO &io = ImGui::GetIO();
    WantCaptureEvent = io.WantCaptureKeyboard || io.WantCaptureMouse;
}

void GUI::MenuBarRender() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Edit##menubar-edit")) {
            ImGui::MenuItem("Camera Info", nullptr, &Windows.CameraInfo.Visible);
            ImGui::MenuItem("Projection Info", nullptr, &Windows.ProjectionInfo.Visible);
            ImGui::MenuItem("Lightning Info", nullptr, &Windows.LightningInfo.Visible);
            ImGui::MenuItem("Transfer Function", nullptr, &Windows.TransferFunction.Visible);
            ImGui::MenuItem("Cubic Bezier", nullptr, &Windows.CubicBezier.Visible);
            ImGui::MenuItem("Settings", nullptr, &Windows.Settings.Visible);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
#ifndef NDEBUG
            if (ImGui::MenuItem("Dear ImGui Demo", nullptr)) {
                Windows.Demo.Visible = true;
            }
#endif

            if (ImGui::MenuItem("About", nullptr)) {
                Windows.About.Visible = true;
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GUI::WindowsRender() {
    CameraInfoRender();
    ProjectionInfoRender();
    LightningInfoRender();
    SettingsRender();
    AboutRender();
    TransferFunctionRender();
    CubicBezierRender();
#ifndef NDEBUG
    // Demo Window Render
    if (Windows.Demo.Visible) {
        ImGui::ShowDemoWindow(&Windows.Demo.Visible);
    }
#endif
}

void GUI::CameraInfoRender() {
    // Camera Info Window Render
    if (Windows.CameraInfo.Visible) {
        ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);
        ImGui::Begin("Camera Info", &Windows.CameraInfo.Visible, Windows.CameraInfo.WindowFlags);

        if (ImGui::BeginTabBar("TabBar##Window_CameraInfo")) {

            if (ImGui::BeginTabItem("Camera")) {
                ImGui::Text("Position = (%.2f, %.2f, %.2f)",
                            state.world->my_camera->position.x,
                            state.world->my_camera->position.y,
                            state.world->my_camera->position.z);
                ImGui::Text("Velocity = (%.2f, %.2f, %.2f)",
                            state.world->my_camera->velocity.x,
                            state.world->my_camera->velocity.y,
                            state.world->my_camera->velocity.z);
                ImGui::Text("Front = (%.2f, %.2f, %.2f)",
                            state.world->my_camera->front.x,
                            state.world->my_camera->front.y,
                            state.world->my_camera->front.z);
                ImGui::Text("Right = (%.2f, %.2f, %.2f)",
                            state.world->my_camera->right.x,
                            state.world->my_camera->right.y,
                            state.world->my_camera->right.z);
                ImGui::Text("Up = (%.2f, %.2f, %.2f)",
                            state.world->my_camera->up.x,
                            state.world->my_camera->up.y,
                            state.world->my_camera->up.z);
                ImGui::Text("World Up = (%.2f, %.2f, %.2f)",
                            state.world->my_camera->world_up.x,
                            state.world->my_camera->world_up.y,
                            state.world->my_camera->world_up.z);
                ImGui::Text("Pitch = %.2f deg", state.world->my_camera->pitch);
                ImGui::Text("Yaw = %.2f deg", state.world->my_camera->yaw);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}

void GUI::ProjectionInfoRender() {
    // Projection Window Render
    if (Windows.ProjectionInfo.Visible) {
        ImGui::SetNextWindowSize(ImVec2(450, 180), ImGuiCond_Once);
        ImGui::Begin("Projection Info", &Windows.ProjectionInfo.Visible, Windows.ProjectionInfo.WindowFlags);
        if (ImGui::BeginTabBar("TabBar##Window_ProjectionInfo")) {

            if (ImGui::BeginTabItem("Projection")) {
                ImGui::Text("Parameters:");
                ImGui::BulletText("FoV = %.2f deg, Aspect = %.2f", state.world->my_camera->zoom, state.world->my_camera->AspectRatio());
                ImGui::DragFloatRange2("Near / Far", &state.world->my_camera->frustum.near, &state.world->my_camera->frustum.far, 1.0f, 0.1f, 500.0f);
                ImGui::Spacing();

                if (ImGui::TreeNode("Projection Matrix")) {
                    glm::mat4 proj = state.world->my_camera->Projection();

                    ImGui::Columns(4, "mycolumns");
                    ImGui::Separator();
                    for (int i = 0; i < 4; i++) {
                        ImGui::Text("%.2f", proj[0][i]); ImGui::NextColumn();
                        ImGui::Text("%.2f", proj[1][i]); ImGui::NextColumn();
                        ImGui::Text("%.2f", proj[2][i]); ImGui::NextColumn();
                        ImGui::Text("%.2f", proj[3][i]); ImGui::NextColumn();
                        ImGui::Separator();
                    }
                    ImGui::Columns(1);

                    ImGui::TreePop();
                }
                ImGui::Spacing();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("ViewVolume")) {
                glm::vec4 temp_lbn = state.world->my_camera->near_plane_vertex[0];
                glm::vec4 temp_rbn = state.world->my_camera->near_plane_vertex[1];
                glm::vec4 temp_rtn = state.world->my_camera->near_plane_vertex[2];
                glm::vec4 temp_ltn = state.world->my_camera->near_plane_vertex[3];
                glm::vec4 temp_lbf = state.world->my_camera->far_plane_vertex[0];
                glm::vec4 temp_rbf = state.world->my_camera->far_plane_vertex[1];
                glm::vec4 temp_rtf = state.world->my_camera->far_plane_vertex[2];
                glm::vec4 temp_ltf = state.world->my_camera->far_plane_vertex[3];

                if (ImGui::TreeNode("View Volume Vertices (World Coordinate)")) {
                    ImGui::BulletText("left-bottom-near: (%.2f, %.2f, %.2f, %.2f)", temp_lbn.x, temp_lbn.y, temp_lbn.z, temp_lbn.w);
                    ImGui::BulletText("right-bottom-near: (%.2f, %.2f, %.2f, %.2f)", temp_rbn.x, temp_rbn.y, temp_rbn.z, temp_rbn.w);
                    ImGui::BulletText("right-top-near: (%.2f, %.2f, %.2f, %.2f)", temp_rtn.x, temp_rtn.y, temp_rtn.z, temp_rtn.w);
                    ImGui::BulletText("left-top-near: (%.2f, %.2f, %.2f, %.2f)", temp_ltn.x, temp_ltn.y, temp_ltn.z, temp_ltn.w);
                    ImGui::BulletText("left-bottom-far: (%.2f, %.2f, %.2f, %.2f)", temp_lbf.x, temp_lbf.y, temp_lbf.z, temp_lbf.w);
                    ImGui::BulletText("right-bottom-far: (%.2f, %.2f, %.2f, %.2f)", temp_rbf.x, temp_rbf.y, temp_rbf.z, temp_rbf.w);
                    ImGui::BulletText("right-top-far: (%.2f, %.2f, %.2f, %.2f)", temp_rtf.x, temp_rtf.y, temp_rtf.z, temp_rtf.w);
                    ImGui::BulletText("left-top-far: (%.2f, %.2f, %.2f, %.2f)", temp_ltf.x, temp_ltf.y, temp_ltf.z, temp_ltf.w);
                    ImGui::TreePop();
                }
                ImGui::Spacing();

                glm::mat4 view = state.world->my_camera->View();
                temp_lbn = view * temp_lbn;
                temp_rbn = view * temp_rbn;
                temp_rtn = view * temp_rtn;
                temp_ltn = view * temp_ltn;
                temp_lbf = view * temp_lbf;
                temp_rbf = view * temp_rbf;
                temp_rtf = view * temp_rtf;
                temp_ltf = view * temp_ltf;

                if (ImGui::TreeNode("View Volume Vertices (View Coordinate)")) {
                    ImGui::BulletText("left-bottom-near: (%.2f, %.2f, %.2f, %.2f)", temp_lbn.x, temp_lbn.y, temp_lbn.z, temp_lbn.w);
                    ImGui::BulletText("right-bottom-near: (%.2f, %.2f, %.2f, %.2f)", temp_rbn.x, temp_rbn.y, temp_rbn.z, temp_rbn.w);
                    ImGui::BulletText("right-top-near: (%.2f, %.2f, %.2f, %.2f)", temp_rtn.x, temp_rtn.y, temp_rtn.z, temp_rtn.w);
                    ImGui::BulletText("left-top-near: (%.2f, %.2f, %.2f, %.2f)", temp_ltn.x, temp_ltn.y, temp_ltn.z, temp_ltn.w);
                    ImGui::BulletText("left-bottom-far: (%.2f, %.2f, %.2f, %.2f)", temp_lbf.x, temp_lbf.y, temp_lbf.z, temp_lbf.w);
                    ImGui::BulletText("right-bottom-far: (%.2f, %.2f, %.2f, %.2f)", temp_rbf.x, temp_rbf.y, temp_rbf.z, temp_rbf.w);
                    ImGui::BulletText("right-top-far: (%.2f, %.2f, %.2f, %.2f)", temp_rtf.x, temp_rtf.y, temp_rtf.z, temp_rtf.w);
                    ImGui::BulletText("left-top-far: (%.2f, %.2f, %.2f, %.2f)", temp_ltf.x, temp_ltf.y, temp_ltf.z, temp_ltf.w);
                    ImGui::TreePop();
                }
                ImGui::Spacing();

                glm::mat4 proj = state.world->my_camera->Projection();
                temp_lbn = proj * temp_lbn;
                temp_rbn = proj * temp_rbn;
                temp_rtn = proj * temp_rtn;
                temp_ltn = proj * temp_ltn;
                temp_lbf = proj * temp_lbf;
                temp_rbf = proj * temp_rbf;
                temp_rtf = proj * temp_rtf;
                temp_ltf = proj * temp_ltf;

                if (ImGui::TreeNode("View Volume Vertices (Projection Coordinate)")) {
                    ImGui::BulletText("left-bottom-near: (%.2f, %.2f, %.2f, %.2f)", temp_lbn.x, temp_lbn.y, temp_lbn.z, temp_lbn.w);
                    ImGui::BulletText("right-bottom-near: (%.2f, %.2f, %.2f, %.2f)", temp_rbn.x, temp_rbn.y, temp_rbn.z, temp_rbn.w);
                    ImGui::BulletText("right-top-near: (%.2f, %.2f, %.2f, %.2f)", temp_rtn.x, temp_rtn.y, temp_rtn.z, temp_rtn.w);
                    ImGui::BulletText("left-top-near: (%.2f, %.2f, %.2f, %.2f)", temp_ltn.x, temp_ltn.y, temp_ltn.z, temp_ltn.w);
                    ImGui::BulletText("left-bottom-far: (%.2f, %.2f, %.2f, %.2f)", temp_lbf.x, temp_lbf.y, temp_lbf.z, temp_lbf.w);
                    ImGui::BulletText("right-bottom-far: (%.2f, %.2f, %.2f, %.2f)", temp_rbf.x, temp_rbf.y, temp_rbf.z, temp_rbf.w);
                    ImGui::BulletText("right-top-far: (%.2f, %.2f, %.2f, %.2f)", temp_rtf.x, temp_rtf.y, temp_rtf.z, temp_rtf.w);
                    ImGui::BulletText("left-top-far: (%.2f, %.2f, %.2f, %.2f)", temp_ltf.x, temp_ltf.y, temp_ltf.z, temp_ltf.w);
                    ImGui::TreePop();
                }
                ImGui::Spacing();

                if (ImGui::TreeNode("View Volume Vertices (Normalized Device Coordinate)")) {
                    ImGui::BulletText("left-bottom-near: (%.2f, %.2f, %.2f)", temp_lbn.x / temp_lbn.w, temp_lbn.y / temp_lbn.w, temp_lbn.z / temp_lbn.w);
                    ImGui::BulletText("right-bottom-near: (%.2f, %.2f, %.2f)", temp_rbn.x / temp_rbn.w, temp_rbn.y / temp_rbn.w, temp_rbn.z / temp_rbn.w);
                    ImGui::BulletText("right-top-near: (%.2f, %.2f, %.2f)", temp_rtn.x / temp_rtn.w, temp_rtn.y / temp_rtn.w, temp_rtn.z / temp_rtn.w);
                    ImGui::BulletText("left-top-near: (%.2f, %.2f, %.2f)", temp_ltn.x / temp_ltn.w, temp_ltn.y / temp_ltn.w, temp_ltn.z / temp_ltn.w);
                    ImGui::BulletText("left-bottom-far: (%.2f, %.2f, %.2f)", temp_lbf.x / temp_lbf.w, temp_lbf.y / temp_lbf.w, temp_lbf.z / temp_lbf.w);
                    ImGui::BulletText("right-bottom-far: (%.2f, %.2f, %.2f)", temp_rbf.x / temp_lbf.w, temp_rbf.y / temp_lbf.w, temp_rbf.z / temp_lbf.w);
                    ImGui::BulletText("right-top-far: (%.2f, %.2f, %.2f)", temp_rtf.x / temp_lbf.w, temp_rtf.y / temp_lbf.w, temp_rtf.z / temp_lbf.w);
                    ImGui::BulletText("left-top-far: (%.2f, %.2f, %.2f)", temp_ltf.x / temp_lbf.w, temp_ltf.y / temp_lbf.w, temp_ltf.z / temp_lbf.w);
                    ImGui::TreePop();
                }
                ImGui::Spacing();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}

void GUI::LightningInfoRender() {
    // Lightning Window Render
    if (Windows.LightningInfo.Visible) {
        ImGui::SetNextWindowSize(ImVec2(400, 180), ImGuiCond_Once);
        ImGui::Begin("Lightning Info", &Windows.LightningInfo.Visible, Windows.LightningInfo.WindowFlags);
        ImGui::Spacing();
        if (ImGui::BeginTabBar("TabBar##Window_LightningInfo")) {

            if (ImGui::BeginTabItem("Point Lights"))  {
                if (ImGui::TreeNode(std::string("Point Light").c_str())) {
                    if (ImGui::ColorEdit3("Color", glm::value_ptr(state.world->my_point_light->color))) {
                        state.world->my_point_light->UpdateColor();
                    }
                    ImGui::TreePop();
                }
                ImGui::Spacing();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Bloom"))  {
                ImGui::Checkbox("Enable", &state.world->use_bloom);
                ImGui::SliderFloat("Intensity", &state.world->bloom_intensity, 0.0f, 2.0f);
                ImGui::SliderFloat("Threshold", &state.world->bloom_threshold, 0.5f, 5.0f);
                ImGui::SliderInt("Strength", &state.world->bloom_strength, 0.0f, 50.0f);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}

void GUI::TransferFunctionRender() {
    if (Windows.TransferFunction.Visible) {
        ImGui::SetNextWindowSize(ImVec2(500, 690), ImGuiCond_Once);
        ImGui::Begin("Volume Settings", &Windows.TransferFunction.Visible, Windows.TransferFunction.WindowFlags);

        // Show FPS
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        // File Loader
        ImGui::InputText("Volume Data Folder", state.world->volume_data_folder_path, IM_ARRAYSIZE(state.world->volume_data_folder_path));
        if (ImGui::Button("Reload Folder")) {
            state.world->ScanVolumeDataFolder();

            if (state.world->volume_data_files.empty()) {
                ImGui::OpenPopup("Error##NoVolumeFiles");
            }
        }

        ErrorNoVolumeFilesModal();

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Select a volume data (.toml) to load.");
        if (ImGui::BeginCombo("Volume Data (toml)", state.world->current_volume_data.c_str())) {
            for (int n = 0; n < state.world->volume_data_files.size(); n++) {
                bool is_selected = (state.world->current_volume_data == state.world->volume_data_files[n]);
                if (ImGui::Selectable(state.world->volume_data_files[n].c_str(), is_selected)) {
                    state.world->current_volume_data = state.world->volume_data_files[n];
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Load Volume Data")) {
            if (state.world->current_volume_data == "Please select a volume file (.toml)") {
                ImGui::OpenPopup("Error##NoChoseVolumeFile");
            } else {
                state.world->my_volume = std::make_unique<Volume>(state.world->current_volume_data);
                state.world->my_volume->GenerateTFTexture(m_transfer_function);
            }
        }

        ErrorNoChoseVolumeFileModal();

        ImGui::Spacing();
        ImGui::Separator();

        // Volume Rendering Setting (Ray Casting)
        if (state.world->my_volume) {
            ImGui::SliderFloat("Camera Distance", &state.world->my_camera->distance, 400.0f, 1200.0f);
            ImGui::SliderFloat("Sample rate", &state.world->sample_rate, 0.1f, 1.0f);
            ImGui::Checkbox("Normal Color", &state.world->use_normal_color);
            ImGui::Checkbox("Lighting", &state.world->use_lighting);
            if (m_transfer_function.DrawUI("Transfer Function", 256)) {
                state.world->my_volume->GenerateTFTexture(m_transfer_function);
            }
        }
        ImGui::Spacing();
        ImGui::End();
    }
}

void GUI::CubicBezierRender() {
    if (Windows.CubicBezier.Visible) {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
        ImGui::Begin("Cubic Bezier", &Windows.CubicBezier.Visible, Windows.CubicBezier.WindowFlags);
        m_cubic_bezier.DrawUI();
        ImGui::End();
    }
}

void GUI::SettingsRender() {
    // Settings Window Render
    if (Windows.Settings.Visible) {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
        ImGui::Begin("Settings", &Windows.Settings.Visible, Windows.Settings.WindowFlags);
        ImGui::Checkbox("Draw Axes ", &state.world->draw_axes);
        ImGui::Checkbox("Back Face Culling", &state.world->culling);
        ImGui::Spacing();
        const char* items_a[] = { "Normal", "Inversion", "Grayscale", "Narcotic", "Blur", "Edge Detection" };
        ImGui::Combo("Screen Mode", reinterpret_cast<int*>(&state.world->current_screen_mode), items_a, IM_ARRAYSIZE(items_a));
        ImGui::Checkbox("Gamma Correction", &state.world->use_gamma_correction);
        if (state.world->use_gamma_correction) {
            ImGui::SliderFloat("Gamma Value", &state.world->gamma_value, 0.1f, 5.0f);
        }
        ImGui::Spacing();
        ImGui::Checkbox("Use HDR", &state.world->use_hdr);
        if (state.world->use_hdr) {
            const char* items_hdr[] = { "Reinhard", "Exposure" };
            ImGui::Combo("HDR Mode", reinterpret_cast<int*>(&state.world->current_hdr_mode), items_hdr, IM_ARRAYSIZE(items_hdr));
            if (state.world->current_hdr_mode == HDRMode::EXPOSURE) {
                ImGui::SliderFloat("HDR Exposure", &state.world->hdr_exposure, 0.1f, 10.0f);
            }
        }
        ImGui::Spacing();
        ImGui::End();
    }
}

void GUI::AboutRender() {
    Windows.About.WindowFlags = ImGuiWindowFlags_NoResize;

    // About Window Render
    if (Windows.About.Visible) {
        ImGui::Begin("About##Window_About", &Windows.About.Visible, Windows.About.WindowFlags);
        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("SDL2 Examples - 06 Cube Map, Normal Map and Bloom");
        ImGui::SetWindowFontScale(1.0f);
        if (ImGui::BeginTabBar("TabBar##Window_About")) {

            if (ImGui::BeginTabItem("About##About")) {
                ImGui::BeginChild("Child##AboutAbout", Windows.About.ChildSize, true);
                ImGui::TextWrapped(
                    "An OpenGL template program for students learning Computer Graphics.\n"
                    "\n"
                    "Developed at:\n"
                    "National Taiwan Ocean University\n"
                    "\n"
                    "Copyright 2022, NTOU CSE 503 Authors\n");
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Components##About")) {
                ImGui::BeginChild("Child##AboutComponents", Windows.About.ChildSize, true);
                ImGui::Text(
                    "SDL2\n"
                    "Version %d.%d.%d\n"
                    "https://www.libsdl.org/\n",
                    SDL_MAJOR_VERSION,
                    SDL_MINOR_VERSION,
                    SDL_PATCHLEVEL);
                ImGui::Text(" ");
                ImGui::Separator();
                ImGui::Text("glad\nVersion 0.1.34\n");
                ImGui::Text(" ");
                ImGui::Separator();
                ImGui::Text("glm\nVersion %d.%d.%d.%d\n", GLM_VERSION_MAJOR, GLM_VERSION_MINOR, GLM_VERSION_PATCH, GLM_VERSION_REVISION);
                ImGui::Text(" ");
                ImGui::Separator();
                ImGui::Text(
                    "Dear ImGui\n"
                    "Version %s\n"
                    "https://github.com/ocornut/imgui\n",
                    IMGUI_VERSION);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Authors##About")) {
                ImGui::BeginChild("Child##AboutAuthors", Windows.About.ChildSize, true);
                ImGui::Text("ThIsJaCk <y23657689@gmail.com>\n");
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}

void GUI::ErrorNoVolumeFilesModal() {
    if (ImGui::BeginPopupModal("Error##NoVolumeFiles", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("There is no any volume data (.toml) files in this folder:\n");
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 0.0f, 1.0f), "%s", state.world->volume_data_folder_path);
        ImGui::Text("Please try another folder path to load.\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}

void GUI::ErrorNoChoseVolumeFileModal() {
    if (ImGui::BeginPopupModal("Error##NoChoseVolumeFile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Please select a folder path and choose a volume data first!\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}