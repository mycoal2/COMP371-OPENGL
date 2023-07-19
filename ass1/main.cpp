//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices

#include<random>       // RANDOM NUMBER GENERATOR

using namespace glm;


const char* getVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                ""
                "uniform vec3 customColor;"
                "uniform mat4 worldMatrix;"
                "uniform mat4 viewMatrix = mat4(1.0);"
                "uniform mat4 projectionMatrix = mat4(1.0);"
                ""
                "out vec3 vertexColor;"
                "void main()"
                "{"
                "   vertexColor = customColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "}";
}


const char* getFragmentShaderSource()
{
    return
                "#version 330 core\n"
                "in vec3 vertexColor;"
                "out vec4 FragColor;"
                "void main()"
                "{"
                "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
                "}";
}
const char* getFragmentShaderSource2()
{
    return
                "#version 330 core\n"
                "in vec3 vertexColor;"
                "out vec4 FragColor;"
                "void main()"
                "{"
                "   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);"
                "}";
}

int createVertexBufferObject()
{
    // Cube model
    vec3 vertexArray[] = {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3( 0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), // top - yellow
        vec3( 0.5f, 0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        
        vec3( 0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.5f, 0.5f, 0.5f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f)
    };
        
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
     
    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          2*sizeof(vec3),       // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
                          );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          2*sizeof(vec3),
                          (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
                          );
    glEnableVertexAttribArray(1);

    
    return vertexBufferObject;
}


int compileAndLinkShaders(int a)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    const char* tempShaderSource;
    const char* tempFragmentShader;
    if (a == 1) {
        tempShaderSource = getVertexShaderSource();
        tempFragmentShader = getFragmentShaderSource();
    } else if(a == 2) {
        tempShaderSource = getVertexShaderSource();
        tempFragmentShader = getFragmentShaderSource2();
    } else {
        tempShaderSource = getVertexShaderSource();
        tempFragmentShader = getFragmentShaderSource();
    }
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = tempShaderSource;
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = tempFragmentShader;
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

float randomInRange(float lowerBound, float upperBound) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(lowerBound, upperBound);
    return dist(gen);
}


