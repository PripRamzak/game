#version 100
precision mediump float;

varying vec2 frag_uv;

uniform int direction;
uniform sampler2D texture;

void main()
{
    if (direction == 1)
    gl_FragColor = texture2D(texture, vec2(1.0 - frag_uv.x, frag_uv.y));
    else
    gl_FragColor = texture2D(texture, frag_uv);
}