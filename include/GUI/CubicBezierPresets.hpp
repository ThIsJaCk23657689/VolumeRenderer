#ifndef CUBICBEZIERPRESETS_HPP
#define CUBICBEZIERPRESETS_HPP

#include <string>
#include <array>
#include <vector>

struct CubicBezierPresets {
    const std::string m_name;
    const std::array<float, 4> m_points;
};

static const std::vector<CubicBezierPresets> presets = {
        {"Linear", 0.000f, 0.000f, 1.000f, 1.000f},

        {"In Sine", 0.470f, 0.000f, 0.745f, 0.715f},
        {"In Quad", 0.550f, 0.085f, 0.680f, 0.530f},
        { "In Cubic", 0.550f, 0.055f, 0.675f, 0.190f },
        { "In Quart", 0.895f, 0.030f, 0.685f, 0.220f },
        { "In Quint", 0.755f, 0.050f, 0.855f, 0.060f },
        { "In Expo", 0.950f, 0.050f, 0.795f, 0.035f },
        { "In Circ", 0.600f, 0.040f, 0.980f, 0.335f },
        { "In Back", 0.600f, -0.28f, 0.735f, 0.045f },

        { "Out Sine", 0.390f, 0.575f, 0.565f, 1.000f },
        { "Out Quad", 0.250f, 0.460f, 0.450f, 0.940f },
        { "Out Cubic", 0.215f, 0.610f, 0.355f, 1.000f },
        { "Out Quart", 0.165f, 0.840f, 0.440f, 1.000f },
        { "Out Quint", 0.230f, 1.000f, 0.320f, 1.000f },
        { "Out Expo", 0.190f, 1.000f, 0.220f, 1.000f },
        { "Out Circ", 0.075f, 0.820f, 0.165f, 1.000f },
        { "Out Back", 0.175f, 0.885f, 0.320f, 1.275f },

        { "InOut Sine", 0.445f, 0.050f, 0.550f, 0.950f },
        { "InOut Quad", 0.455f, 0.030f, 0.515f, 0.955f },
        { "InOut Cubic", 0.645f, 0.045f, 0.355f, 1.000f },
        { "InOut Quart", 0.770f, 0.000f, 0.175f, 1.000f },
        { "InOut Quint", 0.860f, 0.000f, 0.070f, 1.000f },
        { "InOut Expo", 1.000f, 0.000f, 0.000f, 1.000f },
        { "InOut Circ", 0.785f, 0.135f, 0.150f, 0.860f },
        { "InOut Back", 0.680f, -0.55f, 0.265f, 1.550f },
};

#endif

