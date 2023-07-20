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
                          2*sizeof(vec3),      // stride - each vertex contain 2 vec3 (position, color)
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


int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    const char* tempShaderSource;
    const char* tempFragmentShader;

    tempShaderSource = getVertexShaderSource();
    tempFragmentShader = getFragmentShaderSource();

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
// CHARACTER SIZE 3 X 4
vec3 characterHorizontalSize = vec3(1.00f, 1.0f, 3.0f);
vec3 characterVerticalSize = vec3(1.0f, 4.0f, 1.0f);
// INITIAL POSITION
vec3 characterHmovement = vec3(30.0f, 15.0f, 10.0f);
vec3 characterOmovement = vec3(30.0f, 15.0f, -10.0f);
vec3 characterNmovement = vec3(-30.0f, 15.0f, -10.0f);
vec3 characterGmovement = vec3(-30.0f, 15.0f, 10.0f);
// RACKET BELOW CHARACTER       - HANDLE IS THE PARENT
vec3 characterRacketOffset = vec3(0.0f, 10.0f, 0.0f);
vec3 racketHandlePosH = characterHmovement - characterRacketOffset;
vec3 racketHandlePosO = characterOmovement - characterRacketOffset;
vec3 racketHandlePosN = characterNmovement - characterRacketOffset;
vec3 racketHandlePosG = characterGmovement - characterRacketOffset;
// RACKET SURFACE POSITION
vec3 racketPosOffset = vec3(0.0f, 3.0f, 0.0f);
vec3 racketPosH = racketHandlePosH + racketPosOffset;
vec3 racketPosO = racketHandlePosO + racketPosOffset;
vec3 racketPosN = racketHandlePosN + racketPosOffset;
vec3 racketPosG = racketHandlePosG + racketPosOffset;
// LOWER ARM POSITION
vec3 lowArmPosOffset = vec3(0.0f, -2.2f, 0.4f);
vec3 lowArmPosH = racketHandlePosH + lowArmPosOffset;
vec3 lowArmPosO = racketHandlePosO + lowArmPosOffset;
vec3 lowArmPosN = racketHandlePosN + lowArmPosOffset;
vec3 lowArmPosG = racketHandlePosG + lowArmPosOffset;
// UPPER ARM POSITION
vec3 upArmPosOffset = vec3(0.0f, -1.1f, 1.25f);
vec3 upArmPosH = lowArmPosH + upArmPosOffset;
vec3 upArmPosO = lowArmPosO + upArmPosOffset;
vec3 upArmPosN = lowArmPosN + upArmPosOffset;
vec3 upArmPosG = lowArmPosG + upArmPosOffset;

// ROTATION ANGLES
float racketHRotationY = 0;
float racketORotationY = 0;
float racketNRotationY = 180;
float racketGRotationY = 180;
// POINTER TO CHARACTER YOU CONTROL
vec3* currentCharControl = &characterHmovement;
float* currentRacketRotationY = &racketHRotationY;

