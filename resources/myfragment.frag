#version 330 // Specify which version of GLSL we are using.

precision highp float; // Video card drivers require this line to function properly

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.9,0.4,0.3,1.0); //Set colour of each fragment to WHITE
}