#version 100
precision mediump float;

varying vec2 v_t_coord;
uniform float quantity;
uniform float number;
uniform sampler2D texture;

void main()
{
    float new_x = (v_t_coord.x + number) / quantity;
    float new_y = v_t_coord.y;
    vec2 coord = vec2(new_x, new_y);
    gl_FragColor = texture2D(texture, coord);
}