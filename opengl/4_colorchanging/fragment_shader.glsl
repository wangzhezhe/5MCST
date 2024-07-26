#version 330 core
out vec4 FragColor;

// setting this color in opengl cpu program
uniform vec4 ourColor; 

void main()
{
    FragColor = ourColor;
} 
