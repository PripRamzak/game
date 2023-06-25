#version 100
precision mediump float;

varying vec2 out_texture_coordinates;

uniform float quantity;
uniform float number;
uniform int direction;
uniform sampler2D texture;

void main()
{
    float new_u;
    float new_v = out_texture_coordinates.y;
    new_u = (out_texture_coordinates.x + number) / quantity;

    if (direction == 1)
    new_u = 1.0 - new_u;

    vec2 new_texture_coordinates = vec2(new_u, new_v);
    gl_FragColor = texture2D(texture, new_texture_coordinates);
}