void update() {
    racketHandlePosH = characterHmovement - characterRacketOffset;
    racketPosH = racketHandlePosH + racketPosOffset;
    racketHandlePosO = characterOmovement - characterRacketOffset;
    racketPosO = racketHandlePosO + racketPosOffset;
    racketHandlePosN = characterNmovement - characterRacketOffset;
    racketPosN = racketHandlePosN + racketPosOffset;
    racketHandlePosG = characterGmovement - characterRacketOffset;
    racketPosG = racketHandlePosG + racketPosOffset;
    lowArmPosH = racketHandlePosH + lowArmPosOffset;
    lowArmPosO = racketHandlePosO + lowArmPosOffset;
    lowArmPosN = racketHandlePosN + lowArmPosOffset;
    lowArmPosG = racketHandlePosG + lowArmPosOffset;
    upArmPosH = lowArmPosH + upArmPosOffset;
    upArmPosO = lowArmPosO + upArmPosOffset;
    upArmPosN = lowArmPosN + upArmPosOffset;
    upArmPosG = lowArmPosG + upArmPosOffset;
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

    // gray background but it should'nt matter because of skybox
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();
    glUseProgram(shaderProgram);

   
    bool keyPressed = false;        // So that key does not get registered more than once
    bool shift = false;
    int renderingMode = GL_TRIANGLES;

    vec3 cameraPosition(0.0f, 25.0f, 45.0f);
    vec3 cameraLookAt(0.0f, 0.0f, 00.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    float cameraSpeed = 1.0f;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = -30.0f;
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
    //glEnable(GL_CULL_FACE);    // DISABLED SO THAT IT SEES INSIDE OF SKYBOX
    glEnable(GL_DEPTH_TEST);


   
    
    bool reversed = false;
    bool followCam = true;

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

        // Change color to green for ground/court
        tempColor[0] = 0.4f;        // Value for Red
        tempColor[1] = 0.6f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);

        GLuint gridMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.26f, 0.0f)) 
            * scale(mat4(1.0f), vec3(78.0f, 0.001f, 36.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        //  ----------------------- Draw Grid 78x36 ------------------------
        // Change color to white for grid
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);

        gridMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        // DRAWING 78 LINES 
        for(float x = -39; x < 39; x++) { 
            mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(x, -0.25f, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.05f, 0.05f, 36.0f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridXWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        }
        // DRAWING 36 LINES
        for(float x = -18; x < 18; x++) {
            mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, x)) 
                * scale(mat4(1.0f), vec3(78.0f, 0.05f, 0.05f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridZWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        }

        // -------------------- SKYBOX  ----------------------------------
        // sky color
        tempColor[0] = 0.5f;        // Value for Red
        tempColor[1] = 0.71f;        // Value for Green
        tempColor[2] = 0.86f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // THIS IS 1 UNIT 
        mat4 skyWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 20.0f, 0.0f)) 
            * scale(mat4(1.0f), vec3(100.0f, 50.0f, 95.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &skyWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0      

        // -------------------- COORDINATE AXIS ----------------------------------
        // -------------------- CENTER OF AXIS -----------------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);

        mat4 middleWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 0.0f, -25.0f)) * scale(mat4(1.0f), vec3(0.501f, 0.501f, 0.501f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &middleWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0 

        // -------------------- X AXIS -------------------------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(7.5f, 0.0f, -25.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.5f, 0.5f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridXWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // -------------------- Z AXIS -------------------------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 0.0f, -22.5f))* scale(mat4(1.0f), vec3(0.5f, 0.5f, 5.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridZWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // -------------------- Y AXIS -------------------------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridYWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 2.5f, -25.0f)) * scale(mat4(1.0f), vec3(0.5f, 5.0f, 0.5f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &gridYWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // ------------------ CREATING TENNIS NET -------------------------------------
        // ------------------ SIDE POLES ----------------------------------------------
        tempColor[0] = 0.2f;        // Value for Red
        tempColor[1] = 0.2f;        // Value for Green
        tempColor[2] = 0.2f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
       
        mat4 polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, 17.51f)) 
            * scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &polesWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, -17.51f)) 
            * scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &polesWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // ------------------ WHITE NET ON TOP ----------------------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        mat4 netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 4.5f, 0.0f)) 
            * scale(mat4(1.0f), vec3(0.055f, 0.55f, 34.5f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &netWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // ------------------ BLACK NET (MESH) ----------------------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        for (int i = 0; i < 17; i++) {  // HORIZONTAL STRIPES
            netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, float(i)/4 + 0.1f , 0.0f)) 
                * scale(mat4(1.0f), vec3(0.05f, 0.1f, 34.5f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &netWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        }
        for (int i = -69; i < 69; i++) {  // VERTICAL STRIPES
            netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.225f, float(i)/4 + 0.1f)) 
                * scale(mat4(1.0f), vec3(0.05f, 4.45f, 0.1f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &netWorldMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        }

        // ------------------ CREATING ALPHABET CHARACTERS --------------------------
        // ------------------ CHARACTER H --------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // CENTER BAR
        mat4 charH1WorldMatrix = translate(mat4(1.0f), characterHmovement)
            * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
            * scale(mat4(1.0f), characterHorizontalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charH1WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // SIDE BAR 1
        mat4 charH2WorldMatrix = translate(mat4(1.0f), characterHmovement) 
            * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
            * scale(mat4(1.0f), characterVerticalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charH2WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // SIDE BAR 2
        mat4 charH3WorldMatrix = translate(mat4(1.0f), characterHmovement) 
            * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.5f)) 
            * scale(mat4(1.0f), characterVerticalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charH3WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // ------------------ CHARACTER O --------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // HORIZONTAL BAR 1
        mat4 charO1WorldMatrix = translate(mat4(1.0f), characterOmovement) 
            * translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f))
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
            * scale(mat4(1.0f), characterHorizontalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charO1WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // HORIZONTAL BAR 2
        mat4 charO2WorldMatrix = translate(mat4(1.0f), characterOmovement) 
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, -2.5f, 0.0f)) 
            * scale(mat4(1.0f), characterHorizontalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charO2WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // VERTICAL BAR 1
        mat4 charO3WorldMatrix = translate(mat4(1.0f), characterOmovement) 
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
            * scale(mat4(1.0f), characterVerticalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charO3WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // VERTICAL BAR 2
        mat4 charO4WorldMatrix = translate(mat4(1.0f), characterOmovement) 
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.5f)) 
            * scale(mat4(1.0f), characterVerticalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charO4WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // ------------------ CHARACTER N --------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // VERTICAL BAR 1
        mat4 charN1WorldMatrix = translate(mat4(1.0f), characterNmovement) 
            * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
            * scale(mat4(1.0f), characterVerticalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charN1WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // VERTICAL BAR 2
        mat4 charN2WorldMatrix = translate(mat4(1.0f), characterNmovement) 
            * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.5f)) 
            * scale(mat4(1.0f), characterVerticalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charN2WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // DIAGONAL BAR 1
        mat4 charN3WorldMatrix = translate(mat4(1.0f), characterNmovement) 
            * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
            * rotate(mat4(1.0f), radians(53.13f), vec3(1.0f, 0.0f, 0.0f)) 
            * scale(mat4(1.0f), vec3(1.0f, 4.5f, 1.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charN3WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // ------------------ CHARACTER G --------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 0.5f;        // Value for Green
        tempColor[2] = 0.1f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // VERTICAL BAR LEFT
        mat4 charG1WorldMatrix = translate(mat4(1.0f), characterGmovement) 
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
            * scale(mat4(1.0f), characterVerticalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charG1WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // VERTICAL BAR RIGHT
        mat4 charG2WorldMatrix = translate(mat4(1.0f), characterGmovement) 
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, -1.0f, -1.5f)) 
            * scale(mat4(1.0f), vec3(1.0f, 2.0f, 1.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charG2WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // HORIZONTAL TOP BAR 1
        mat4 charG3WorldMatrix = translate(mat4(1.0f), characterGmovement) 
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 2.0f, 0.0f)) 
            * scale(mat4(1.0f), characterHorizontalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charG3WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // HORIZONTAL BOT BAR 2
        mat4 charG4WorldMatrix = translate(mat4(1.0f), characterGmovement) 
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, -2.0f, 0.0f)) 
            * scale(mat4(1.0f), characterHorizontalSize);
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charG4WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        // HORIZONTAL MIDDLE BAR 3
        mat4 charG5WorldMatrix = translate(mat4(1.0f), characterGmovement) 
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
            * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.0f)) 
            * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.5f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &charG5WorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // ------------------ CREATING RACKET -----------------------------------------
        // ------------------ RACKET CHARACTER H --------------------------------------
        // ------------------ RACKET HANDLE -------------------------------------------
        tempColor[0] = 0.4f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        mat4 racketHandleWorldMatrix = translate(mat4(1.0f), racketHandlePosH) 
            * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
            * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketHandleWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET SURFACE --------------------------------------
        tempColor[0] = 0.6f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH) 
            * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
            * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET NET --------------------------------------
        tempColor[0] = 0.3f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.9f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        for (int i = -4; i < 5; i++) {          // VERTICAL GRID
            vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH + offset) * translate(mat4(1.0f), -1.0f * offset) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                * translate(mat4(1.0f), offset) 
                * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }
        for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
            vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH + offset) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));

            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }

        // ------------------ LOWER ARM ------------------------------------------
        tempColor[0] = 0.7f;        // Value for Red
        tempColor[1] = 0.6f;        // Value for Green
        tempColor[2] = 0.5f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        mat4 lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosH)
            * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
            * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), lowArmPosOffset)
            * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &lowerArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ UPPER ARM ------------------------------------------
        tempColor[0] = 0.8f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.6f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        mat4 upperArmWorldMatrix = translate(mat4(1.0f), upArmPosH) 
            * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &upperArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET CHARACTER O --------------------------------------
        // ------------------ RACKET HANDLE -------------------------------------------
        tempColor[0] = 0.4f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        racketHandleWorldMatrix = translate(mat4(1.0f), racketHandlePosO)
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
            * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketHandleWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET SURFACE --------------------------------------
        tempColor[0] = 0.6f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        racketWorldMatrix = translate(mat4(1.0f), racketPosO)
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
            * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET NET --------------------------------------
        tempColor[0] = 0.3f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.9f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        for (int i = -4; i < 5; i++) {          // VERTICAL GRID
            vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosO + offset) * translate(mat4(1.0f), -1.0f * offset)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), offset)
                * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }
        for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
            vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosO + offset)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));

            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }

        // ------------------ LOWER ARM ------------------------------------------
        tempColor[0] = 0.7f;        // Value for Red
        tempColor[1] = 0.6f;        // Value for Green
        tempColor[2] = 0.5f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosO)
            * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), lowArmPosOffset)
            * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &lowerArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ UPPER ARM ------------------------------------------
        tempColor[0] = 0.8f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.6f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        upperArmWorldMatrix = translate(mat4(1.0f), upArmPosO)
            * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &upperArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET CHARACTER N --------------------------------------
        // ------------------ RACKET HANDLE -------------------------------------------
        tempColor[0] = 0.4f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        racketHandleWorldMatrix =  translate(mat4(1.0f), racketHandlePosN)
            * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0, 0.0f))
            * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketHandleWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET SURFACE --------------------------------------
        tempColor[0] = 0.6f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        racketWorldMatrix = translate(mat4(1.0f), racketPosN)
            * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0, 0.0f))
            * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET NET --------------------------------------
        tempColor[0] = 0.3f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.9f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        for (int i = -4; i < 5; i++) {          // VERTICAL GRID
            vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosN + offset) * translate(mat4(1.0f), -1.0f * offset)
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), offset)
                * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }
        for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
            vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosN + offset)
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0, 0.0f))
                * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));

            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }

        // ------------------ LOWER ARM ------------------------------------------
        tempColor[0] = 0.7f;        // Value for Red
        tempColor[1] = 0.6f;        // Value for Green
        tempColor[2] = 0.5f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosN)
            * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
            * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), lowArmPosOffset)
            * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &lowerArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ UPPER ARM ------------------------------------------
        tempColor[0] = 0.8f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.6f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        upperArmWorldMatrix = translate(mat4(1.0f), upArmPosN)
            * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &upperArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET CHARACTER G --------------------------------------
        // ------------------ RACKET HANDLE -------------------------------------------
        tempColor[0] = 0.4f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        racketHandleWorldMatrix = translate(mat4(1.0f), racketHandlePosG)
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0, 0.0f))
            * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketHandleWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET SURFACE --------------------------------------
        tempColor[0] = 0.6f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.4f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        racketWorldMatrix = translate(mat4(1.0f), racketPosG)
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0, 0.0f))
            * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ RACKET NET --------------------------------------
        tempColor[0] = 0.3f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.9f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        for (int i = -4; i < 5; i++) {          // VERTICAL GRID
            vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
            mat4 racketWorldMatrix =  translate(mat4(1.0f), racketPosG + offset) * translate(mat4(1.0f), -1.0f * offset)
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), offset)
                * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }
        for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
            vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosG + offset)
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0, 0.0f))
                * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));

            glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
        }

        // ------------------ LOWER ARM ------------------------------------------
        tempColor[0] = 0.7f;        // Value for Red
        tempColor[1] = 0.6f;        // Value for Green
        tempColor[2] = 0.5f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);

        lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosG)
            * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), lowArmPosOffset)
            * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &lowerArmWorldMatrix[0][0]);
        glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

        // ------------------ UPPER ARM ------------------------------------------
        tempColor[0] = 0.8f;        // Value for Red
        tempColor[1] = 0.7f;        // Value for Green
        tempColor[2] = 0.6f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        upperArmWorldMatrix = translate(mat4(1.0f), upArmPosG)
            * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0, 0.0f))
            * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
            * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
        glUniformMatrix4fv(gridMatrixLocation, 1, GL_FALSE, &upperArmWorldMatrix[0][0]);
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
        const float cameraAngularSpeed = 25.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        if (cameraHorizontalAngle > 360)
        {
            cameraHorizontalAngle -= 360;
        }
        else if (cameraHorizontalAngle < -360)
        {
            cameraHorizontalAngle += 360;
        }
        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);
        cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(cameraSideVector);

        float currentCameraSpeed = 20;

        // ------------------------------------------------------------------------------------
        // ------------------------------ Handle inputs ---------------------------------------
        // ------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        // ------------------------------ CHOOSE CHARACTER ------------------------------------
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { // GOES TO CHARACTER H
            currentCharControl = &characterHmovement;
            currentRacketRotationY = &racketHRotationY;
            reversed = false; 
            // MOVING CAMERA TO BEHIND THE CHARACTER
            cameraPosition = vec3(45.0f, 12.5f, 10.0f);
            cameraHorizontalAngle = 180.0f;
            cameraVerticalAngle = -10.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { // GOES TO CHARACTER O
            currentCharControl = &characterOmovement;
            currentRacketRotationY = &racketORotationY;
            reversed = false;
            cameraPosition = vec3(45.0f, 12.5f, -10.0f);
            cameraHorizontalAngle = 180.0f;
            cameraVerticalAngle = -10.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { // GOES TO CHARACTER N
            currentCharControl = &characterNmovement;
            currentRacketRotationY = &racketNRotationY;
            reversed = true;
            cameraPosition = vec3(-45.0f, 12.5f, -10.0f);
            cameraHorizontalAngle = 0.0f;
            cameraVerticalAngle = -10.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { // GOES TO CHARACTER G
            currentCharControl = &characterGmovement;
            currentRacketRotationY = &racketGRotationY;
            reversed = true;
            cameraPosition = vec3(-45.0f, 12.5f, 10.0f);
            cameraHorizontalAngle = 00.0f;
            cameraVerticalAngle = -10.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) { // CAMERA FOLLOW MOVEMENTS OF RACKET
            if (!keyPressed) {
                followCam = !followCam;
                keyPressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
            keyPressed = false;
        }

        // ------------------------------ UPDATE MODEL POSITION --------------------------------
        shift = false;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            shift = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {     
            if(shift) {                                             // MOVE MODEL LEFT
                if(reversed) {
                    *currentCharControl += vec3(0.0f, 0.0f, -0.1f);
                    if (followCam) {
                        cameraPosition += vec3(0.0f, 0.0f, -0.1f);
                    }
                } else {
                    *currentCharControl += vec3(0.0f, 0.0f, 0.1f);
                    if (followCam) {
                        cameraPosition += vec3(0.0f, 0.0f, 0.1f);
                    }
                }                  
                update();
            } else {                                            // ROTATE 5 DEGREE COUNTERCLOCKWISE
            *currentRacketRotationY += 3.0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if(shift) {    
                if(reversed) {
                    *currentCharControl += vec3(0.0f, 0.0f, 0.1f);
                    if (followCam) {
                        cameraPosition += vec3(0.0f, 0.0f, 0.1f);
                    }
                } else {
                    *currentCharControl += vec3(0.0f, 0.0f, -0.1f);
                    if (followCam) {
                        cameraPosition += vec3(0.0f, 0.0f, -0.1f);
                    }
                }                                     // MOVE MODEL RIGHT
                update();
            } else {                                            // ROTATE 5 DEGREE CLOCKWISE
            *currentRacketRotationY  -= 3.0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if(shift) {                                         // MOVE MODEL FORWARD
                if(reversed) {
                    *currentCharControl += vec3(0.1f, 0.0f, 0.0f);
                    if (followCam) {
                        cameraPosition += vec3(0.1f, 0.0f, 0.0f);
                    }
                } else {
                    *currentCharControl += vec3(-0.1f, 0.0f, 0.0f);
                    if (followCam) {
                        cameraPosition += vec3(-0.1f, 0.0f, 0.0f);
                    }
                }
                update();
            } else {
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if(shift) {    
                if(reversed) {
                    *currentCharControl += vec3(-0.1f, 0.0f, 0.0f);
                    if (followCam) {
                        cameraPosition += vec3(-0.1f, 0.0f, 0.0f);
                    }
                } else {
                    *currentCharControl += vec3(0.1f, 0.0f, 0.0f);
                    if (followCam) {
                        cameraPosition += vec3(0.1f, 0.0f, 0.0f);
                    }
                }                                     // MOVE MODEL BACK
                update();
            } else {
            }
        }
        // ------------------------------ MOVE CAMERA --------------------------------
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  // move camera to the left
            cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { // move camera to the right
            cameraPosition += cameraSideVector * currentCameraSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {  // move camera backward
            cameraPosition -= cameraLookAt * currentCameraSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {    // move camera forward
            cameraPosition += cameraLookAt * currentCameraSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { // move camera up
            cameraPosition += cameraUp * (currentCameraSpeed / 2) * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { // move camera down
            cameraPosition -= cameraUp * (currentCameraSpeed / 2) * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {     // RESET EVERYTHING TO INITIAL
            cameraPosition = vec3(0.0f, 25.0f, 45.0f);
            cameraHorizontalAngle = 90.0f;
            cameraVerticalAngle = -30.0f;

            characterHmovement = vec3(30.0f, 15.0f, 10.0f);
            characterOmovement = vec3(30.0f, 15.0f, -10.0f);
            characterNmovement = vec3(-30.0f, 15.0f, -10.0f);
            characterGmovement = vec3(-30.0f, 15.0f, 10.0f);
            update();
        }

        mat4 viewMatrix = mat4(1.0);
        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    }
    // Shutdown GLFW
    glfwTerminate();
    
    return 0;
}
