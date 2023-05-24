#include "geometry.hxx"

std::istream& operator>>(std::istream& is, vertex_2d& vertex)
{
    is >> vertex.x >> vertex.y >> vertex.u >> vertex.v;
    return is;
}

std::istream& operator>>(std::istream& is, triangle_2d& triangle)
{
    is >> triangle.vertices[0] >> triangle.vertices[1] >> triangle.vertices[2];
    return is;
}