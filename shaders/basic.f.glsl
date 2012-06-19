#version 110

uniform vec4 color;

void main()
{
    // Set the output color of our current pixel
    gl_FragColor = color;
}