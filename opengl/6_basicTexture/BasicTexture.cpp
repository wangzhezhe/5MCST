// follow this link
// https://learnopengl.com/Getting-started/Hello-Triangle

// Several questions here
// 1 The meaning of the projection division
// 2 The relatipnship between VAO and glVertexAttribPointer
// 3 What does the each parameter of glVertexAttribPointer mean, why the first one is 0
//   what is relationship with the local in vertex shader

#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string LoadShaderAsSring(const std::string fileName)
{
    std::string results = "";
    std::string line = "";

    std::ifstream myFile(fileName.c_str());
    if (myFile.fail())
    {
        throw std::runtime_error("Error: shader source file does not exist");
    }
    if (myFile.is_open())
    {
        while (std::getline(myFile, line))
        {
            results += line + "\n";
        }
        myFile.close();
    }
    return results;
}

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "failed to execute glfw init" << std::endl;
        return -1;
    }

    // Attention, there is error on mac to compile shader without adding these four hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello EBO", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // load the shader file and compile it
    std::string vertexShaderSrc = LoadShaderAsSring("../vertex_shader.glsl");
    // create shader object
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // attach shader source to the shader object
    const char *vertexShaderPtr = vertexShaderSrc.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderPtr, NULL);
    glCompileShader(vertexShader);

    // check compile status of the shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // print out msg if compilation is failed
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // load the fragment shader and compile it
    // use similar way to check if compilation is successful
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentShaderSource = LoadShaderAsSring("../fragment_shader.glsl");
    const char *fragmentShaderPtr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderPtr, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // print out msg if compilation is failed
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // linking the compiled vertex shader and fragment shader into one shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // use the linked shader program
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    // we do not need shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // unique verticies, the last two values are texture info
    // Attention! in opengl the 0 of the y axis is usually at the bottom of figure
    // but for the image, the 0 of the y axis is at the top of the image
    // so the output rendered figure is upside down now
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f,1.0f,   //top right
        0.5f, -0.5f, 0.0f, 1.0f,0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,0.0f,0.0f, // bottom left
        -0.5f, 0.5f, 0.0f,0.0f,1.0f   // top left
    };

    // create the vbo for managing vertecies
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    //Attention!
    //the ebo does not run properly if does not bind vao firstly
    //1 bind the Vertex Array Object first, 
    //2 then bind and set vertex buffer(s), 
    //3 and then configure vertex attributes(s).

    // populate info into VAO to explain how to parser the array
    // we need to manually specify which part of vbo goes to which vertex element
    // we first need to bind VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // after binding VAO, any following operation related to
    // glVertexAttribPointer will be applied to binded VAO
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    // the type of buffer is GL_ELEMENT_ARRAY_BUFFER this time
    // EBO only store the index of the vertices
    // we do not need to bind vbo each time, 
    // since there is ptr of ebo associated with vao

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // set the layout of the VAO
    // 3 coordinates and two texture coordinates
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set all kinds of texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Loading texture contents
    // get info for assocaited texture
    int width, height, nrChannels;
    //flip the y axis, make it same with the requirments in opengl
    //without using this flip, the result is upside down
    stbi_set_flip_vertically_on_load(true);
    unsigned char *textureData = stbi_load("../awesome-face.png", &width, &height, &nrChannels, 0);
    std::cout << "texture info " << width << " " << height << " " << nrChannels << std::endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    //free the image
    stbi_image_free(textureData);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // gl api to draw things
        /* Render here */
        // ClearColor can help to set background color
        // when the color buffer is cleared
        // set buffer as the a value specified here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
