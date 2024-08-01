#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 transform;

void main()
{
    //`in` represent the input of vertex, which is a vec3, 
    //gl position is the position of points on the screen
    //the forth element in vector is used for perspective division
    //which is related to the projection from 3d to 2d plane
    gl_Position = transform*vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
