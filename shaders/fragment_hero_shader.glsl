#version 100
precision mediump float;

varying vec2 out_texture_coordinates;

uniform float quantity;
uniform float number;
uniform float width;
uniform int direction;
uniform sampler2D texture;

void main()
{
    float new_u;
    if (direction == 1)
    new_u = 1.0 - (width * out_texture_coordinates.x + (quantity - number - 1.0) / quantity);
    else
    new_u = width * out_texture_coordinates.x + number / quantity;


    vec2 new_texture_coordinates = vec2(new_u, out_texture_coordinates.y);
    gl_FragColor = texture2D(texture, new_texture_coordinates);
}