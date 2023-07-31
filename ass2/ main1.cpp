

#define M_PI   3.14159265358979323846264338327950288
#include <iostream>
#include <vector>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>

#include<random>       // RANDOM NUMBER GENERATOR - https://cplusplus.com/reference/random/

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/shaderloader.h"

using namespace glm;
using namespace std;

// OpenGL Sphere by Song Ho - http://www.songho.ca/opengl/gl_sphere.html

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix);
void setViewMatrix(int shaderProgram, mat4 viewMatrix);
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value);
void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value);
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value);

void createSPhere(vector<vec3>& vertices, vector<vec3>& normals, vector<vec2>& UV, vector<int>& indices, float radius, int slices, int stacks);
GLuint setupModelEBO(int& vertexCount, vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> UVs, vector<int> vertexIndices);

int createCubeVAO();

GLuint loadTexture(const char* filename);


float randomInRange(float lowerBound, float upperBound);

struct TexturedColoredVertex {
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
        : position(_position), color(_color), uv(_uv) {}

    vec3 position;
    vec3 color;
    vec2 uv;
};



float modelScale = 1;
float upperArmRotationXAngle = 0;
float upperArmRotationYAngle = 0;

vec3 lowerArmPosOffset = vec3(5.0f, 6.5f, 0.0f);
vec3 racketHandlePosOffset = vec3(0.0f, 8.0f, 0.0f);
vec3 racketPosOffset = vec3(0.0f, 8.0f, 0.0f);

vec3 upperArmPos = vec3(0.0f, 6.0f, 0.0f);
vec3 lowerArmPos = upperArmPos + lowerArmPosOffset;
vec3 racketHandlePos = lowerArmPos + racketHandlePosOffset;
vec3 racketPos = racketHandlePos + racketPosOffset;
vec3 racketNetPos = racketHandlePos + racketPosOffset;

