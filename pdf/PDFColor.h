#pragma once

namespace PDF {
    struct Color {
        HPDF_REAL r;
        HPDF_REAL g;
        HPDF_REAL b;
    };

    auto constexpr BLACK = Color{.r = 0, .g = 0, .b =  0};
    auto constexpr WHITE = Color{.r = 1, .g = 1, .b =  1};
    auto constexpr RED = Color{.r = 1, .g = 0, .b =  0};
    auto constexpr GREEN = Color{.r = 0, .g = 1, .b =  0};
    auto constexpr BLUE = Color{.r = 0, .g = 0, .b =  1};
    auto constexpr YELLOW = Color{.r = 1, .g = 1, .b =  0};
    auto constexpr CYAN = Color{.r = 0, .g = 1, .b =  1};
    auto constexpr MAGENTA = Color{.r = 1, .g = 0, .b =  1};
    auto constexpr ORANGE = Color{.r = 1, .g = 0.5, .b =  0};
    auto constexpr PINK = Color{.r = 1, .g = 0.75, .b =  0.75};
    auto constexpr PURPLE = Color{.r = 0.5, .g = 0, .b =  0.5};
    auto constexpr GRAY = Color{.r = 0.5, .g = 0.5, .b =  0.5};
    auto constexpr BROWN = Color{.r = 0.5, .g = 0.25, .b =  0};
    auto constexpr LIME = Color{.r = 0.75, .g = 1, .b =  0};
    auto constexpr TEAL = Color{.r = 0, .g = 0.5, .b =  0.5};
    auto constexpr INDIGO = Color{.r = 0.5, .g = 0, .b =  0.5};
    auto constexpr VIOLET = Color{.r = 0.5, .g = 0, .b =  1};

    auto constexpr LIGHT_GRAY = Color{.r = 0.95, .g = 0.95, .b =  0.95};
    auto constexpr LIGHT_BLUE = Color{.r = 0.75, .g = 0.75, .b =  1};
    auto constexpr LIGHT_GREEN = Color{.r = 0.75, .g = 1, .b =  0.75};
    auto constexpr LIGHT_RED = Color{.r = 1, .g = 0.75, .b =  0.75};
    auto constexpr LIGHT_YELLOW = Color{.r = 1, .g = 1, .b =  0.75};
    auto constexpr LIGHT_CYAN = Color{.r = 0.75, .g = 1, .b =  1};
    auto constexpr LIGHT_MAGENTA = Color{.r = 1, .g = 0.75, .b =  1};
    auto constexpr LIGHT_PURPLE = Color{.r = 0.75, .g = 0.75, .b =  1};

    auto constexpr DARK_GRAY = Color{.r = 0.25, .g = 0.25, .b =  0.25};
    auto constexpr DARK_BLUE = Color{.r = 0, .g = 0, .b =  0.5};
    auto constexpr DARK_GREEN = Color{.r = 0, .g = 0.5, .b =  0};
    auto constexpr DARK_RED = Color{.r = 0.5, .g = 0, .b =  0};
    auto constexpr DARK_YELLOW = Color{.r = 0.5, .g = 0.5, .b =  0};
    auto constexpr DARK_CYAN = Color{.r = 0, .g = 0.5, .b =  0.5};
    auto constexpr DARK_MAGENTA = Color{.r = 0.5, .g = 0, .b =  0.5};
}