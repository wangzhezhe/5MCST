#version 330 core
layout (location = 0) in vec3 aPos;   //attribute 0
layout (location = 1) in vec3 aColor; //attribute 1

out vec3 ourColor; //this variable is set by the cpu program through aColor

void main()
{
    gl_Position = vec4(aPos, 1.0); //do not do position transformation
    ourColor = aColor; //the actual ourColor on GPU is set through the value in vertex buffer
}