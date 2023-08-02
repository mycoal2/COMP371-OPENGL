//
// COMP 371 Lab 8 - Shadow Mapping
//
// Created by Simon Demeule on 10/11/2019.
//

#include <iostream>
#include <algorithm>
#include <vector>


#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>

#include "../include/shaderloader.h"
#include "../include/OBJloader.h"  //For loading .obj files
#include "../include/OBJloaderV2.h"  //For loading .obj files using a polygon list format
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

using namespace glm;
using namespace std;

// window dimensions
const GLuint WIDTH = 1024, HEIGHT = 768;

GLuint setupModelVBO(string path, int& vertexCount);

//Sets up a model using an Element Buffer Object to refer to vertex data
GLuint setupModelEBO(string path, int& vertexCount);
GLuint setupModelEBO1(int& vertexCount, vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> UVs, vector<int> vertexIndices) {
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

// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value)
{
  glUseProgram(shader_id);
  glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value)
{
  glUseProgram(shader_id);
  glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));
}

template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
  glUseProgram(shader_id);
  glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
  glUseProgram(0);
}
GLFWwindow* window = nullptr;
bool InitContext();

int main(int argc, char* argv[])
{
  if (!InitContext()) return -1;

  // Black background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  std::string shaderPathPrefix = "../assets/shaders/";
  
  GLuint shaderScene = loadSHADER(shaderPathPrefix + "scene_vertex.glsl", shaderPathPrefix + "scene_fragment.glsl");

  GLuint shaderShadow = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl");

  GLuint tennisTextureID = loadTexture("../assets/textures/brick.jpg");
  GLuint defaultTextureID = loadTexture("../assets/textures/white.png");


  //Setup models
  string heraclesPath = "../assets/models/heracles.obj";
  string cubePath = "../assets/models/cube.obj";
  
  int heraclesVertices;
  GLuint heraclesVAO = setupModelEBO(heraclesPath, heraclesVertices);

  int activeVertices = heraclesVertices;
  GLuint activeVAO = heraclesVAO;

vector<int> vertexIndices;
    //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;
    createSPhere(vertices, normals, UVs, vertexIndices, 3.0f, 40, 40);
    int sphereVertices;
    GLuint sphereVAO = setupModelEBO1(sphereVertices, vertices, normals, UVs, vertexIndices);


  // Setup texture and framebuffer for creating shadow map

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

  // Camera parameters for view transform
  vec3 cameraPosition(0.6f, 1.0f, 2.0f);
  vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
  vec3 cameraUp(0.0f, 1.0f, 0.0f);

  // Other camera parameters
  float cameraSpeed = 1.0f;
  float cameraFastSpeed = 3 * cameraSpeed;
  float cameraHorizontalAngle = 90.0f;
  float cameraVerticalAngle = 0.0f;

  // For spinning model
  float spinningAngle = 0.0f;

  // Set projection matrix for shader, this won't change
  mat4 projectionMatrix = glm::perspective(70.0f,           // field of view in degrees
                                           WIDTH * 1.0f / HEIGHT, // aspect ratio
                                           0.01f, 800.0f);  // near and far (near > 0)

  // Set initial view matrix on both shaders
  mat4 viewMatrix = lookAt(cameraPosition,                // eye
                           cameraPosition + cameraLookAt, // center
                           cameraUp);                     // up

  // Set projection matrix on both shaders
  SetUniformMat4(shaderScene, "projection_matrix", projectionMatrix);

  // Set view matrix on both shaders
  SetUniformMat4(shaderScene, "view_matrix", viewMatrix);
	

  float lightAngleOuter = 30.0;
  float lightAngleInner = 20.0;
  // Set light cutoff angles on scene shader
  SetUniform1Value(shaderScene, "light_cutoff_inner", cos(radians(lightAngleInner)));
  SetUniform1Value(shaderScene, "light_cutoff_outer", cos(radians(lightAngleOuter)));

  // Set light color on scene shader
  SetUniformVec3(shaderScene, "light_color", vec3(1.0, 1.0, 1.0));

  // Set object color on scene shader
  SetUniformVec3(shaderScene, "object_color", vec3(1.0, 1.0, 1.0));

  // For frame time
  float lastFrameTime = glfwGetTime();
  int lastMouseLeftState = GLFW_RELEASE;
  double lastMousePosX, lastMousePosY;
  glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

  // Other OpenGL states to set once
  // Enable Backface culling
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

      GLint shadowMapUniform = glGetUniformLocation(shaderScene, "shadow_map");
      glUniform1i(shadowMapUniform, 0); // Texture unit 1 is now bound to texture1

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, tennisTextureID);
      GLint texture1Uniform = glGetUniformLocation(shaderScene, "textureSampler");
      glUniform1i(texture1Uniform, 1); // Texture unit 1 is now bound to texture1

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, defaultTextureID);
      GLint texture2Uniform = glGetUniformLocation(shaderScene, "textureSampler");
      glUniform1i(texture1Uniform, 2); // Texture unit 1 is now bound to texture1

    
  // Entering Main Loop
  while (!glfwWindowShouldClose(window)) {
     // Frame time calculation
    float dt = glfwGetTime() - lastFrameTime;
    lastFrameTime = glfwGetTime();


  // light parameters
  vec3 lightPosition =   vec3(0.0f,50.0f,0.0f); // the location of the light in 3D space
      // vec3(sinf(glfwGetTime() * 6.0f * 3.141592f), sinf(glfwGetTime() * 3.141592f), cosf(glfwGetTime() * 3.141592f));
  vec3 lightFocus(0.0, 0.0, -1.0);      // the point in 3D space the light "looks" at
  vec3 lightDirection = normalize(lightFocus - lightPosition);

  float lightNearPlane = 1.0f;
  float lightFarPlane = 180.0f;

  mat4 lightProjectionMatrix = frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
  //perspective(20.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
  mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
  mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

  // Set light space matrix on both shaders
  SetUniformMat4(shaderShadow, "light_view_proj_matrix", lightSpaceMatrix);
  SetUniformMat4(shaderScene, "light_view_proj_matrix", lightSpaceMatrix);

  // Set light far and near planes on scene shader
  SetUniform1Value(shaderScene, "light_near_plane", lightNearPlane);
  SetUniform1Value(shaderScene, "light_far_plane", lightFarPlane);

  // Set light position on scene shader
  SetUniformVec3(shaderScene, "light_position", lightPosition);

  // Set light direction on scene shader
  SetUniformVec3(shaderScene, "light_direction", lightDirection);

    // Spinning model rotation animation
    spinningAngle += 45.0f * dt; //This is equivalent to 45 degrees per second

    // Set model matrix and send to both shaders
    mat4 modelMatrix = //mat4(1.0f);
      glm::translate(mat4(1.0f), vec3(0.0f, 1.0f, -3.0f)) *
      glm::rotate(mat4(1.0f), radians(spinningAngle), vec3(0.0f, 1.0f, 0.0f)) *
      glm::rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) *
      glm::scale(mat4(1.0f), vec3(0.1f));


    // Set the view matrix for first person camera and send to both shaders
    mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
    SetUniformMat4(shaderScene, "view_matrix", viewMatrix);

    // Set view position on scene shader
    SetUniformVec3(shaderScene, "view_position", cameraPosition);

  	// Render shadow in 2 passes: 1- Render depth map, 2- Render scene
    // 1- Render shadow map:
    // a- use program for shadows
  	// b- resize window coordinates to fix depth map output size
  	// c- bind depth map framebuffer to output the depth values
    {
      // Use proper shader
      glUseProgram(shaderShadow);
      // Use proper image output size
      glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
      // Bind depth map texture as output framebuffer
      glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
      // Clear depth data on the framebuffer
      glClear(GL_DEPTH_BUFFER_BIT);
      // Bind geometry
      mat4 modelMatrix = //mat4(1.0f);
      glm::translate(mat4(1.0f), vec3(0.0f, 1.0f, -3.0f)) *
      glm::rotate(mat4(1.0f), radians(spinningAngle), vec3(0.0f, 1.0f, 0.0f)) *
      glm::rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) *
      glm::scale(mat4(1.0f), vec3(0.1f));
      SetUniformMat4(shaderShadow, "model_matrix", modelMatrix);

      glBindVertexArray(activeVAO);

      // Draw geometry
      glDrawElements(GL_TRIANGLES, activeVertices, GL_UNSIGNED_INT, 0);
      // Unbind geometry
      glBindVertexArray(0);
      modelMatrix = glm::translate(mat4(1.0f), vec3(0.0f, 9.0f, -3.0f))
      * scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
      SetUniformMat4(shaderShadow, "model_matrix", modelMatrix);

      glBindVertexArray(sphereVAO);
      glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);


    }

    
     //2- Render scene: a- bind the default framebuffer and b- just render like what we do normally
    {
	    // Use proper shader
	    glUseProgram(shaderScene);

      glUniform1i(shadowMapUniform, 0); 
	    // Use proper image output size
	    // Side note: we get the size from the framebuffer instead of using WIDTH and HEIGHT because of a bug with highDPI displays
	    int width, height;
	    glfwGetFramebufferSize(window, &width, &height);
	    glViewport(0, 0, width, height);
	    // Bind screen as output framebuffer
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	    // Clear color and depth data on framebuffer
	    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    // Bind geometry
	    glBindVertexArray(activeVAO);
	    // Draw geometry
      mat4 modelMatrix = //mat4(1.0f);
      glm::translate(mat4(1.0f), vec3(0.0f, 1.0f, -3.0f)) *
      glm::rotate(mat4(1.0f), radians(spinningAngle), vec3(0.0f, 1.0f, 0.0f)) *
      glm::rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) *
      glm::scale(mat4(1.0f), vec3(0.1f));
      SetUniformMat4(shaderScene, "model_matrix", modelMatrix);
      glUniform1i(texture1Uniform, 1); // Texture unit 1 is now bound to texture1

	    glDrawElements(GL_TRIANGLES, activeVertices, GL_UNSIGNED_INT, 0);
	    // Unbind geometry
	    glBindVertexArray(0);
      modelMatrix = glm::translate(mat4(1.0f), vec3(0.0f, 9.0f, -3.0f)) *
        scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
      SetUniformMat4(shaderScene, "model_matrix", modelMatrix);
      // GLint texture0Uniform = glGetUniformLocation(shaderScene, "shadow_map");
      // texture1Uniform = glGetUniformLocation(shaderScene, "textureSampler");

      // glUniform1i(texture0Uniform, 0); // Texture unit 0 is now bound to 'texture0'
      glUniform1i(texture2Uniform, 2); 

      glBindVertexArray(sphereVAO);
      glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    }
    /**/

    glfwSwapBuffers(window);
    glfwPollEvents();

    // Handle inputs
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);


    // Everything below here is the solution for Lab02 - Moving camera exercise
    // We'll change this to be a first or third person camera
    bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) ==
      GLFW_PRESS;
    float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;


    // - Calculate mouse motion dx and dy
    // - Update camera horizontal and vertical angle
    double mousePosX, mousePosY;
    glfwGetCursorPos(window, &mousePosX, &mousePosY);

    double dx = mousePosX - lastMousePosX;
    double dy = mousePosY - lastMousePosY;

    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    // Convert to spherical coordinates
    const float cameraAngularSpeed = 60.0f;
    cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
    cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

    // Clamp vertical angle to [-85, 85] degrees
    cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));

    float theta = radians(cameraHorizontalAngle);
    float phi = radians(cameraVerticalAngle);

    cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
    vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

    glm::normalize(cameraSideVector);

    // Use camera lookat and side vectors to update positions with ASDW
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      cameraPosition += cameraLookAt * dt * currentCameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      cameraPosition -= cameraLookAt * dt * currentCameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      cameraPosition += cameraSideVector * dt * currentCameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      cameraPosition -= cameraSideVector * dt * currentCameraSpeed;
    }

  }

  glfwTerminate();

  return 0;
}

GLuint setupModelVBO(string path, int& vertexCount)
{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> UVs;

  //read the vertex data from the model's OBJ file
  loadOBJ(path.c_str(), vertices, normals, UVs);

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

  glBindVertexArray(0);
  // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
  vertexCount = vertices.size();
  return VAO;
}

GLuint setupModelEBO(string path, int& vertexCount)
{
  vector<int> vertexIndices;
  //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
  vector<glm::vec3> vertices;
  vector<glm::vec3> normals;
  vector<glm::vec2> UVs;

  //read the vertices from the cube.obj file
  //We won't be needing the normals or UVs for this program
  loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

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

bool InitContext()
{
    // Initialize GLFW and OpenGL version
  glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  // On windows, we set OpenGL version to 2.1, to support more hardware
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

  // Create Window and rendering context using GLFW, resolution is 800x600
  window = glfwCreateWindow(WIDTH, HEIGHT, "Comp371 - Lab 08", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwMakeContextCurrent(window);


  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to create GLEW" << std::endl;
    glfwTerminate();
    return false;
  }

  return true;
}
