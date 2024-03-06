#version 100
precision mediump float;

varying vec2 out_texture_coordinates;

uniform float quantity;
uniform float number;
uniform sampler2D texture;

void main()
{
    float new_u = (1.0 / quantity) * out_texture_coordinates.x + number / quantity;
    vec2 new_texture_coordinates = vec2(new_u, out_texture_coordinates.y);

    gl_FragColor = texture2D(texture, new_texture_coordinates);
}