vec3 cameraPosition(0.0f, 15.0f, 40.0f);
vec3 cameraLookAt(0.0f,-5.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
float cameraAngleZ = 0.0f;
float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;

void update() {
    lowerArmPos = upperArmPos + lowerArmPosOffset;
    racketHandlePos = lowerArmPos + racketHandlePosOffset;
    racketPos = racketHandlePos + racketPosOffset;
    racketNetPos = racketHandlePos + racketPosOffset;
}

bool keyPressed = false;
bool shift = false;
int renderingMode = GL_TRIANGLES;
GLFWwindow* window = nullptr;

int main(int argc, char*argv[]) {
    { // SETUP context
        // Initialize GLFW and OpenGL version
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // Create Window and rendering context using GLFW, resolution is 1024x768
        window = glfwCreateWindow(1024, 768, "Comp371 - Quiz 1", NULL, NULL);
        if (window == NULL) {
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
    }
    
    // green background
    glClearColor(0.20f, 0.3f, 0.3f, 1.0f);

    // Load Textures
    GLuint tennisTextureID = loadTexture("assets/textures/tennisTexture.jpg");
    GLuint clayTextureID = loadTexture("assets/textures/clay.jpg");

    // Compile and link shaders here ...
    string shaderPathPrefix = "assets/shaders/";
    int shaderProgram           = loadSHADER(shaderPathPrefix + "temp_vertex.glsl", shaderPathPrefix + "temp_fragment.glsl");
    int texturedShaderProgram   = loadSHADER(shaderPathPrefix + "texture_vertex.glsl", shaderPathPrefix + "texture_fragment.glsl");
    int shadowShaderProgram     = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl");

    // Dimensions of the shadow texture, which should cover the viewport window size and shouldn't be oversized and waste resources
    const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;
        
    // Variable storing index to texture used for shadow mapping
    GLuint depth_map_texture;
    // Get the texture
    glGenTextures(1, &depth_map_texture);
    // Bind the texture so the next glTex calls affect it
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    // Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                NULL);
    // Set texture sampler parameters.
    // The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // Variable storing index to framebuffer used for shadow mapping
    GLuint depth_map_fbo;  // fbo: framebuffer object
    // Get the framebuffer
    glGenFramebuffers(1, &depth_map_fbo);
    // Bind the framebuffer so the next glFramebuffer calls affect it
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    // Attach the depth map texture to the depth map framebuffer
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


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
    

    setViewMatrix(shaderProgram, viewMatrix);
    setViewMatrix(texturedShaderProgram, viewMatrix);

    setProjectionMatrix(shaderProgram, projectionMatrix);
    setProjectionMatrix(texturedShaderProgram, projectionMatrix);

    int vao = createCubeVAO();

    // ---------------------------------------------------------
    // -------------------- SPHERE -----------------------------
    // ---------------------------------------------------------
    vector<int> vertexIndices;
    //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;
    createSPhere(vertices, normals, UVs, vertexIndices, 3.0f, 40, 40);
    int sphereVertices;
    GLuint sphereVAO = setupModelEBO(sphereVertices, vertices, normals, UVs, vertexIndices);

    int activeVertices = sphereVertices;
    GLuint activeVAO = sphereVAO;
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------

    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    float texColor[3] = {1.0f, 1.0f, 1.0f};
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
        GLuint texColorLocation = glGetUniformLocation(texturedShaderProgram, "customColor");

        // Draw Geometry
        glBindVertexArray(vao);

        // ---------------------------------------------------------------------
        // -------------------------- CLAY GROUND ------------------------------
        // ---------------------------------------------------------------------
        glUseProgram(texturedShaderProgram);
        glUniform3fv(texColorLocation, 1, texColor);

        glActiveTexture(GL_TEXTURE0);
        GLuint textureLocation = glGetUniformLocation(texturedShaderProgram, "textureSampler");
        glBindTexture(GL_TEXTURE_2D, clayTextureID);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0

        GLuint groundMatrixLocation = glGetUniformLocation(texturedShaderProgram, "worldMatrix");
        mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.26f, 0.0f)) 
            * scale(mat4(1.0f), vec3(100.0f, 0.01f, 100.0f));
        glUniformMatrix4fv(groundMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // --------------------------------------------------------------------------------------
        //  ----------------------- Draw Grid 100x100 -------------------------------------------
        // --------------------------------------------------------------------------------------
        glUseProgram(shaderProgram);
        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        // Change shader Color to Yellow
        tempColor[0] = 0.9f;        // Value for Red
        tempColor[1] = 0.9f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        for(float x = -50; x < 50; x++) {
            // glUseProgram(shaderProgram2);
            mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(x, -0.25f, 0.0f)) 
            * scale(mat4(1.0f), vec3(0.1f, 0.1f, 100.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridXWorldMatrix[0][0]);

            // glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            
            mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, x)) 
            * scale(mat4(1.0f), vec3(100.0f, 0.10f, 0.10f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridZWorldMatrix[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        }

        // --------------------------------------------------------------------------------------
        // -------------------- COORDINATE AXIS -------------------------------------------------
        // --------------------------------------------------------------------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        // THIS IS 1 UNIT 
        // mat4 middleWorldMatrix = translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.5f)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        mat4 middleWorldMatrix = scale(mat4(1.0f), vec3(0.501f, 0.501f, 0.501f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &middleWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0      
        // -------------------- X AXIS -------------------------------------------
        tempColor[0] = 1.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(2.5f, 0.0f, 0.0f)) 
        * scale(mat4(1.0f), vec3(5.0f, 0.5f, 0.5f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridXWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // -------------------- Z AXIS ----------------------------------------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 1.0f;        // Value for Green
        tempColor[2] = 0.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 2.5f)) 
        * scale(mat4(1.0f), vec3(0.5f, 0.5f, 5.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridZWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // -------------------- Y AXIS ----------------------------------------------------------
        tempColor[0] = 0.0f;        // Value for Red
        tempColor[1] = 0.0f;        // Value for Green
        tempColor[2] = 1.0f;        // Value for Blue
        glUniform3fv(colorLocation, 1, tempColor);
        
        mat4 gridYWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f)) 
        * scale(mat4(1.0f), vec3(0.5f, 5.0f, 0.5f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridYWorldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        // --------------------------------------------------------------------------------------
        // ------------------ CREATING MODEL ----------------------------------------------------
        // --------------------------------------------------------------------------------------
        {
            // ------------------ UPPER ARM ---------------------------------------------------------
            tempColor[0] = 0.8f;        // Value for Red
            tempColor[1] = 0.7f;        // Value for Green
            tempColor[2] = 0.6f;        // Value for Blue
            glUniform3fv(colorLocation, 1, tempColor);
            // vec3 upperArmPos = vec3(10.0f, 5.0f, -20.0f);
            mat4 upperArmWorldMatrix = scale(mat4(1.0f), vec3(modelScale, modelScale, modelScale))
                * translate(mat4(1.0f), upperArmPos)
                * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f))
                * rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 0.0f, 1.0f))
                * scale(mat4(1.0f), vec3(12.0f, 2.0f, 2.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &upperArmWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

            // ------------------ LOWER ARM ---------------------------------------------------------
            tempColor[0] = 0.7f;        // Value for Red
            tempColor[1] = 0.6f;        // Value for Green
            tempColor[2] = 0.5f;        // Value for Blue
            glUniform3fv(colorLocation, 1, tempColor);
            // vec3 lowerArmPos = vec3(upperArmPos.x + 6.0f, upperArmPos.y + 4.0f, upperArmPos.z + 0.0f);
            mat4 lowerArmWorldMatrix = scale(mat4(1.0f), vec3(modelScale, modelScale, modelScale))
                * translate(mat4(1.0f), (upperArmPos + lowerArmPosOffset))
                * translate(mat4(1.0f), -1.0f * lowerArmPosOffset)
                * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), lowerArmPosOffset)
                * scale(mat4(1.0f), vec3(1.5f, 8.0f, 1.5f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &lowerArmWorldMatrix[0][0]);

            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

            // ------------------ RACKET HANDLE  ----------------------------------------------------
            tempColor[0] = 0.4f;        // Value for Red
            tempColor[1] = 0.7f;        // Value for Green
            tempColor[2] = 0.4f;        // Value for Blue
            glUniform3fv(colorLocation, 1, tempColor);
            // vec3 lowerArmPos = vec3(upperArmPos.x + 6.0f, upperArmPos.y + 4.0f, upperArmPos.z + 0.0f);
            mat4 racketHandleWorldMatrix = scale(mat4(1.0f), vec3(modelScale, modelScale, modelScale))
                * translate(mat4(1.0f), (lowerArmPos + racketHandlePosOffset))
                * translate(mat4(1.0f), -1.0f * lowerArmPosOffset)
                * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), lowerArmPosOffset)
                * scale(mat4(1.0f), vec3(0.75f, 8.0f, 0.75f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &racketHandleWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

            // ------------------ RACKET SURFACE ----------------------------------------------------
            tempColor[0] = 0.6f;        // Value for Red
            tempColor[1] = 0.0f;        // Value for Green
            tempColor[2] = 0.4f;        // Value for Blue
            glUniform3fv(colorLocation, 1, tempColor);
            // vec3 lowerArmPos = vec3(upperArmPos.x + 6.0f, upperArmPos.y + 4.0f, upperArmPos.z + 0.0f);
            mat4 racketWorldMatrix = scale(mat4(1.0f), vec3(modelScale, modelScale, modelScale))
                * translate(mat4(1.0f), (racketHandlePos + racketPosOffset))
                * translate(mat4(1.0f), -1.0f * lowerArmPosOffset)
                * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), lowerArmPosOffset)
                * scale(mat4(1.0f), vec3(5.0f, 8.0f, 1.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
            glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0

            // ------------------ RACKET NET --------------------------------------------------------
            tempColor[0] = 0.3f;        // Value for Red
            tempColor[1] = 1.0f;        // Value for Green
            tempColor[2] = 0.3f;        // Value for Blue
            glUniform3fv(colorLocation, 1, tempColor);
            for (int i = -4; i < 5; i++) {
                vec3 offset = vec3(i * 0.5f, 0.0f, 0.0f);
                mat4 racketWorldMatrix = scale(mat4(1.0f), vec3(modelScale, modelScale, modelScale))
                    * translate(mat4(1.0f), (racketHandlePos + racketPosOffset + offset))
                    * translate(mat4(1.0f), -1.0f * (lowerArmPosOffset + offset))
                    * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f))
                    * translate(mat4(1.0f), lowerArmPosOffset + offset)
                    * scale(mat4(1.0f), vec3(0.1f, 7.0f, 1.1f));
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
                glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
            }
            for (int i = -7; i < 8; i++) {
                vec3 offset = vec3(0.0f, i * 0.5f, 0.0f);
                mat4 racketWorldMatrix = scale(mat4(1.0f), vec3(modelScale, modelScale, modelScale))
                    * translate(mat4(1.0f), (racketHandlePos + racketPosOffset + offset))
                    * translate(mat4(1.0f), -1.0f * lowerArmPosOffset)
                    * rotate(mat4(1.0f), radians(upperArmRotationXAngle), vec3(0.0f, 1.0f, 0.0f))
                    * translate(mat4(1.0f), lowerArmPosOffset)
                    * scale(mat4(1.0f), vec3(4.0f, 0.1f, 1.1f));
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &racketWorldMatrix[0][0]);
                glDrawArrays(renderingMode, 0, 36); // 36 vertices, starting at index 0
            }
        }
        // --------------------------------------------------------------------------------------
        // ------------------- TENNIS BALL ------------------------------------------------------
        // --------------------------------------------------------------------------------------
        {
            glBindVertexArray(0);
            glBindVertexArray(activeVAO);

            glUseProgram(texturedShaderProgram);
            glUniform3fv(texColorLocation, 1, texColor);

            glBindTexture(GL_TEXTURE_2D, tennisTextureID);


            mat4 sphereWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 15.0f, 0.0f))
                * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(texturedShaderProgram, "worldMatrix");
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &sphereWorldMatrix[0][0]);
            // Draw geometry
            glDrawElements(renderingMode, activeVertices, GL_UNSIGNED_INT, 0);
            // Unbind geometry
            glBindVertexArray(0);
        }
        glUseProgram(shaderProgram);
        
         // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();


        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;
       
        // ------------------------------------------------------------------------------------
        // ------------------------------ Handle inputs ---------------------------------------
        // ------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // --------------------------------------------------------------------------------------
        // ------------------------------ REPOSITION MODEL --------------------------------------
        // --------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if(!keyPressed) {
                float randomX = randomInRange(-25.0f, 25.0f);
                float randomY = randomInRange(5.0f, 15.0f);
                float randomZ = randomInRange(-25.0f, 25.0f);

                upperArmPos = vec3(randomX, randomY, randomZ);
                update();
                keyPressed = true;
            }             
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
                keyPressed = false;
        }

        // --------------------------------------------------------------------------------------
        // ------------------------------ MODEL SCALE -------------------------------------------
        // --------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {      // SCALE UP
            modelScale += 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {     // SCALE DOWN
            modelScale -= 0.01;
        }

        // --------------------------------------------------------------------------------------
        // ------------------------------ UPDATE MODEL POSITION ---------------------------------
        // --------------------------------------------------------------------------------------
        shift = false;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { 
            shift = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {     
            if(shift) {                                         // MOVE MODEL LEFT
               upperArmPos -= vec3(0.1f, 0.0f, 0.0f);
               update();
            } else {                                            // ROTATE 5 DEGREE COUNTERCLOCKWISE
            upperArmRotationXAngle  += 5.0f;
            update();
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if(shift) {                                         // MOVE MODEL RIGHT
                upperArmPos += vec3(0.1f * cos(radians(upperArmRotationXAngle)), 0.0f, -0.1f * sin(radians(upperArmRotationXAngle)));
                update();
            } else {                                            // ROTATE 5 DEGREE CLOCKWISE
            upperArmRotationXAngle  -= 5.0;
            update();
            }
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if(shift) {                                         // MOVE MODEL UP
                upperArmPos += vec3(0.0f, 0.1f, 0.0f);
                update();
            } else {
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if(shift) {                                         // MOVE MODEL DOWN
                upperArmPos -= vec3(0.0f, 0.1f, 0.0f);
                update();
            } else {
            }
        }
        // --------------------------------------------------------------------------------------
        // ------------------------------ CHANGE WORLD ORIENTATION ------------------------------
        // --------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {      // move camera to the left
            cameraAngleX += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {     // move camera to the right
            cameraAngleX -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {      // move camera back
            cameraAngleY -= 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {        // move camera forward
            cameraAngleY += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) { // ZOOM OUT 
            cameraAngleX = 0.0f;
            cameraAngleY = 0.0f;
            cameraAngleZ = 0.0f;
        }

        // --------------------------------------------------------------------------------------
        // ------------------- RENDERING MODE ---------------------------------------------------
        // --------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { // RENDER TRIANGLES   
            renderingMode = GL_TRIANGLES;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { // RENDER LINES
            renderingMode = GL_LINES;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { // RENDER POINTS
            renderingMode = GL_POINTS;
        }

        // --------------------------------------------------------------------------------------
        // --------------------- CAMERA PAN AND TILT  -------------------------------------------
        // --------------------------------------------------------------------------------------
        const float cameraAngularSpeed = 15.0f;
        float theta;
        float phi;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
            if (cameraHorizontalAngle > 360) {
                cameraHorizontalAngle -= 360;
            }
            else if (cameraHorizontalAngle < -360) {
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

        // --------------------------------------------------------------------------------------
        // --------------------- ZOOM IN AND ZOOM OUT -------------------------------------------
        // --------------------------------------------------------------------------------------
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
        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp) * rotate(mat4(1.0f), radians(cameraAngleX), vec3(0.0f, 1.0f, 0.0f)) *  rotate(mat4(1.0f), radians(cameraAngleY), vec3(1.0f, 0.0f, 0.0f));
        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

        setViewMatrix(texturedShaderProgram, viewMatrix);
    }
    
    // Shutdown GLFW
    glfwTerminate();
    
    return 0;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix) {
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}
void setViewMatrix(int shaderProgram, mat4 viewMatrix) {
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}
// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value) {
  glUseProgram(shader_id);
  glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}
void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value) {
  glUseProgram(shader_id);
  glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));
}
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value) {
  glUseProgram(shader_id);
  glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
  glUseProgram(0);
}

