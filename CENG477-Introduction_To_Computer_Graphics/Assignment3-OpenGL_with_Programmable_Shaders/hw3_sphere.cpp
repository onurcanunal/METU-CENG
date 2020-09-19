#include "helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"


static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idHeightMapTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;
int heightMapWidthTexture, heightMapHeightTexture;
int numberOfVertices;
glm::vec3* vertices;

GLfloat heightFactor = 0.0;

GLuint windowWidth = 1000;
GLuint windowHeight = 1000;

glm::vec3 v = glm::vec3(0.0, 0.0, 1.0);
glm::vec3 gaze = glm::vec3(0.0,-1.0,0.0);
//check
glm::vec3 u = glm::vec3(1.0, 0.0, 0.0);
glm::vec3 e;

GLuint flag = 1;
GLuint counter = 0;

glm::vec3 lightPosition;

GLfloat cameraSpeed = 0.0;

void keyboardCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //rise
      heightFactor += 0.5;
      glUniform1f(glGetUniformLocation(idProgramShader,"heightFactor"),heightFactor);
    }
    else if(key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //fall
      heightFactor -= 0.5;
      glUniform1f(glGetUniformLocation(idProgramShader,"heightFactor"),heightFactor);
    }
    // need to be controlled!!!
    else if(key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //left
      counter = (counter - 1 + 250)%250;
    }
    // need to be controlled!!!
    else if(key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //right
      counter = (counter + 1)%250;     
    }
    else if(key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //rise light
      lightPosition.y += 5;
    }
    else if(key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //fall light
      lightPosition.y -= 5;
    }
    else if(key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //forward light
      lightPosition.z += 5;
    }
    else if(key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //backward light
      lightPosition.z -= 5;
    }
    else if(key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //left light
      lightPosition.x += 5;
    }
    else if(key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //right light
      lightPosition.x -= 5;
    }

    else if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //pitch(up)
      v = glm::rotate(v,-0.05f,u);
      gaze = glm::rotate(gaze,-0.05f,u);
    }
    else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //pitch(down)
      v = glm::rotate(v,0.05f,u);
      gaze = glm::rotate(gaze,0.05f,u);
    }
    else if(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //yaw(left)
      gaze = glm::rotate(gaze,0.05f,v);
      u = glm::rotate(u,0.05f,v);
    }
    else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //yaw(right)
      gaze = glm::rotate(gaze,-0.05f,v);
      u = glm::rotate(u,-0.05f,v);
    }
    else if(key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //increase speed(0.01)
      cameraSpeed += 0.01;
    }
    else if(key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //decrease speed(-0.01)
      cameraSpeed -= 0.01;    
    }
    else if(key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //speed == 0
      cameraSpeed = 0.0;
    }
    else if(key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)){ //initial position
      v.x = 0.0; v.y = 0.0; v.z = 1.0;
      gaze.x = 0.0; gaze.y = -1.0; gaze.z = 0.0;
      u.x = 1.0; u.y = 0.0; u.z = 0.0;    
      e.x = 0.0; e.y = 600.0; e.z = 0.0;
      cameraSpeed = 0;  
      counter = 0;    
    }
    else if(key == GLFW_KEY_P && action == GLFW_PRESS && flag){ //full-screen mode
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      glfwSetWindowMonitor(win, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      glViewport(0, 0, mode->width, mode->height);
      flag = 0;

    }
    else if(key == GLFW_KEY_P && action == GLFW_PRESS && !flag){ //regular-screen mode
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      glfwSetWindowMonitor(win, NULL, 0, 0, windowWidth, windowHeight, mode->refreshRate);
      glViewport(0, 0, windowWidth, windowHeight);
      flag = 1;
    }    
}

