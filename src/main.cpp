
// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

// Linear Algebra Library
#include <Eigen/Core>

#include <fstream>

// For A x = b solversolver
#include <Eigen/Dense>

#include <Eigen/Geometry>

// IO Stream
#include <iostream>
#include <fstream>

#include <vector>

// Timer
#include <chrono>

#include <cmath>

#include <math.h>

#include "Utils.h"

#define PI 3.14159265

// TEXTURE IMAGE READER
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.h"
#include "Shader.h"
#include "Object.h"

using namespace std;
using namespace Eigen;

//----------------------------------
// VERTICES
//----------------------------------
VertexBufferObject VBO_CAT;
VertexBufferObject VBO_NUT;

// EDGES
//----------------------------------
VertexBufferObject VBO_OU;
VertexBufferObject VBO_OV;



//----------------------------------
// NORMALS
//----------------------------------
VertexBufferObject VBO_N_CAT;
VertexBufferObject VBO_N_NUT;

//----------------------------------
// TEXTURE
//----------------------------------
VertexBufferObject VBO_T_CAT;
VertexBufferObject VBO_T_NUT;


Object pearObj = Object::fromObjFile("../data/pear_obj/pear_tri.obj");
Object catObj = Object::fromObjFile("../data/treasure_chest_obj/treasure_chest_tri.obj");

//----------------------------------
// MODEL MATRIX
//----------------------------------
MatrixXf model(4,8);

//----------------------------------
// VIEW/CAMERA MATRIX
//----------------------------------
MatrixXf view(4,4);
float focal_length = 4.4;
Vector3f eye(0.0, 0.0, focal_length); //camera position/ eye position  //e
Vector3f look_at(0.0, 0.0, 0.0); //target point, where we want to look //g
Vector3f up_vec(0.0, 1.0, 0.0); //up vector //t

Vector3f lightPos(0.0, 1.0, focal_length);
//----------------------------------
// PERSPECTIVE PROJECTION MATRIX
//----------------------------------
MatrixXf projection(4,4);
MatrixXf perspective(4,4);
// FOV angle is hardcoded to 60 degrees
float theta = (PI/180) * 60;

// near and far are hardcoded
float n = -0.1;
float f = -100.;
// right and left
float r;
float l;
// top and bottom
float t;
float b;
float aspect;

MatrixXf dna_faces_ou = MatrixXf::Zero(3, pearObj.getVertices().cols());
MatrixXf dna_vertices_ou = MatrixXf::Zero(3, pearObj.getVertices().cols());

MatrixXf dna_faces_ov = MatrixXf::Zero(3, pearObj.getVertices().cols());
MatrixXf dna_vertices_ov = MatrixXf::Zero(3, pearObj.getVertices().cols());


