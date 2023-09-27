

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
// BASE CODE OF QUIZ 1 AND ASSIGNMENT 2
// TEXTURES FROM LAB 4
// SHADOWS FROM LAB 8

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix);
void setViewMatrix(int shaderProgram, mat4 viewMatrix);
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value);
void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value);
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value);
void setCam1();
void setCam2();
void setCam3();
int createCubeVAO();

GLuint loadTexture(const char* filename);

struct TexturedColoredVertex {
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
        : position(_position), color(_color), uv(_uv) {}

    vec3 position;
    vec3 color;
    vec2 uv;
};

vec3 cameraPosition(0.0f, 25.0f, 45.0f);
vec3 cameraLookAt(0.0f, 0.0f, 00.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

float cameraAngleX = 0.0f;
float cameraAngleX1 = 0.0f;
float cameraAngleY = 0.0f;
float cameraAngleZ = 0.0f;
float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = -30.0f;
float cameraSpeed = 1.0f;

float spinAngle = 90.0f;


// CHARACTER SIZE 3 X 4
vec3 characterHorizontalSize = vec3(1.0f, 1.0f, 3.0f);
vec3 characterVerticalSize = vec3(1.0f, 4.0f, 1.0f);

// RACKET INITIAL POSITION      - HANDLE IS THE PARENT
vec3 racketHandlePosH = vec3(-30.0f, 5.0f, 0.0f);
vec3 racketHandlePosO = vec3(30.0f, 5.0f, 0.0f);
// RACKET SURFACE POSITION
vec3 racketPosOffset = vec3(0.0f, 3.0f, 0.0f);
vec3 racketPosH = racketHandlePosH + racketPosOffset;
vec3 racketPosO = racketHandlePosO + racketPosOffset;
// LOWER ARM POSITION
vec3 lowArmPosOffset = vec3(0.0f, -2.2f, 0.4f);
vec3 lowArmPosH = racketHandlePosH + lowArmPosOffset;
vec3 lowArmPosO = racketHandlePosO + lowArmPosOffset;
// UPPER ARM POSITION
vec3 upArmPosOffset = vec3(0.0f, -1.1f, 1.25f);
vec3 upArmPosH = lowArmPosH + upArmPosOffset;
vec3 upArmPosO = lowArmPosO + upArmPosOffset;

vec3 characterHmovement = racketHandlePosH + vec3(0.0f, 10.0f, -2.5f);
vec3 characterOmovement = racketHandlePosO + vec3(0.0f, 10.0f, -2.5f);
vec3 characterNmovement = racketHandlePosO + vec3(0.0f, 10.0f, 2.5f);
vec3 characterGmovement = racketHandlePosH + vec3(0.0f, 10.0f, 2.5f);

// ROTATION ANGLES
float racketHRotationY = 0;
float racketORotationY = 0;
float racketNRotationY = 180;
float racketGRotationY = 180;
// POINTER TO CHARACTER YOU CONTROL
vec3* currentCharControl = &racketHandlePosH;
float* currentRacket1RotationY = &racketHRotationY;
float* currentRacket2RotationY = &racketGRotationY;
int currentCam = 3;
void update() {
    racketPosH = racketHandlePosH + racketPosOffset;
    racketPosO = racketHandlePosO + racketPosOffset;
    lowArmPosH = racketHandlePosH + lowArmPosOffset;
    lowArmPosO = racketHandlePosO + lowArmPosOffset;
    upArmPosH = lowArmPosH + upArmPosOffset;
    upArmPosO = lowArmPosO + upArmPosOffset;
    characterHmovement = racketHandlePosH + vec3(0.0f, 10.0f, -2.5f);
    characterOmovement = racketHandlePosO + vec3(0.0f, 10.0f, -2.5f);
    characterNmovement = racketHandlePosO + vec3(0.0f, 10.0f, 2.5f);
    characterGmovement = racketHandlePosH + vec3(0.0f, 10.0f, 2.5f);
}

GLFWwindow* window = nullptr;

bool keyPressed = false;
bool shift = false;
int renderingMode = GL_TRIANGLES;
bool reversed = false;
bool followCam = true;

bool toggleTexture = true;
bool xPressed = false;
bool toggleShadow = true;
bool bPressed = false;
bool mPressed = false;
bool toggleLight = false;
bool lPressed = false;
bool toggleCircle = false;
bool yPressed = false;

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
    
    // gray background but it should'nt matter because of skybox
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    // Load Textures
    GLuint defaultTextureID =   loadTexture("assets/textures/white.png");
    GLuint checkeredTextureID = loadTexture("assets/textures/stripe.jpg");
    GLuint cloudTextureID =     loadTexture("assets/textures/cloud.png");
    GLuint steelTextureID =     loadTexture("assets/textures/steel.jpeg");
    GLuint tattoo1TextureID =   loadTexture("assets/textures/tat1.png");
    GLuint tattoo2TextureID =   loadTexture("assets/textures/tat2.png");
    GLuint woodTextureID =      loadTexture("assets/textures/wood.png");
    GLuint stoneTextureID =     loadTexture("assets/textures/stone.png");
    GLuint netherTextureID =    loadTexture("assets/textures/nether.jpg");
    GLuint blueTextureID =      loadTexture("assets/textures/blue.png");
    

    // Compile and link shaders here ...
    string shaderPathPrefix = "assets/shaders/";
    // int shaderProgram           = loadSHADER(shaderPathPrefix + "temp_vertex.glsl", shaderPathPrefix + "temp_fragment.glsl");
    int shaderProgram           = loadSHADER(shaderPathPrefix + "texture_vertex.glsl", shaderPathPrefix + "texture_fragment.glsl");
    int shadowShaderProgram     = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl");

    // Dimensions of the shadow texture, which should cover the viewport window size and shouldn't be oversized and waste resources
    const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;
        
    GLuint depth_map_texture;
    glGenTextures(1, &depth_map_texture);
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint depth_map_fbo;  // fbo: framebuffer object
    glGenFramebuffers(1, &depth_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    float fov = 70.0f;
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(glm::radians(fov),            // field of view in degrees
                                             1024.0f / 768.0f,  // aspect ratio
                                             0.01f, 180.0f);   // near and far (near > 0)

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp ); // up
    

    setViewMatrix(shaderProgram, viewMatrix);
    setProjectionMatrix(shaderProgram, projectionMatrix);


    int vao = createCubeVAO();

    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    // ------------------------------------------------------------------------

    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint shadowMapUniform = glGetUniformLocation(shaderProgram, "shadow_map");
    glUniform1i(shadowMapUniform, 0); // Texture unit 1 is now bound to texture1

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, defaultTextureID);
    GLint texture1Uniform = glGetUniformLocation(shaderProgram, "textureSampler");
    glUniform1i(texture1Uniform, 1); // Texture unit 1 is now bound to texture1

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, checkeredTextureID);
    glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glUniform1i(texture1Uniform, 3); // Texture unit 3 is now bound to texture1
    
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, steelTextureID);
    glUniform1i(texture1Uniform, 4); // Texture unit 4 is now bound to texture1

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tattoo1TextureID);
    glUniform1i(texture1Uniform, 5); // Texture unit 5 is now bound to texture1

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, tattoo2TextureID);
    glUniform1i(texture1Uniform, 6); // Texture unit 6 is now bound to texture1

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, woodTextureID);
    glUniform1i(texture1Uniform, 7); // Texture unit 7 is now bound to texture1

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, stoneTextureID);
    glUniform1i(texture1Uniform, 8); // Texture unit 8 is now bound to texture1

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, netherTextureID);
    glUniform1i(texture1Uniform, 9); // Texture unit 9 is now bound to texture1

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, blueTextureID);
    glUniform1i(texture1Uniform, 10); // Texture unit 10 is now bound to texture1

    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        vec3 tempColor(0.5f, 0.5f, 0.5f);   // Change Color to Grey
        SetUniformVec3(shaderProgram, "customColor", tempColor);

        // -------------- LIGHTING -----------------------------------

        SetUniformVec3(shaderProgram, "light_color", vec3(1.0f, 1.0f, 1.0f));
        SetUniformVec3(shaderProgram, "light_color1", vec3(1.0f, 0.0f, 0.0f));
        vec3 lightPosition; // the location of the light in 3D space
        vec3 lightFocus;      // the point in 3D space the light "looks" at

        if(toggleCircle){
            lightPosition = cameraPosition+vec3(2.0f * cos(radians(spinAngle)), 1.0f, 2.0f * sin(radians(spinAngle))); // the location of the light in 3D space
            lightFocus = vec3(0.0f, 10.0f, 0.0f);      // the point in 3D space the light "looks" at

        } else {
            lightPosition = vec3(10.0f, 60.0f, 0.0f); // the location of the light in 3D space
            lightFocus = vec3(-5.0f, 0.0f, 1.0f);      // the point in 3D space the light "looks" at

        }
        vec3 lightPosition1 = cameraPosition;

        vec3 lightFocus1 = vec3(0.0f, 1.0f, 1.0f);      // the point in 3D space the light "looks" at

        vec3 lightDirection = normalize(lightFocus - lightPosition);
        vec3 lightDirection1 = normalize(lightFocus1 - lightPosition1);

        float lightNearPlane = 1.0f;
        float lightFarPlane = 180.0f;

        float lightAngleOuter = 30.0;
        float lightAngleInner = 20.0;
        // Set light cutoff angles on scene shader
        SetUniform1Value(shaderProgram, "light_cutoff_inner", cos(radians(lightAngleInner)));
        SetUniform1Value(shaderProgram, "light_cutoff_outer", cos(radians(lightAngleOuter)));
        
        
        mat4 lightProjectionMatrix = frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
        //perspective(20.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
        mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
        mat4 lightViewMatrix1 = lookAt(lightPosition1, lightFocus1, vec3(0.0f, 1.0f, 0.0f));

        mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
        mat4 lightSpaceMatrix1 = lightProjectionMatrix * lightViewMatrix1;

        SetUniformMat4(shadowShaderProgram, "light_view_proj_matrix", lightSpaceMatrix);
        SetUniformMat4(shaderProgram, "light_view_proj_matrix", lightSpaceMatrix);
        SetUniformMat4(shaderProgram, "light_view_proj_matrix1", lightSpaceMatrix1);

        SetUniform1Value(shaderProgram, "light_near_plane", lightNearPlane);
        SetUniform1Value(shaderProgram, "light_far_plane", lightFarPlane);
        
        SetUniformVec3(shaderProgram, "light_position", lightPosition);
        SetUniformVec3(shaderProgram, "light_position1", lightPosition1);

        SetUniformVec3(shaderProgram, "light_direction", lightDirection);
        SetUniformVec3(shaderProgram, "light_direction1", lightDirection1);

        
        // ------------------------- SHADOW PASS -------------------------------
        {
            glUseProgram(shadowShaderProgram);
            glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
            // Bind depth map texture as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            // Clear depth data on the framebuffer
            glClear(GL_DEPTH_BUFFER_BIT);
    
            // ---------------------------------------------------------------------
            // -------------------------- CLAY GROUND ------------------------------
            // ---------------------------------------------------------------------

            mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.26f, 0.0f)) 
                * scale(mat4(1.0f), vec3(78.0f, 0.001f, 36.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", groundWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // -------------------- SKYBOX  ----------------------------------
            mat4 skyWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 20.0f, 0.0f)) 
                * scale(mat4(1.0f), vec3(100.0f, 100.0f, 95.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", skyWorldMatrix);
            
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // --------------------------------------------------------------------------------------
            //  ----------------------- Draw Grid 100x100 -------------------------------------------
            // --------------------------------------------------------------------------------------

            for(float x = -39; x < 39; x++) {
                mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(x, -0.25f, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.05f, 0.05f, 36.0f));
                SetUniformMat4(shadowShaderProgram, "model_matrix", gridXWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for(float x = -18; x < 18; x++) {
                mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, x)) 
                * scale(mat4(1.0f), vec3(78.0f, 0.05f, 0.05f));
                SetUniformMat4(shadowShaderProgram, "model_matrix", gridZWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }

            // --------------------------------------------------------------------------------------
            // -------------------- COORDINATE AXIS -------------------------------------------------
            // --------------------------------------------------------------------------------------
            // THIS IS 1 UNIT 
            mat4 middleWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 0.0f, -25.0f)) 
            * scale(mat4(1.0f), vec3(0.501f, 0.501f, 0.501f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", middleWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // -------------------- X AXIS -------------------------------------------
        
            mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(7.5f, 0.0f, -25.0f)) 
            * scale(mat4(1.0f), vec3(5.0f, 0.5f, 0.5f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", gridXWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // -------------------- Z AXIS ----------------------------------------------------------
            
            mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 0.0f, -22.5f))
            * scale(mat4(1.0f), vec3(0.5f, 0.5f, 5.0f));

            SetUniformMat4(shadowShaderProgram, "model_matrix", gridZWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // -------------------- Y AXIS ----------------------------------------------------------
            
            mat4 gridYWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 2.5f, -25.0f)) 
            * scale(mat4(1.0f), vec3(0.5f, 5.0f, 0.5f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", gridYWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CREATING TENNIS NET -------------------------------------
            // ------------------ SIDE POLES ----------------------------------------------
            mat4 polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, 17.51f)) 
                * scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", polesWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, -17.51f)) 
                * scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", polesWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, 0.0f)) 
                * scale(mat4(1.0f), vec3(1.0f, 4.5f, 1.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", polesWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ WHITE NET ON TOP ----------------------------------------
            mat4 netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 4.5f, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.055f, 0.55f, 34.5f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", netWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ BLACK NET (MESH) ----------------------------------------
            for (int i = 0; i < 17; i++) {  // HORIZONTAL STRIPES
                netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, float(i)/4 + 0.1f , 0.0f)) 
                    * scale(mat4(1.0f), vec3(0.05f, 0.1f, 34.5f));
                SetUniformMat4(shadowShaderProgram, "model_matrix", netWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for (int i = -69; i < 69; i++) {  // VERTICAL STRIPES
                netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.225f, float(i)/4 + 0.1f)) 
                    * scale(mat4(1.0f), vec3(0.05f, 4.45f, 0.1f));
                SetUniformMat4(shadowShaderProgram, "model_matrix", netWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            // ------------------ CREATING ALPHABET CHARACTERS --------------------------
            // ------------------ CHARACTER H --------------------------
            // CENTER BAR
            mat4 charWorldMatrix = translate(mat4(1.0f), characterHmovement)
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // SIDE BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterHmovement) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // SIDE BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterHmovement) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CHARACTER O --------------------------
            // HORIZONTAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f))
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, -1.5f, 0.0f)) 
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.25f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.25f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CHARACTER N --------------------------
            // VERTICAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterNmovement) 
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterNmovement) 
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // DIAGONAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterNmovement) 
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
                * rotate(mat4(1.0f), radians(53.13f), vec3(1.0f, 0.0f, 0.0f)) 
                * scale(mat4(1.0f), vec3(1.0f, 4.5f, 1.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CHARACTER G --------------------------
            // VERTICAL BAR LEFT
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR RIGHT
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, -1.0f, -1.5f)) 
                * scale(mat4(1.0f), vec3(1.0f, 2.0f, 1.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL TOP BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 2.0f, 0.0f)) 
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL BOT BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, -2.0f, 0.0f)) 
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL MIDDLE BAR 3
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.0f)) 
                * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.5f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", charWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CREATING RACKET -----------------------------------------
            // ------------------ RACKET CHARACTER H --------------------------------------
            // ------------------ RACKET HANDLE -------------------------------------------
            mat4 racketHandleWorldMatrix = translate(mat4(1.0f), racketHandlePosH) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", racketHandleWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ RACKET SURFACE --------------------------------------
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", racketWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ RACKET NET --------------------------------------
            for (int i = -4; i < 5; i++) {          // VERTICAL GRID
                vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH + offset) * translate(mat4(1.0f), -1.0f * offset) 
                    * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                    * translate(mat4(1.0f), offset) 
                    * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
                SetUniformMat4(shadowShaderProgram, "model_matrix", racketWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
                vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH + offset) 
                    * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                    * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));
                SetUniformMat4(shadowShaderProgram, "model_matrix", racketWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }

            // ------------------ LOWER ARM ------------------------------------------
            mat4 lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosH)
                * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), lowArmPosOffset)
                * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", lowerArmWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ UPPER ARM ------------------------------------------
            mat4 upperArmWorldMatrix = translate(mat4(1.0f), upArmPosH) 
                * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", upperArmWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ RACKET CHARACTER O --------------------------------------
            // ------------------ RACKET HANDLE -------------------------------------------
            racketHandleWorldMatrix = translate(mat4(1.0f), racketHandlePosO)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", racketHandleWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ RACKET SURFACE --------------------------------------
            racketWorldMatrix = translate(mat4(1.0f), racketPosO)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", racketWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ RACKET NET --------------------------------------
            for (int i = -4; i < 5; i++) {          // VERTICAL GRID
                vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosO + offset) * translate(mat4(1.0f), -1.0f * offset)
                    * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                    * translate(mat4(1.0f), offset)
                    * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
                SetUniformMat4(shadowShaderProgram, "model_matrix", racketWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
                vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosO + offset)
                    * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                    * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));

                SetUniformMat4(shadowShaderProgram, "model_matrix", racketWorldMatrix);

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }

            // ------------------ LOWER ARM ------------------------------------------
            lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosO)
                * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), lowArmPosOffset)
                * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", lowerArmWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ UPPER ARM ------------------------------------------
            upperArmWorldMatrix = translate(mat4(1.0f), upArmPosO)
                * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
            SetUniformMat4(shadowShaderProgram, "model_matrix", upperArmWorldMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
        }
        // ----------------------------------------------------------------------------
        // ---------------------- RENDER SCENE AFTER SHADOW ---------------------------
        // ----------------------------------------------------------------------------
        {
            glUseProgram(shaderProgram);

            glUniform1i(shadowMapUniform, 0); 

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);
            // Bind screen as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Clear color and depth data on framebuffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // -------------------- SKYBOX  ----------------------------------
            // sky color
            tempColor[0] = 0.5f;        // Value for Red
            tempColor[1] = 0.71f;        // Value for Green
            tempColor[2] = 0.86f;        // Value for Blue
            mat4 skyWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 20.0f, 0.0f)) 
                * scale(mat4(1.0f), vec3(100.0f, 100.0f, 95.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", skyWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 3); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ---------------------------------------------------------------------
            // -------------------------- CLAY GROUND ------------------------------
            // ---------------------------------------------------------------------
            
            tempColor[0] = 0.4f;        // Value for Red
            tempColor[1] = 0.6f;        // Value for Green
            tempColor[2] = 0.4f;        // Value for Blue
            mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.26f, 0.0f)) 
                * scale(mat4(1.0f), vec3(78.0f, 0.01f, 36.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", groundWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // --------------------------------------------------------------------------------------
            //  ----------------------- Draw Grid 100x100 -------------------------------------------
            // --------------------------------------------------------------------------------------
            tempColor[0] = 1.0f;        // Value for Red
            tempColor[1] = 1.0f;        // Value for Green
            tempColor[2] = 1.0f;        // Value for Blue
            for(float x = -39; x < 39; x++) {
                mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(x, -0.24f, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.05f, 0.05f, 36.0f));
                SetUniformMat4(shaderProgram, "worldMatrix", gridXWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for(float x = -18; x < 18; x++) {
                mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, x)) 
                * scale(mat4(1.0f), vec3(78.0f, 0.05f, 0.05f));
                SetUniformMat4(shaderProgram, "worldMatrix", gridZWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }

            // --------------------------------------------------------------------------------------
            // -------------------- COORDINATE AXIS -------------------------------------------------
            // --------------------------------------------------------------------------------------
            tempColor[0] = 1.0f;        // Value for Red
            tempColor[1] = 1.0f;        // Value for Green
            tempColor[2] = 1.0f;        // Value for Blue
            // THIS IS 1 UNIT 
            mat4 middleWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 0.0f, -25.0f)) 
            * scale(mat4(1.0f), vec3(0.501f, 0.501f, 0.501f));
            SetUniformMat4(shaderProgram, "worldMatrix", middleWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0      
            glBindVertexArray(0);

            // -------------------- X AXIS -------------------------------------------
            tempColor[0] = 1.0f;        // Value for Red
            tempColor[1] = 0.0f;        // Value for Green
            tempColor[2] = 0.0f;        // Value for Blue
            mat4 gridXWorldMatrix = translate(mat4(1.0f), vec3(7.5f, 0.0f, -25.0f)) 
            * scale(mat4(1.0f), vec3(5.0f, 0.5f, 0.5f));
            SetUniformMat4(shaderProgram, "worldMatrix", gridXWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0      
            glBindVertexArray(0);
            // -------------------- Z AXIS ----------------------------------------------------------
            tempColor[0] = 0.0f;        // Value for Red
            tempColor[1] = 1.0f;        // Value for Green
            tempColor[2] = 0.0f;        // Value for Blue
            mat4 gridZWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 0.0f, -22.5f))
            * scale(mat4(1.0f), vec3(0.5f, 0.5f, 5.0f));

            SetUniformMat4(shaderProgram, "worldMatrix", gridZWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0      
            glBindVertexArray(0);
            // -------------------- Y AXIS ----------------------------------------------------------
            tempColor[0] = 0.0f;        // Value for Red
            tempColor[1] = 0.0f;        // Value for Green
            tempColor[2] = 1.0f;        // Value for Blue
            mat4 gridYWorldMatrix = translate(mat4(1.0f), vec3(5.0f, 2.5f, -25.0f)) 
            * scale(mat4(1.0f), vec3(0.5f, 5.0f, 0.5f));
            SetUniformMat4(shaderProgram, "worldMatrix", gridYWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0      
            glBindVertexArray(0);

            // ------------------ CREATING TENNIS NET -------------------------------------
            // ------------------ SIDE POLES ----------------------------------------------
            tempColor[0] = 1.0f;        // Value for Red
            tempColor[1] = 1.0f;        // Value for Green
            tempColor[2] = 1.0f;        // Value for Blue
            mat4 polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, 17.51f)) 
                * scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", polesWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, -17.51f)) 
                * scale(mat4(1.0f), vec3(1.0f, 4.9f, 1.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", polesWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }                 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            polesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.25f, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.5f, 4.5f, 0.5f));
            SetUniformMat4(shaderProgram, "worldMatrix", polesWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ WHITE NET ON TOP ----------------------------------------
            tempColor[0] = 1.0f;        // Value for Red
            tempColor[1] = 1.0f;        // Value for Green
            tempColor[2] = 1.0f;        // Value for Blue
            mat4 netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 4.5f, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.055f, 0.55f, 34.5f));
            SetUniformMat4(shaderProgram, "worldMatrix", netWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ BLACK NET (MESH) ----------------------------------------
            tempColor[0] = 0.0f;        // Value for Red
            tempColor[1] = 0.0f;        // Value for Green
            tempColor[2] = 0.0f;        // Value for Blue
            for (int i = 0; i < 17; i++) {  // HORIZONTAL STRIPES
                netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, float(i)/4 + 0.1f , 0.0f)) 
                    * scale(mat4(1.0f), vec3(0.05f, 0.1f, 34.5f));
                SetUniformMat4(shaderProgram, "worldMatrix", netWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for (int i = -69; i < 69; i++) {  // VERTICAL STRIPES
                netWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.225f, float(i)/4 + 0.1f)) 
                    * scale(mat4(1.0f), vec3(0.05f, 4.45f, 0.1f));
                SetUniformMat4(shaderProgram, "worldMatrix", netWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1
               
                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }

            // ------------------ CREATING ALPHABET CHARACTERS --------------------------
            // ------------------ CHARACTER H --------------------------
            // CENTER BAR
            tempColor[0] = 1.0f;        // Value for Red
            tempColor[1] = 1.0f;        // Value for Green
            tempColor[2] = 1.0f;        // Value for Blue
            mat4 charWorldMatrix = translate(mat4(1.0f), characterHmovement)
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 7); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // SIDE BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterHmovement) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 7); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // SIDE BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterHmovement) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 7); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CHARACTER O --------------------------
            // HORIZONTAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f))
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 8); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, -1.5f, 0.0f)) 
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 8); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.25f)) 
                * scale(mat4(1.0f), characterVerticalSize - vec3(0.0f, 1.0f, 0.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 8); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterOmovement) 
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.25f)) 
                * scale(mat4(1.0f), characterVerticalSize - vec3(0.0f, 1.0f, 0.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 8); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CHARACTER N --------------------------
            // VERTICAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterNmovement) 
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 9); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterNmovement) 
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 9); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // DIAGONAL BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterNmovement) 
                * rotate(mat4(1.0f), radians(racketNRotationY), vec3(0.0f, 1.0f, 0.0f))
                * rotate(mat4(1.0f), radians(53.13f), vec3(1.0f, 0.0f, 0.0f)) 
                * scale(mat4(1.0f), vec3(1.0f, 4.5f, 1.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 9); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ CHARACTER G --------------------------
            // VERTICAL BAR LEFT
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, 1.5f)) 
                * scale(mat4(1.0f), characterVerticalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 10); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            }  
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // VERTICAL BAR RIGHT
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, -1.0f, -1.5f)) 
                * scale(mat4(1.0f), vec3(1.0f, 2.0f, 1.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 10); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL TOP BAR 1
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 2.0f, 0.0f)) 
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 10); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL BOT BAR 2
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, -2.0f, 0.0f)) 
                * scale(mat4(1.0f), characterHorizontalSize);
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 10); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // HORIZONTAL MIDDLE BAR 3
            charWorldMatrix = translate(mat4(1.0f), characterGmovement) 
                * rotate(mat4(1.0f), radians(racketGRotationY), vec3(0.0f, 1.0f, 0.0f))
                * translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.0f)) 
                * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.5f));
            SetUniformMat4(shaderProgram, "worldMatrix", charWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 10); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            
            // ------------------ CREATING RACKET -----------------------------------------
            // ------------------ RACKET CHARACTER H --------------------------------------
            // ------------------ RACKET HANDLE -------------------------------------------
            tempColor = vec3(1.0f, 1.0f, 1.0f);
            mat4 racketHandleWorldMatrix = translate(mat4(1.0f), racketHandlePosH) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
            SetUniformMat4(shaderProgram, "worldMatrix", racketHandleWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 4); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ RACKET SURFACE --------------------------------------
            mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH) 
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
            SetUniformMat4(shaderProgram, "worldMatrix", racketWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 4); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
            // ------------------ RACKET NET --------------------------------------
            tempColor = vec3(0.3f, 1.0f, 0.9f);
            for (int i = -4; i < 5; i++) {          // VERTICAL GRID
                vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH + offset) * translate(mat4(1.0f), -1.0f * offset) 
                    * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                    * translate(mat4(1.0f), offset) 
                    * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
                SetUniformMat4(shaderProgram, "worldMatrix", racketWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
                vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosH + offset) 
                    * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f)) 
                    * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));
                SetUniformMat4(shaderProgram, "worldMatrix", racketWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }

            // ------------------ LOWER ARM ------------------------------------------
            tempColor = vec3(0.6f, 0.5f, 0.4f);
            mat4 lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosH)
                * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), lowArmPosOffset)
                * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
            SetUniformMat4(shaderProgram, "worldMatrix", lowerArmWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 5); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ UPPER ARM ------------------------------------------
            tempColor = vec3(0.7f, 0.6f, 0.5f);
            mat4 upperArmWorldMatrix = translate(mat4(1.0f), upArmPosH) 
                * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(racketHRotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", upperArmWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 6); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ RACKET CHARACTER O --------------------------------------
            // ------------------ RACKET HANDLE -------------------------------------------
            tempColor = vec3(1.0f, 1.0f, 1.0f);
            racketHandleWorldMatrix = translate(mat4(1.0f), racketHandlePosO)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * scale(mat4(1.0f), vec3(0.45f, 3.0f, 0.45f));
            SetUniformMat4(shaderProgram, "worldMatrix", racketHandleWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 4); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ RACKET SURFACE --------------------------------------
            racketWorldMatrix = translate(mat4(1.0f), racketPosO)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * scale(mat4(1.0f), vec3(0.5f, 3.75f, 2.25f));
            SetUniformMat4(shaderProgram, "worldMatrix", racketWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 4); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ RACKET NET --------------------------------------
            tempColor = vec3(0.3f, 1.0f, 0.9f);
            for (int i = -4; i < 5; i++) {          // VERTICAL GRID
                vec3 offset = vec3(0.0f, 0.0f, i * 0.2f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosO + offset) * translate(mat4(1.0f), -1.0f * offset)
                    * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                    * translate(mat4(1.0f), offset)
                    * scale(mat4(1.0f), vec3(0.55f, 3.25f, 0.05f));
                SetUniformMat4(shaderProgram, "worldMatrix", racketWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }
            for (int i = -8; i < 9; i++) {          // HORIZONTAL GRID
                vec3 offset = vec3(0.0f, i * 0.2f, 0.0f);
                mat4 racketWorldMatrix = translate(mat4(1.0f), racketPosO + offset)
                    * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                    * scale(mat4(1.0f), vec3(0.55f, 0.05f, 1.75f));
                SetUniformMat4(shaderProgram, "worldMatrix", racketWorldMatrix);
                SetUniformVec3(shaderProgram, "customColor", tempColor);
                glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

                glBindVertexArray(vao);
                glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
                glBindVertexArray(0);
            }

            // ------------------ LOWER ARM ------------------------------------------
            tempColor = vec3(0.6f, 0.5f, 0.4f);
            lowerArmWorldMatrix = translate(mat4(1.0f), lowArmPosO)
                * translate(mat4(1.0f), -1.0f * lowArmPosOffset)
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), lowArmPosOffset)
                * rotate(mat4(1.0f), radians(-25.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.6f, 2.f, 0.6f));
            SetUniformMat4(shaderProgram, "worldMatrix", lowerArmWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 6); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // ------------------ UPPER ARM ------------------------------------------
            tempColor = vec3(0.7f, 0.6f, 0.5f);
            upperArmWorldMatrix = translate(mat4(1.0f), upArmPosO)
                * translate(mat4(1.0f), -1.0f * (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(racketORotationY), vec3(0.0f, 1.0, 0.0f))
                * translate(mat4(1.0f), (upArmPosOffset + lowArmPosOffset))
                * rotate(mat4(1.0f), radians(20.0f), vec3(1.0f, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(0.7f, 0.7f, 2.0f));
            SetUniformMat4(shaderProgram, "worldMatrix", upperArmWorldMatrix);
            SetUniformVec3(shaderProgram, "customColor", tempColor);
            if(toggleTexture) {
                glUniform1i(texture1Uniform, 5); // Texture unit 2 is now bound to texture1
            } else {
                glUniform1i(texture1Uniform, 1); // Texture unit 3 is now bound to texture1
            } 
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
        }

        // ----------------------------------------------------------------------------

         // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;
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
            setCam1();
            cameraAngleX = 0.0f;
            cameraAngleX1 = 0.0f;

        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { // GOES TO CHARACTER O
            setCam2();
            cameraAngleX = 0;
            cameraAngleX1 = 0.0f;

        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { // GOES TO CHARACTER G
            setCam3();
            cameraAngleX = 0;
            cameraAngleX1 = 0.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) { // GOES TO CHARACTER G
            if(!mPressed) {
                if(currentCam == 3) {
                    setCam1();
                } else if(currentCam == 1) {
                    setCam2();
                } else {
                    setCam3();
                }
                mPressed = true;
            }
            cameraAngleX = 0;
            cameraAngleX1 = 0.0f;
        }
         if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
            mPressed = false;
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
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) { // CAMERA FOLLOW MOVEMENTS OF RACKET
            if (!yPressed) {
                setCam3();
                toggleCircle = !toggleCircle;
                yPressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
            yPressed = false;
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
            *currentRacket1RotationY += 3.0;
            *currentRacket2RotationY += 3.0;
            if(currentCam != 3) 
                cameraAngleX1 -= 3.0f;

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
            *currentRacket1RotationY  -= 3.0;
            *currentRacket2RotationY -= 3.0;
            if(currentCam != 3) 
                cameraAngleX1 += 3.0f;

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
        // ------------------------------ CHANGE WORLD ORIENTATION ------------------------------
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  // move camera to the left
            if(toggleCircle) {
                spinAngle--;
                cameraPosition = vec3(-40.0f * cos(radians(spinAngle)), 25.0f, 40.0f * sin(radians(spinAngle)));
            } else {
                cameraPosition -= cameraSideVector * currentCameraSpeed * dt;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { // move camera to the right
            if(toggleCircle) {
                spinAngle++;
                cameraPosition = vec3(-40.0f * cos(radians(spinAngle)), 25.0f, 40.0f * sin(radians(spinAngle)));

            } else {
                cameraPosition += cameraSideVector * currentCameraSpeed * dt;
            }
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


        // ------------------- RENDERING MODE ---------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { // RENDER TRIANGLES   
            renderingMode = GL_TRIANGLES;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { // RENDER LINES
            renderingMode = GL_LINES;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { // RENDER POINTS
            renderingMode = GL_POINTS;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) { // TOGGLE TEXTURE
            if(!xPressed) {
                toggleTexture = !toggleTexture;
                xPressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
                xPressed = false;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) { // TOGGLE TEXTURE
            if(!bPressed) {
                toggleShadow = !toggleShadow;
                SetUniform1Value(shaderProgram, "shadowToggle", toggleShadow);
                bPressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
                bPressed = false;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { // TOGGLE LIGHT
            if(!lPressed) {
                toggleLight = !toggleLight;
                SetUniform1Value(shaderProgram, "lightToggle", toggleLight);
                lPressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {   // check for release so it doesn't do it constantly
                lPressed = false;
        }

        // --------------------- CAMERA PAN AND TILT  -------------------------------------------
        // theta = radians(cameraHorizontalAngle);
        // phi = radians(cameraVerticalAngle);
        // cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
        cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        // --------------------- ZOOM IN AND ZOOM OUT -------------------------------------------
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
                                             1024.0f / 768.0f,  // aspect ratio
                                             0.01f, 100.0f);   // near and far (near > 0)
    

            setProjectionMatrix(shaderProgram, projectionMatrix);
        }

        mat4 viewMatrix = mat4(1.0);
        viewMatrix = rotate(mat4(1.0f), radians(cameraAngleX1), vec3(0.0f, 1.0f, 0.0f)) * lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp) * rotate(mat4(1.0f), radians(cameraAngleX), vec3(0.0f, 1.0f, 0.0f)) *  rotate(mat4(1.0f), radians(cameraAngleY), vec3(1.0f, 0.0f, 0.0f));

        setViewMatrix(shaderProgram, viewMatrix);
    }
    
    // Shutdown GLFW
    glfwTerminate();
    
    return 0;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix) {
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
    glUseProgram(0);

}
void setViewMatrix(int shaderProgram, mat4 viewMatrix) {
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glUseProgram(0);

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

int createCubeVAO() {
    // Cube model
    const TexturedColoredVertex vertexArray[] = {  // position,                            normals
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), //left - red
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)), // right - purple
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top - yellow
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
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


    glVertexAttribPointer(1,                            // attribute 1 matches aNormals in Vertex Shader
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

void setCam1() {
    currentCharControl = &racketHandlePosH;
    currentRacket1RotationY = &racketHRotationY;
    currentRacket2RotationY = &racketGRotationY;
    reversed = true; 
    // MOVING CAMERA TO BEHIND THE CHARACTER
    cameraPosition = racketHandlePosH + vec3(-1.0f, 7.0f, 0.0f);
    cameraHorizontalAngle = 0.0f;
    cameraVerticalAngle = -10.0f;
    currentCam = 1;
}
void setCam2() {
    currentCharControl = &racketHandlePosO;
    currentRacket1RotationY = &racketORotationY;
    currentRacket2RotationY = &racketNRotationY;

    reversed = false;
    cameraPosition = racketHandlePosO + vec3(1.0f, 7.0f, 0.0f);
    cameraHorizontalAngle = 180.0f;
    cameraVerticalAngle = -10.0f;
    currentCam = 2;
}
void setCam3() {
    cameraPosition = vec3(0.0f, 25.0f, 45.0f);
    cameraHorizontalAngle = 90.0f;
    cameraVerticalAngle = -30.0f;
    currentCam = 3;
}
