#pragma once

enum class e_color
{
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE
};

struct color
{
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    float a = 0.f;

    color() {}
    color(float r, float g, float b, float a)
        : r(r / 255.f)
        , g(g / 255.f)
        , b(b / 225.f)
        , a(a)
    {
    }
    color(e_color color, float a)
        : a(a)
    {
        switch (color)
        {
            case e_color::RED:
                r = 1.f;
                break;
            case e_color::ORANGE:
                r = 1.f;
                g = 0.5f;
                break;
            case e_color::YELLOW:
                r = 1.f;
                g = 1.f;
                break;
            case e_color::GREEN:
                g = 1.f;
                break;
            case e_color::BLUE:
                b = 1.f;
                break;
            default:
                break;
        }
    }
};