int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(800, 600, "Comp371 - Assignment 1", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // green background
    glClearColor(0.20f, 0.3f, 0.3f, 1.0f);

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders(1);
    // int shaderProgram2 = compileAndLinkShaders(2);
    glUseProgram(shaderProgram);

    bool keyPressed = false;
    bool shift = false;
    int renderingMode = GL_TRIANGLES;

    vec3 cameraPosition(0.0f, 15.0f, 20.0f);
    vec3 cameraLookAt(0.0f, -5.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    float cameraSpeed = 1.0f;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    // Define and upload geometry to the GPU here ...


    float fov = 70.0f;
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(glm::radians(fov),            // field of view in degrees
                                             800.0f / 600.0f,  // aspect ratio
                                             0.01f, 100.0f);   // near and far (near > 0)

    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp ); // up
    
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    
    int vao = createVertexBufferObject();

    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    float modelScale = 1;
    float upperArmRotationXAngle = 0;
    float upperArmRotationYAngle = 0;
    vec3 upperArmPos = vec3(0.0f, 6.0f, 0.0f);
    vec3 lowerArmPosOffset = vec3(5.0f, 6.5f, 0.0f);
    vec3 lowerArmPos = upperArmPos + lowerArmPosOffset;
    vec3 racketHandlePosOffset = vec3(0.0f, 8.0f, 0.0f);
    vec3 racketHandlePos = lowerArmPos + racketHandlePosOffset;
    vec3 racketPosOffset = vec3(0.0f, 8.0f, 0.0f);
    vec3 racketPos = racketHandlePos + racketPosOffset;
    
    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        
        glUseProgram(shaderProgram);

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float tempColor[3] = {0.5f, 0.5f, 0.5f};    // Change Color to Grey
        GLuint colorLocation = glGetUniformLocation(shaderProgram, "customColor");
        glUniform3fv(colorLocation, 1, tempColor);

        // Draw Geometry
        glBindVertexArray(vao);
        // Draw ground
        // mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -2.51f, 0.0f)) * scale(mat4(1.0f), vec3(100.0f, 1.0f, 100.0f));
        // GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        // glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        // glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        
        //  ----------------------- Draw Grid 100x100 ------------------------
        // Change shader Color to Yellow
        tempColor[0] = 0.9f;        // Value for Red
        tempColor[1] = 0.9f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        for(float x = -50; x < 50; x++) {
            // glUseProgram(shaderProgram2);
            mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(x, -0.25f, 0.0f)) * scale(mat4(1.0f), vec3(0.05f, 0.05f, 100.0f));
            GLuint gridMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridXWorldMatrix[0][0]);

            // glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            
            mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, x)) * scale(mat4(1.0f), vec3(100.0f, 0.05f, 0.05f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridZWorldMatrix[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        }


        // -------------------- COORDINATE AXIS ----------------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // THIS IS 1 UNIT 
        // mat4 middleWorldMatrix = translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.5f)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        mat4 middleWorldMatrix = scale(mat4(1.0f), vec3(0.501f, 0.501f, 0.501f));
        GLuint gridMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &middleWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0      
        // -------------------- X AXIS -------------------------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(2.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.5f, 0.5f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridXWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // -------------------- Z AXIS -------------------------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 2.5f))* scale(mat4(1.0f), vec3(0.5f, 0.5f, 5.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridZWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // -------------------- Y AXIS -------------------------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridYWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f)) * scale(mat4(1.0f), vec3(0.5f, 5.0f, 0.5f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridYWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0


        // ------------------ CREATING MODEL -------------------------------------
        // ------------------ UPPER ARM ------------------------------------------
        tempColor[0] = 0.8f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.6f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // vec3 upperArmPos = vec3(10.0f, 5.0f, -20.0f);
        mat4 upperArmWorldMatrix = translate(mat4(1.0f), modelScale * upperArmPos) * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f)) * rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), modelScale * vec3(12.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &upperArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ LOWER ARM ------------------------------------------
        tempColor[0] = 0.7f;        // Value for Red
        tempColor[1] = 0.6f;        // Value for Green
        tempColor[2] = 0.5f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // vec3 lowerArmPos = vec3(upperArmPos.x + 6.0f, upperArmPos.y + 4.0f, upperArmPos.z + 0.0f);
        mat4 lowerArmWorldMatrix = translate(mat4(1.0f), modelScale * (upperArmPos + lowerArmPosOffset)) * translate(mat4(1.0f), -1.0f * lowerArmPosOffset) * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f)) * translate(mat4(1.0f), lowerArmPosOffset) * scale(mat4(1.0f), modelScale * vec3(1.5f, 8.0f, 1.5f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &lowerArmWorldMatrix[0][0]);
         
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET HANDLE  --------------------------------------
        tempColor[0] = 0.4f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // vec3 lowerArmPos = vec3(upperArmPos.x + 6.0f, upperArmPos.y + 4.0f, upperArmPos.z + 0.0f);
        mat4 racketHandleWorldMatrix = translate(mat4(1.0f), modelScale * (lowerArmPos + racketHandlePosOffset)) * translate(mat4(1.0f), -1.0f * lowerArmPosOffset) * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f)) * translate(mat4(1.0f), lowerArmPosOffset) * scale(mat4(1.0f), modelScale * vec3(0.75f, 8.0f, 0.75f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketHandleWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0


        // ------------------ RACKET SURFACE --------------------------------------
        tempColor[0] = 0.4f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // vec3 lowerArmPos = vec3(upperArmPos.x + 6.0f, upperArmPos.y + 4.0f, upperArmPos.z + 0.0f);
        mat4 racketWorldMatrix = translate(mat4(1.0f), modelScale * (racketHandlePos + racketPosOffset)) * translate(mat4(1.0f), -1.0f * lowerArmPosOffset) * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f)) * translate(mat4(1.0f), lowerArmPosOffset) * scale(mat4(1.0f), modelScale * vec3(5.0f, 8.0f, 1.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0


        
         // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();


        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;
        
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;
        
        // Convert to spherical coordinates

        
        // glm::normalize(cameraSideVector);

        float currentCameraSpeed = 20;

        // ------------------------------------------------------------------------------------
        // ------------------------------ Handle inputs ---------------------------------------
        // ------------------------------------------------------------------------------------

        // ------------------------------ REPOSITION MODEL ------------------------------------
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if(!keyPressed) {
                float randomX = randomInRange(-40.0f, 40.0f);
                float randomY = randomInRange(5.0f, 15.0f);
                float randomZ = randomInRange(-40.0f, 40.0f);

                upperArmPos = vec3(randomX, randomY, randomZ);
                lowerArmPos = upperArmPos + lowerArmPosOffset;
                racketHandlePos = lowerArmPos + racketHandlePosOffset;
                racketPos = racketHandlePos + racketPosOffset;
                keyPressed = true;
            }             
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
                keyPressed = false;
        }
        // ------------------------------ MODEL SCALE -----------------------------------------
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {      // SCALE UP
            modelScale += 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {     // SCALE DOWN
            modelScale -= 0.01;
        }
        // ------------------------------ UPDATE MODEL POSITION --------------------------------
        shift = false;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { 
            shift = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {     
            if(shift) {                                         // MOVE MODEL LEFT
                upperArmPos -= vec3(0.1f, 0.0f, 0.0f);
                lowerArmPos = upperArmPos + lowerArmPosOffset;
                racketHandlePos = lowerArmPos + racketHandlePosOffset;
                racketPos = racketHandlePos + racketPosOffset;
            } else {                                            // ROTATE 5 DEGREE COUNTERCLOCKWISE
            upperArmRotationXAngle  += 5.0;
            lowerArmPosOffset = vec3(5.0f, 6.5f, 0.0f);
            lowerArmPos = upperArmPos + lowerArmPosOffset;
            racketHandlePos = lowerArmPos + racketHandlePosOffset;
            racketPos = racketHandlePos + racketPosOffset;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if(shift) {                                         // MOVE MODEL RIGHT
                upperArmPos += vec3(0.1f, 0.0f, 0.0f);
                lowerArmPos = upperArmPos + lowerArmPosOffset;
                racketHandlePos = lowerArmPos + racketHandlePosOffset;
                racketPos = racketHandlePos + racketPosOffset;
            } else {                                            // ROTATE 5 DEGREE CLOCKWISE
            upperArmRotationXAngle  -= 5.0;
            lowerArmPosOffset = vec3(5.0f, 6.5f, 0.0f);
            lowerArmPos = upperArmPos + lowerArmPosOffset;
            racketHandlePos = lowerArmPos + racketHandlePosOffset;
            racketPos = racketHandlePos + racketPosOffset;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if(shift) {                                         // MOVE MODEL UP
                upperArmPos += vec3(0.0f, 0.1f, 0.0f);
                lowerArmPos = upperArmPos + lowerArmPosOffset;
                racketHandlePos = lowerArmPos + racketHandlePosOffset;
                racketPos = racketHandlePos + racketPosOffset;
            } else {
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if(shift) {                                         // MOVE MODEL DOWN
                upperArmPos -= vec3(0.0f, 0.1f, 0.0f);
                lowerArmPos = upperArmPos + lowerArmPosOffset;
                racketHandlePos = lowerArmPos + racketHandlePosOffset;
                racketPos = racketHandlePos + racketPosOffset;
            } else {
            }
        }
        // ------------------------------ CHANGE WORLD ORIENTATION --------------------------------

        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        // if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {      // move camera to the left
        //     cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
        // }
        // if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {     // move camera to the right
        //     cameraPosition += cameraSideVector * currentCameraSpeed * dt;
        // }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {      // move camera back
            cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {        // move camera forward
            cameraPosition += cameraLookAt * currentCameraSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {       // move camera up
            cameraPosition += cameraUp * (currentCameraSpeed/2) * dt;
        }        
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {      // move camera down
            cameraPosition -= cameraUp * (currentCameraSpeed/2) * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) // ZOOM OUT
        {
            upperArmRotationXAngle = 0;
            upperArmRotationYAngle = 0;
            upperArmPos = vec3(10.0f, 5.0f, -20.0f);
            upperArmPos = vec3(10.0f, 5.0f, -20.0f);
            lowerArmPosOffset = vec3(6 * cos(upperArmRotationXAngle ), 4.0f, 6 * sin(upperArmRotationXAngle ));
            lowerArmPos = upperArmPos + lowerArmPosOffset;
            racketHandlePosOffset = vec3(0.0f, 8.0f, 0.0f);
            racketHandlePos = lowerArmPos + racketHandlePosOffset;
            racketPosOffset = vec3(0.0f, 8.0f, 0.0f);
            racketPos = racketHandlePos + racketPosOffset;
        }


        // ------------------- RENDERING MODE ---------------------------------------
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { // RENDER TRIANGLES   
            renderingMode = GL_TRIANGLES;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { // RENDER LINES
            renderingMode = GL_LINES;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { // RENDER POINTS
            renderingMode = GL_POINTS;
        }



        // --------------------- CAMERA PAN AND TILT  ------------------------------
        const float cameraAngularSpeed = 15.0f;
        float theta;
        float phi;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
            if (cameraHorizontalAngle > 360)
            {
                cameraHorizontalAngle -= 360;
            }
            else if (cameraHorizontalAngle < -360)
            {
                cameraHorizontalAngle += 360;
            }
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            cameraVerticalAngle   -= dy * cameraAngularSpeed * dt;
            // Clamp vertical angle to [-85, 85] degrees
            cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        }
        theta = radians(cameraHorizontalAngle);
        phi = radians(cameraVerticalAngle);
        cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));


        // --------------------- ZOOM IN AND ZOOM OUT ------------------------------
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if(dx < 0) {                // ZOOM OUT
                if (fov < 179.0) {
                    fov += 1.0;
                }
            } else if(dx > 0) {         // ZOOM IN
                if (fov > 1.0) {
                    fov -= 1.0;
                }
            }
            projectionMatrix = glm::perspective(glm::radians(fov),            // field of view in degrees
                                             800.0f / 600.0f,  // aspect ratio
                                             0.01f, 100.0f);   // near and far (near > 0)
    
            // GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        }

        mat4 viewMatrix = mat4(1.0);
        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp );
        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    }
    
    // Shutdown GLFW
    glfwTerminate();
    
    return 0;
}
