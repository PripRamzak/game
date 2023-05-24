#version 320 es
precision mediump float;

in vec2 v_t_coord;
uniform sampler2D texture;
out vec4 fragColor;

void main()
{
    fragColor = texture2D(texture, v_t_coord);
}