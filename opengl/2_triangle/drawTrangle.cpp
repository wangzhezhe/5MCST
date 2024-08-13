// follow this link
// https://learnopengl.com/Getting-started/Hello-Triangle

// Several questions here
// 1 The meaning of the projection division
// 2 The relatipnship between VAO and glVertexAttribPointer
// 3 What does the each parameter of glVertexAttribPointer mean, why the first one is 0
//   what is relationship with the local in vertex shader

#include <glad/glad.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif

#define NOINTERACTIVE

// https://www.david-amador.com/2012/09/how-to-take-screenshot-in-opengl/
bool save_screenshot(std::string filename, int w, int h)
{
    // This prevents the images getting padded
    // when the width multiplied by 3 is not a multiple of 4
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    int nSize = w * h * 3;
    // First let's create our buffer, 3 channels per Pixel
    char *dataBuffer = (char *)malloc(nSize * sizeof(char));

    if (!dataBuffer)
        return false;

    // Let's fetch them from the backbuffer
    // We request the pixels in GL_BGR format, thanks to Berzeger for the tip
    glReadPixels((GLint)0, (GLint)0,
                 (GLint)w, (GLint)h,
                 GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

    // Now the file creation
    FILE *filePtr = fopen(filename.c_str(), "wb");
    if (!filePtr)
        return false;

    unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char header[6] = {w % 256, w / 256,
                               h % 256, h / 256,
                               24, 0};
    // We write the headers
    fwrite(TGAheader, sizeof(unsigned char), 12, filePtr);
    fwrite(header, sizeof(unsigned char), 6, filePtr);
    // And finally our image data
    fwrite(dataBuffer, sizeof(GLubyte), nSize, filePtr);
    fclose(filePtr);
    free(dataBuffer);

    return true;
}

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

    int width = 640;
    int height = 480;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // setting loader after window!!!
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Something went wrong for gladLoader!\n");
        exit(-1);
    }
    printf("OpenGL load  %d.%d\n", GLVersion.major, GLVersion.minor);

    // -- snip --

    // init the buffer

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
    
    //quering information of the opengl to make sure
    //it is crearted by cpu or gpu, if the vendor is mesa
    //it is created by cpu
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);

    printf("Vendor: %s\n", vendor);
    printf("Renderer: %s\n", renderer);
    printf("Version: %s\n", version);

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

    // normalized device coordinates between -1 to 1
    // Any coordinates that fall outside this range will be discarded/clipped
    // and won't be visible on your screen
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    // create the vbo for managing vertecies
    // it needs three API to complete one memory copy to device\s
    unsigned int VBOID;
    glGenBuffers(1, &VBOID);
    // bind vbo with an array buffer
    // No buffer objects are associated with the returned buffer object names
    // until they are first bound by calling glBindBuffer.
    glBindBuffer(GL_ARRAY_BUFFER, VBOID);
    // copy data into buffer
    // parameters:
    // type of buffer, size of data we want to send, pointer of sending data
    // last one represents how GPU manages the data
    // GL_STATIC_DRAW: the data is set only once and used many times.
    // two other types are GL_STREAM_DRAW and GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // populate info into VAO to explain how to parser the array
    // we need to manually specify which part of vbo goes to which vertex element
    // we first need to bind VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // after binding VAO, any following operation related to
    // glVertexAttribPointer will be applied to binded VAO
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    // set the layout of the VAO
    // it tells opengl how to get data from first attribute (attribute 0) in vertex shader, there may include multiple attributes
    // there are three elements per attributed, sthe type of data is float
    // normalized is false and the size of stride is 3*sizeof(float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // non interactive mode

    // Make the BYTE array, factor of 3 because it's RBG.

    // noninteractive mode, render figure and draw it as tga file
#ifdef NOINTERACTIVE
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    save_screenshot(std::string("./test.tga"), width, height);
#else
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // gl api to draw things
        // Render here
        // ClearColor can help to set background color
        // when the color buffer is cleared
        // set buffer as the a value specified here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events
        glfwPollEvents();
    }
#endif

    glfwTerminate();
    return 0;
}
