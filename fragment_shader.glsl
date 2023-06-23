#version 100
precision mediump float;

varying vec2 v_t_coord;

uniform float quantity;
uniform float number;
uniform int direction;
uniform sampler2D texture;

void main()
{
    float new_u;
    new_u = (v_t_coord.x + number) / quantity;
    if (direction == 1)
    new_u = 1.0 - new_u;
    float new_v = v_t_coord.y;
    vec2 coord = vec2(new_u, new_v);
    gl_FragColor = texture2D(texture, coord);
}