void initialize(GLFWwindow* window)
{
  VertexArrayObject VAO;
  VAO.init();
  VAO.bind();

  // READ IN PARSED DATA
  VBO_CAT.init();
  VBO_NUT.init();
  VBO_N_CAT.init();
  VBO_N_NUT.init();
  VBO_T_CAT.init();
  VBO_T_NUT.init();
  VBO_OV.init();
  VBO_OU.init();

  // READ IN OBJ FILES

  cout << "Pear vertices: " << pearObj.getVertices().cols() <<endl;
  cout << "Pear normals: " << pearObj.getNormals().cols() <<endl;
  cout << "Pear textures: " << pearObj.getUVs().cols() <<endl;


  cout << "Cat vertices: " << catObj.getVertices().cols() << endl;
  cout << "Cat normals: " << catObj.getNormals().cols() << endl;
  cout << "Cat textures: " << catObj.getUVs().cols() << endl;

  VBO_CAT.update(catObj.getVertices());
  VBO_NUT.update(pearObj.getVertices());
  VBO_N_NUT.update(pearObj.getNormals());
  VBO_N_CAT.update(catObj.getNormals());
  VBO_T_NUT.update(pearObj.getUVs());
  VBO_T_CAT.update(catObj.getUVs());
  // READ IN NORMALS
  //------------------------------------------
  // MODEL MATRIX
  //------------------------------------------
  model <<
  1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
  0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
  0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0,
  0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0;

  // DNA
  float direction = (PI/180) * 90;
  MatrixXf rotation(4,4);
  // rotation <<
  // 1.,    0.,                  0.,                 0.,
  // 0.,    cos(direction),   sin(direction),  0.,
  // 0.,    -sin(direction),  cos(direction),  0.,
  // 0.,    0.,                  0.,                 1.;
  //
  // model.block(0,0,4,4) = model.block(0,0,4,4) * rotation;

  // CAT
  rotation <<
  cos(direction),  0.,  -sin(direction),   0.,
  0.,                 1.,  0.,                   0.,
  sin(direction),  0.,  cos(direction),    0.,
  0.,                 0.,  0.,                   1.;

  model.block(0,4,4,4) = model.block(0,4,4,4) * rotation;

  direction = (PI/180) * 15;
  rotation <<
  cos(direction),      sin(direction),  0.,  0.,
  -sin(direction),     cos(direction),  0.,  0.,
  0.,                     0.,                 1.,  0.,
  0.,                     0.,                 0.,  1.;
  model.block(0,4,4,4) = model.block(0,4,4,4) * rotation;

  //------------------------------------------
  // VIEW/CAMERA MATRIX
  //------------------------------------------
  Vector3f w = (eye - look_at).normalized();
  Vector3f u = (up_vec.cross(w).normalized());
  Vector3f v = w.cross(u);

  Matrix4f look;
  look <<
  u[0], u[1], u[2], 0.,
  v[0], v[1], v[2], 0.,
  w[0], w[1], w[2], 0.,
  0.,   0.,    0.,  0.5;

  Matrix4f at;
  at <<
  0.5, 0.0, 0.0, -eye[0],
  0.0, 0.5, 0.0, -eye[1],
  0.0, 0.0, 0.5, -eye[2],
  0.0, 0.0, 0.0, 0.5;

  view = look * at;

  //------------------------------------------
  // PROJECTION MATRIX
  //------------------------------------------
  // Get the size of the window
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  aspect = width/height;

  t = tan(theta/2) * abs(n);
  b = -t;

  r = aspect * t;
  l = -r;

  perspective <<
  2*abs(n)/(r-l), 0., (r+l)/(r-l), 0.,
  0., (2 * abs(n))/(t-b), (t+b)/(t-b), 0.,
  0., 0.,  (abs(f) + abs(n))/(abs(n) - abs(f)), (2 * abs(f) * abs(n))/(abs(n) - abs(f)),
  0., 0., -1., 0;

  projection = perspective;


}

