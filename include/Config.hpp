#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

struct Config {
    // Window Settings
    bool is_fullscreen = false;
    int width = 1024;
    int height = 720;
    std::string title = "Volume Renderer";

    // OpenGL Settings
    int opengl_major_version = 3;
    int opengl_minor_version = 3;
};

#endif