void updateCameraSettings(){
  e = e + gaze * cameraSpeed;
  glm::mat4 modelingMatrix = glm::mat4(1.0);
  glm::mat4 viewingMatrix = glm::lookAt(e, e + gaze*0.1f, v);
  glm::mat4 perspectiveProjectionMatrix = glm::perspective(45.0, 1.0, 0.1, 1000.0);
  glm::mat4 modelingViewingProjectionMatrix = perspectiveProjectionMatrix * viewingMatrix * modelingMatrix;
  glm::mat4 normalTransformationMatrix = glm::inverseTranspose(viewingMatrix);
  glUniform3fv(glGetUniformLocation(idProgramShader, "cameraPosition"), 1, &e[0]);
  glUniformMatrix4fv(glGetUniformLocation(idProgramShader, "MV"), 1, GL_FALSE, &viewingMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(idProgramShader, "MVP"), 1, GL_FALSE, &modelingViewingProjectionMatrix[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(idProgramShader, "Z"), 1, GL_FALSE, &normalTransformationMatrix[0][0]);
}

void updateLightSetting(){
  glUniform3fv(glGetUniformLocation(idProgramShader, "lightPosition"), 1, &lightPosition[0]);
}

void updateWindow(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
  if(flag){
    windowHeight = height;
    windowWidth = width;    
  }
}

void renderFrame(){
  glEnableClientState(GL_VERTEX_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glDrawArrays(GL_TRIANGLES, 0, numberOfVertices);

  glDisableClientState(GL_VERTEX_ARRAY);
}

static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char * argv[]) {

  if (argc != 3) {
    printf("Only two texture images expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(1000, 1000, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & heightMapWidthTexture, & heightMapHeightTexture, 0);
  initTexture(argv[2], & widthTexture, & heightTexture, 1);



  glViewport(0, 0, 1000, 1000);
  updateCameraSettings();
  glfwSetWindowSizeCallback(win, updateWindow);
  glfwSetKeyCallback(win, keyboardCallback);

  e = glm::vec3(0.0, 600.0, 0.0);
  lightPosition = glm::vec3(0.0, 1600.0, 0.0);  

  glUniform1f(glGetUniformLocation(idProgramShader, "heightFactor"), heightFactor);
  glUniform1i(glGetUniformLocation(idProgramShader, "heightMapTexture"), 0);
  glUniform1i(glGetUniformLocation(idProgramShader, "rgbTexture"), 1);
  glUniform1i(glGetUniformLocation(idProgramShader, "widthTexture"), widthTexture);
  glUniform1i(glGetUniformLocation(idProgramShader, "heightTexture"), heightTexture);
  glUniform1i(glGetUniformLocation(idProgramShader, "heightMapWidthTexture"), heightMapWidthTexture);
  glUniform1i(glGetUniformLocation(idProgramShader, "heightMapHeightTexture"), heightMapHeightTexture);

  numberOfVertices = 3 * 2 * 125 * 250;
  vertices = new glm::vec3[numberOfVertices];
  int vertexIndex;
  vertexIndex = 0;
  int i, j;
  for(i=0; i<125; i++){
    for(j=0; j<250; j++){
      if(i!=0){
        vertices[vertexIndex++] = glm::vec3(j/250.0, 0.0, i/125.0);
        vertices[vertexIndex++] = glm::vec3(j/250.0, 0.0, (i+1)/125.0);
        vertices[vertexIndex++] = glm::vec3((j+1)/250.0, 0.0, i/125.0);
      }
      if(i!=249){
        vertices[vertexIndex++] = glm::vec3((j+1)/250.0, 0.0, i/125.0);
        vertices[vertexIndex++] = glm::vec3(j/250.0, 0.0, (i+1)/125.0);
        vertices[vertexIndex++] = glm::vec3((j+1)/250.0, 0.0, (i+1)/125.0);
      }
    }
  }

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(win)) {

    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUniform1i(glGetUniformLocation(idProgramShader, "counter"), counter);

    updateCameraSettings();
    updateLightSetting();
    renderFrame();

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}