void changeView(int direction)
{
  float factor = 0.3;

  if(direction == 0){
    cout << "Moving eye to the left" << endl;
    eye[0] -= factor;
  }else if(direction == 1){
    cout << "Moving eye to the right" << endl;
    eye[0] += factor;
  }else if(direction == 2){
    cout << "Moving eye up" << endl;
    eye[1] += factor;
  }else if(direction == 3){
    cout << "Moving eye down" << endl;
    eye[1] -= factor;
  }else if(direction == 4){
    cout << "Moving eye in" << endl;
    eye[2] -= factor;
  }else if(direction == 5){
    cout << "Moving eye out" << endl;
    eye[2] += factor;
  }
  Vector3f w = (eye - look_at).normalized();
  Vector3f u = (up_vec.cross(w).normalized());
  Vector3f v = w.cross(u);

  Matrix4f look;
  look <<
  u[0], u[1], u[2], 0.,
  v[0], v[1], v[2], 0.,
  w[0], w[1], w[2], 0.,
  0.,   0.,    0.,  0.5;

  Matrix4f at;
  at <<
  0.5, 0.0, 0.0, -eye[0],
  0.0, 0.5, 0.0, -eye[1],
  0.0, 0.0, 0.5, -eye[2],
  0.0, 0.0, 0.0, 0.5;

  view = look * at;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  float direction = (PI/180) * 20;
  MatrixXf rotation(4,4);
  if(action == GLFW_RELEASE){
    switch(key){
      // ROTATE CAT
      // rotate

      case GLFW_KEY_1:{
          pearObj.translate(Vector3f(1., 0., 0.));
        rotation <<
          cos(direction),  0.,  -sin(direction),   0.,
          0.,                 1.,  0.,                   0.,
          sin(direction),  0.,  cos(direction),    0.,
          0.,                 0.,  0.,                   1.;
          model.block(0, 4, 4, 4) = model.block(0, 4, 4, 4) * rotation;
        break;
      }
      case GLFW_KEY_2:{
        direction = - direction;
        rotation <<
          cos(direction),  0.,  -sin(direction),   0.,
          0.,                 1.,  0.,                   0.,
          sin(direction),  0.,  cos(direction),    0.,
          0.,                 0.,  0.,                   1.;
          model.block(0, 4, 4, 4) = model.block(0, 4, 4, 4) * rotation;
        break;
      }
      case GLFW_KEY_3:{
        rotation <<
          1.,    0.,                  0.,                 0.,
          0.,    cos(direction),   sin(direction),  0.,
          0.,    -sin(direction),  cos(direction),  0.,
          0.,    0.,                  0.,                 1.;
          model.block(0, 4, 4, 4) = model.block(0, 4, 4, 4) * rotation;
        break;
      }
      case GLFW_KEY_4:{
        direction = -direction;
        rotation <<
          1.,    0.,                  0.,                 0.,
          0.,    cos(direction),   sin(direction),  0.,
          0.,    -sin(direction),  cos(direction),  0.,
          0.,    0.,                  0.,                 1.;
          model.block(0, 4, 4, 4) = model.block(0, 4, 4, 4) * rotation;
        break;
      }

      // NUT
      case GLFW_KEY_5:{
        rotation <<
          cos(direction),  0.,  -sin(direction),   0.,
          0.,                 1.,  0.,                   0.,
          sin(direction),  0.,  cos(direction),    0.,
          0.,                 0.,  0.,                   1.;
          model.block(0, 0, 4, 4) = model.block(0, 0, 4, 4) * rotation;
        break;
      }
      case GLFW_KEY_6:{
        direction = - direction;
        rotation <<
          cos(direction),  0.,  -sin(direction),   0.,
          0.,                 1.,  0.,                   0.,
          sin(direction),  0.,  cos(direction),    0.,
          0.,                 0.,  0.,                   1.;
          model.block(0, 0, 4, 4) = model.block(0, 0, 4, 4) * rotation;
        break;
      }
      case GLFW_KEY_7:{
        rotation <<
          1.,    0.,                  0.,                 0.,
          0.,    cos(direction),   sin(direction),  0.,
          0.,    -sin(direction),  cos(direction),  0.,
          0.,    0.,                  0.,                 1.;
          model.block(0, 0, 4, 4) = model.block(0, 0, 4, 4) * rotation;
        break;
      }
      case GLFW_KEY_8:{
        direction = -direction;
        rotation <<
          1.,    0.,                  0.,                 0.,
          0.,    cos(direction),   sin(direction),  0.,
          0.,    -sin(direction),  cos(direction),  0.,
          0.,    0.,                  0.,                 1.;
          model.block(0, 0, 4, 4) = model.block(0, 0, 4, 4) * rotation;
        break;
      }
      case GLFW_KEY_LEFT:{
        cout << "Moving LEFT" << endl;
        changeView(0);
        break;
      }
      case GLFW_KEY_RIGHT:{
        cout << "Moving RIGHT" << endl;
        changeView(1);
        break;
      }
      case GLFW_KEY_UP:{
        cout << "Moving UP" << endl;
        changeView(2);
        break;
      }
      case GLFW_KEY_DOWN:{
        cout << "Moving DOWN" << endl;
        changeView(3);
        break;
      }
      case GLFW_KEY_EQUAL:{
        cout << "Moving IN" << endl;
        changeView(4);
        break;
      }
      case GLFW_KEY_MINUS:{
        cout << "Moving OUT" << endl;
        changeView(5);
        break;
      }

    }
  }
}

