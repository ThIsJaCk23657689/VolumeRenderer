#ifndef GUI_HPP
#define GUI_HPP

#include <SDL.h>
#include <imgui.h>
#include <string>
#include "GUI/CubicBezierWidget.hpp"
#include "GUI/TransferFunctionWidget.hpp"

struct GUI {
    GUI(SDL_Window* window, SDL_GLContext glContext);

    SDL_Window* WindowHandler;
    SDL_GLContext GLContext;
    bool WantCaptureEvent = false;

    CubicBezierWidget m_cubic_bezier;
    TransferFunctionWidget m_transfer_function;

    struct Windows {
        struct CameraInfo {
            bool Visible = false;
            int WindowFlags = 0;
        } CameraInfo;
        struct ProjectionInfo {
            bool Visible = false;
            int WindowFlags = 0;
        } ProjectionInfo;
        struct LightningInfo {
            bool Visible = false;
            int WindowFlags = 0;
        } LightningInfo;
        struct Settings {
            bool Visible = false;
            int WindowFlags = 0;
        } Settings;
        struct About {
            bool Visible = false;
            int WindowFlags = 0;
            ImVec2 ChildSize = ImVec2(350, 300);
        } About;
        struct TransferFunction {
            bool Visible = true;
            int WindowFlags = 0;
        } TransferFunction;
        struct CubicBezier {
            bool Visible = false;
            int WindowFlags = 0;
        } CubicBezier;
#ifndef NDEBUG
        struct Demo {
            bool Visible = false;
        } Demo;
#endif
    } Windows;

    void Create();
    void Destroy();
    void Render();
    void ProcessEvent(const SDL_Event &event);

private:
    void MenuBarRender();
    void WindowsRender();

    void CameraInfoRender();
    void ProjectionInfoRender();
    void LightningInfoRender();
    void SettingsRender();
    void AboutRender();
    void TransferFunctionRender();
    void CubicBezierRender();

    void ErrorNoVolumeFilesModal();
    void ErrorNoChoseVolumeFileModal();
};

#endif