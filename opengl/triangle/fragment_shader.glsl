#version 330 core
out vec4 FragColor;

void main()
{
    //four elements in vector is RGBA value
    //where A represents the alpha
    //rgb value is from 0 to 1
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 