int main()
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
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
    if (!window)
    {
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
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid

    Program program;

    const GLchar* vertex_shader = Shader::loadShaderCodeAsString("../src/vertex_shader.glsl")->c_str();
    const GLchar* fragment_shader = Shader::loadShaderCodeAsString("../src/fragment_shader.glsl")->c_str();

    // INITIALIZE EVERYTHING
    initialize(window);

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader, fragment_shader,"outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader

    vector<MatrixXf> derivatives = pearObj.calculateDerivatives();
    VBO_OU.update(derivatives.at(0));
    VBO_OV.update(derivatives.at(1));
    program.bindVertexAttribArray("ov_in", VBO_OV);
    program.bindVertexAttribArray("ou_in", VBO_OU);

    // UNIFORMS
    glUniform3f(program.uniform("lightPos"), lightPos[0] ,lightPos[1], lightPos[2]);
    glUniform3f(program.uniform("viewPos"), eye[0], eye[1], eye[2]);
    glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());
    glUniformMatrix4fv(program.uniform("projection"), 1, GL_FALSE, projection.data());


    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();


    unsigned int pearTexture, pearBumpTexture, catTexture, bricksNormal, bricksDisp;

    cout << "Loading texture/bump maps" << endl;
    Texture::generateAndBindTexture(pearTexture, "../data/pear_obj/pear_texture.jpg");
    Texture::generateAndBindTexture(pearBumpTexture, "../data/hazelnut_obj/carpet_noise.jpeg");
    Texture::generateAndBindTexture(catTexture, "../data/bricks/bricks_texture.jpg");
    Texture::generateAndBindTexture(bricksNormal, "../data/bricks/bricks_normal.jpg");
    Texture::generateAndBindTexture(bricksDisp, "../data/bricks/bricks_disp.jpg");

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
      // Bind your program
      program.bind();
      // ------
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
      glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());

      //--------
      // NUT
      //--------
      program.bindVertexAttribArray("position",VBO_NUT);
      program.bindVertexAttribArray("normal",VBO_N_NUT);
      program.bindVertexAttribArray("texCoord",VBO_T_NUT);

      glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, pearObj.getModel().data());
      glUniform1i(program.uniform("isBumpMapping"), true);

      glUniform1i(program.uniform("ourTexture"), 0);
      glUniform1i(program.uniform("bump"), 2);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pearTexture); //texture
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, pearBumpTexture); //height map

      for(int i = 0; i < pearObj.getVertices().cols(); i+=3){
        glDrawArrays(GL_TRIANGLES, i , 3);
      }

      //--------
      // CAT
      //--------
      program.bindVertexAttribArray("position",VBO_CAT);
      program.bindVertexAttribArray("normal",VBO_N_CAT);
      program.bindVertexAttribArray("texCoord",VBO_T_CAT);
      glUniformMatrix4fv(program.uniform("model"), 1, GL_FALSE, model.block(0,4,4,4).data());
      glUniform1i(program.uniform("isBumpMapping"), false);

      glUniform1i(program.uniform("ourTexture"), 1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, catTexture); // cat texture

      glUniform1i(program.uniform("normalMap"), 3);
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, bricksNormal);

      glUniform1i(program.uniform("depthMap"), 4);
      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, bricksDisp);

      for(int i = 0; i < catObj.getVertices().cols(); i+=3){
          glDrawArrays(GL_TRIANGLES, i , 3);
      }
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
