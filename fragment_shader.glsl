#version 100
precision mediump float;

varying vec2 v_t_coord;
uniform sampler2D texture;
// out vec4 fragColor;

void main()
{
    gl_FragColor = texture2D(texture, v_t_coord);
}