#version 320 es

precision mediump float;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT
{
    int tile_id;
}
gs_in[];

out vec2 uv;

uniform mat4 projection;

uniform int tile_size;
uniform ivec2 texture_size;

void main()
{
    int tilemap_width = texture_size.x / 16;

    vec2 tile_pos;
    tile_pos.x = float(gs_in[0].tile_id % tilemap_width) * 16.f;
    tile_pos.y =
        float(texture_size.y) - float(gs_in[0].tile_id / tilemap_width) * 16.f;

    gl_Position = projection * gl_in[0].gl_Position;
    uv          = tile_pos / vec2(texture_size);
    EmitVertex();

    gl_Position =
        projection * (gl_in[0].gl_Position + vec4(tile_size, 0.f, 0.f, 0.f));
    uv = (tile_pos + vec2(16.f, 0.f)) / vec2(texture_size);
    EmitVertex();

    gl_Position =
        projection * (gl_in[0].gl_Position + vec4(0.f, tile_size, 0.f, 0.f));
    uv = (tile_pos + vec2(0.f, -16.f)) / vec2(texture_size);
    EmitVertex();

    gl_Position = projection *
                  (gl_in[0].gl_Position + vec4(tile_size, tile_size, 0.f, 0.f));
    uv = (tile_pos + vec2(16.f, -16.f)) / vec2(texture_size);
    EmitVertex();
}