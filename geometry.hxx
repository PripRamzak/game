#pragma once

#include <iostream>

struct vertex_2d
{
    float x = 0.f;
    float y = 0.f;
    float u = 0.f;
    float v = 0.f;
};

std::istream& operator>>(std::istream& is, vertex_2d& vertex);

struct triangle_2d
{
    vertex_2d vertices[3];

    triangle_2d()
    {
        vertices[0] = { 0.f, 0.f, 0.f, 0.f };
        vertices[0] = { 0.f, 0.f, 0.f, 0.f };
        vertices[0] = { 0.f, 0.f, 0.f, 0.f };
    };

    triangle_2d(vertex_2d v1, vertex_2d v2, vertex_2d v3)
    {
        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
    }
};

std::istream& operator>>(std::istream& is, triangle_2d& triangle);