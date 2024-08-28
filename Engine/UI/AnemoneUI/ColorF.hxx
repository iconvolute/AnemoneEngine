#pragma once
#include <cstdint>

namespace Anemone::UI
{
    struct Color
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;

        static constexpr Color Create(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            return Color{
                .R = r,
                .G = g,
                .B = b,
                .A = a,
            };
        }

        static constexpr Color Create(uint8_t r, uint8_t g, uint8_t b)
        {
            return Color{
                .R = r,
                .G = g,
                .B = b,
                .A = 255,
            };
        }
    };

    struct ColorF
    {
        float R;
        float G;
        float B;
        float A;

        static constexpr ColorF Create(float r, float g, float b, float a)
        {
            return ColorF{
                .R = r,
                .G = g,
                .B = b,
                .A = a,
            };
        }

        static constexpr ColorF Create(float r, float g, float b)
        {
            return ColorF{
                .R = r,
                .G = g,
                .B = b,
                .A = 1.0f,
            };
        }
    };

}
