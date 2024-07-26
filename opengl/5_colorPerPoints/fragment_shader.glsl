#version 330 core

out vec4 FragColor;  

//ourColor is the output of vertex shader
//for pixel not on vertex, we use linear interpolation in default
//if we want to set the qualifer
//we need to add it for both output of the vertex shader and the input of the fragment shader
in vec3 ourColor; 

void main()
{
    //default pattern is linear interpolation
    FragColor = vec4(ourColor, 1.0);
}