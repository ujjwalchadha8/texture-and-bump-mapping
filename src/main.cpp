
// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

// Linear Algebra Library
#include <Eigen/Core>


#include <vector>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include "Texture.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

using namespace std;
using namespace Eigen;


VertexBufferObject VBO_TREASURE;
VertexBufferObject VBO_BANANA;

VertexBufferObject VBO_OU;
VertexBufferObject VBO_OV;

VertexBufferObject VBO_NORMALS_TREASURE;
VertexBufferObject VBO_NORMALS_BANANA;

VertexBufferObject VBO_TEXTURE_TREASURE;
VertexBufferObject VBO_TEXTURE_BANANA;


Object bananaObj = Object::fromObjFile("../data/objects/banana.obj");
Object treasureObj = Object::fromObjFile("../data/objects/treasure_chest_tri.obj");

Camera camera(Vector3f(0., 1.0, 2.5), Vector3f(0., 0., 0.),
              Camera::PROJECTION_PERSPECTIVE, 1, -0.5, -100.0, (3.14 / 180) * 100);
Vector3f lightPosition(0.0, 1.0, 4.4);

bool noMapping = true;

void initWorld(GLFWwindow* window) {
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    VBO_TREASURE.init();
    VBO_BANANA.init();
    VBO_NORMALS_TREASURE.init();
    VBO_NORMALS_BANANA.init();
    VBO_TEXTURE_TREASURE.init();
    VBO_TEXTURE_BANANA.init();
    VBO_OV.init();
    VBO_OU.init();

    VBO_TREASURE.update(treasureObj.getVertices());
    VBO_BANANA.update(bananaObj.getVertices());
    VBO_NORMALS_BANANA.update(bananaObj.getNormals());
    VBO_NORMALS_TREASURE.update(treasureObj.getNormals());
    VBO_TEXTURE_BANANA.update(bananaObj.getTextureCoordinates());
    VBO_TEXTURE_TREASURE.update(treasureObj.getTextureCoordinates());

    int screenWidth, screenHeight;
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    camera.setAspectRatio((float) screenWidth / (float) screenHeight);

    bananaObj.translate(Vector3f(1, 0., 0.));
    treasureObj.translate(Vector3f(-1, 0., 0.));
    treasureObj.scale(1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    camera.setAspectRatio((float)width/(float)height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_1: {
                bananaObj.translate(Vector3f(1., 0., 0.));
                break;
            }
            case GLFW_KEY_2: {

                break;
            }
            case GLFW_KEY_3: {

                break;
            }
            case GLFW_KEY_4: {

                break;
            }

            case GLFW_KEY_5: {

                break;
            }
            case GLFW_KEY_6: {

                break;
            }
            case GLFW_KEY_7: {

                break;
            }
            case GLFW_KEY_8: {

                break;
            }
            case GLFW_KEY_TAB: {
                noMapping = !noMapping;
                break;
            }
            case GLFW_KEY_LEFT: {
                camera.translateBy(Vector3f(-0.3, 0.0, 0.0));
                break;
            }
            case GLFW_KEY_RIGHT: {
                camera.translateBy(Vector3f(0.3, 0.0, 0.0));
                break;
            }
            case GLFW_KEY_UP: {
                camera.translateBy(Vector3f(0.0, 0.3, 0.0));
                break;
            }
            case GLFW_KEY_DOWN: {
                camera.translateBy(Vector3f(0.0, -0.3, 0.0));
                break;
            }
            case GLFW_KEY_EQUAL: {
                camera.translateBy(Vector3f(0.0, 0.0, -0.3));
                break;
            }
            case GLFW_KEY_MINUS: {
                camera.translateBy(Vector3f(0.0, 0.0, 0.3));
                break;
            }

        }
    }
}

int main() {
    GLFWwindow *window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate super sampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

#ifndef __APPLE__
    glewExperimental = true;
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
     fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char *) glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid

    Program program;

    const GLchar *vertex_shader = Shader::loadShaderCodeAsString("../src/vertex_shader.glsl")->c_str();
    const GLchar *fragment_shader = Shader::loadShaderCodeAsString("../src/fragment_shader.glsl")->c_str();

    // INITIALIZE EVERYTHING
    initWorld(window);

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader, fragment_shader, "outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader

    vector<MatrixXf> bananaDerivatives = bananaObj.calculateTangentBasis();
    vector<MatrixXf> treasureDerivatives = treasureObj.calculateTangentBasis();
    VBO_OU.update(bananaDerivatives.at(0));
    VBO_OV.update(bananaDerivatives.at(1));
    program.bindVertexAttribArray("ovIn", VBO_OV);
    program.bindVertexAttribArray("ouIn", VBO_OU);

    // UNIFORMS
    glUniform3f(program.uniform("lightPos"), lightPosition[0], lightPosition[1], lightPosition[2]);
    glUniform3f(program.uniform("viewPos"), camera.getCameraTarget()(0), camera.getCameraTarget()(1),
                camera.getCameraTarget()(2));
    glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, camera.getView().data());

    unsigned int bananaTexture, bananaNormalMap, treasureTexture, bricksNormalMap, bricksDepthMap;
    Texture::generateAndBindTexture(bananaTexture, "../data/textures/banana_texture.jpg");
    Texture::generateAndBindTexture(bananaNormalMap, "../data/textures/carpet_noise.jpeg");
    Texture::generateAndBindTexture(treasureTexture, "../data/textures/bricks_texture.jpg");
    Texture::generateAndBindTexture(bricksNormalMap, "../data/textures/bricks_normal.jpg");
    Texture::generateAndBindTexture(bricksDepthMap, "../data/textures/bricks_disp.jpg");

    // Register the callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Bind your program
        program.bind();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, camera.getView().data());
        glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, camera.getProjection().data());
        glUniform1i(program.uniform("noMapping"), noMapping);
        glUniform1f(program.uniform("height_scale"), 0.005);

        VBO_OU.update(bananaDerivatives.at(0));
        VBO_OV.update(bananaDerivatives.at(1));

        program.bindVertexAttribArray("position", VBO_BANANA);
        program.bindVertexAttribArray("normal", VBO_NORMALS_BANANA);
        program.bindVertexAttribArray("textureCoordinates", VBO_TEXTURE_BANANA);

        glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, bananaObj.getModel().data());
        glUniform1i(program.uniform("isBumpMapping"), true);

        glUniform1i(program.uniform("textureMap"), 0);
        glUniform1i(program.uniform("normalMap"), 2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bananaTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, bananaNormalMap);

        glDrawArrays(GL_TRIANGLES, 0, bananaObj.getVertices().cols());



        program.bindVertexAttribArray("position", VBO_TREASURE);
        program.bindVertexAttribArray("normal", VBO_NORMALS_TREASURE);
        program.bindVertexAttribArray("textureCoordinates", VBO_TEXTURE_TREASURE);

        VBO_OU.update(treasureDerivatives.at(0));
        VBO_OV.update(treasureDerivatives.at(1));
        glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, treasureObj.getModel().data());
        glUniform1i(program.uniform("isBumpMapping"), false);

        glUniform1i(program.uniform("textureMap"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, treasureTexture);

        glUniform1i(program.uniform("normalMap"), 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, bricksNormalMap);

        glUniform1i(program.uniform("depthMap"), 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, bricksDepthMap);

        glDrawArrays(GL_TRIANGLES, 0, treasureObj.getVertices().cols());

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();

    }
    // Deallocate opengl memory
    program.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