void createSPhere(vector<vec3>& vertices, vector<vec3>& normals, vector<vec2>& UV, vector<int>& indices, float radius, int slices, int stacks) {
    int k1, k2;
    for (int i = 0; i <= slices; i++) {
        k1 = i * (stacks + 1);
        k2 = k1 + stacks + 1;
        for (int j = 0; j <= stacks; j++, k1++, k2++) {
            vec3 v;
            float theta = 2.0f * M_PI * j / slices;
            float phi = M_PI * i / stacks;
            v.x = radius * cos(theta) * sin(phi);
            v.y = radius * sin(theta) * sin(phi);
            v.z = radius * cos(phi);
            vertices.push_back(v);
            vec3 n(v.x / radius, v.y / radius, v.z / radius);
            normals.push_back(n);
            vec2 m;
            m.x = (float)j / (float)slices;
            m.y = (float)i / (float)stacks;
            UV.push_back(m);

            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (slices - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
GLuint setupModelEBO(int& vertexCount, vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> UVs, vector<int> vertexIndices) {
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    //EBO setup
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertexIndices.size();
    return VAO;
}

int createCubeVAO() {
    // Cube model
    const TexturedColoredVertex vertexArray[] = {  // position,                            color
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)), //left - red
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 10.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 1.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(10.0f, 10.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(10.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 10.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 10.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 10.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 10.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)), // right - purple
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 10.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)), // top - yellow
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(1.0f, 1.0f, 1.0f), vec2(10.0f, 10.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec2(0.0f, 10.0f))
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
        sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    
    return vertexBufferObject;
}

GLuint loadTexture(const char* filename) {
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);

    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

float randomInRange(float lowerBound, float upperBound) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(lowerBound, upperBound);
    return dist(gen);
}