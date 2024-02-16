#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>

#include "mgl/mgl.hpp"
#include "Entity.h"
#include "BlinnMaterial.h"
#include "CelMaterial.h"
#include "OutlineMaterial.h"
#include "PbrMaterial.h"
#include "Environment.h"
#include "Camera.h"

#include <string>
#include <limits.h>
#include "utils.h"
////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {

public:
  MyApp(GLsizei width, GLsizei height);
  void initCallback(GLFWwindow *win) override;
  void displayCallback(GLFWwindow *win, double elapsed) override;
  void windowCloseCallback(GLFWwindow *win) override;
  void windowSizeCallback(GLFWwindow *win, int width, int height) override;
  void cursorCallback(GLFWwindow *window, double xpos, double ypos) override;
  void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) override;
  void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) override;
  void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) override;
  void createLights();

private:
  mgl::ShaderProgram *PBRShader;
  mgl::ShaderProgram *EquiToCubeMapShader;
  mgl::ShaderProgram *SkyboxShader;
  mgl::ShaderProgram *IrradianceShader;
  mgl::ShaderProgram *PrefilterShader;
  mgl::ShaderProgram *BrdfShader;
  Environment* env;
  Entity root;
  std::map<std::string, mgl::Mesh*> meshes;
  std::vector<Light> lights;

  GLsizei initialWindowWidth = 800;
  GLsizei initialWindowHeight = 600;

  float lastMousePosX = 0.0f;
  float lastMousePosY = 0.0f;
  bool mouseStart = true;
  bool mouseLeftHeld = false;
  bool mouseRightHeld = false;
  bool mouseMiddleHeld = false;

  float elapsedTime = 0.0f;

  std::unique_ptr<Camera> camera;

  void createShaderPrograms();
  void createMeshes();
  void createEntities();
  void drawScene();
  void createCamera();
  void createEnvironmentMap();
};

MyApp::MyApp(GLsizei width, GLsizei height) : initialWindowWidth(width), initialWindowHeight(height) {
}

//////////////////////////////////////////////////////////////////////// SHADERs

void MyApp::createShaderPrograms() {
  // ------------------------------------------------------------------------
  // PBRShader = new mgl::ShaderProgram();
  // PBRShader->addShader(GL_VERTEX_SHADER, "pbr-vs.glsl");
  // PBRShader->addShader(GL_FRAGMENT_SHADER, "pbr-fs.glsl");

  // PBRShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
  // PBRShader->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
  // PBRShader->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
  // PBRShader->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
  // PBRShader->addAttribute(mgl::BITANGENT_ATTRIBUTE, mgl::Mesh::BITANGENT);

  // PBRShader->addUniform(mgl::MODEL_MATRIX);
  // PBRShader->addUniform(mgl::NORMAL_MATRIX);
  // PBRShader->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
  // for (int i = 0; i < MAX_LIGHTS; i++) {
  //   PBRShader->addUniform("lights[" + std::to_string(i) + "].position");
  //   PBRShader->addUniform("lights[" + std::to_string(i) + "].color");
  //   PBRShader->addUniform("lights[" + std::to_string(i) + "].attenuation");
  // }
  // PBRShader->addUniform("material.AlbedoTexture");
  // PBRShader->addUniform("material.NormalTexture");
  // // PBRShader->addUniform("material.RoughnessTexture");
  // // PBRShader->addUniform("material.MetallicTexture");
  // PBRShader->addUniform("material.MetallicRoughnessTexture");
  // // PBRShader->addUniform("material.AOTexture");
  // PBRShader->addUniform("material.albedoColor");
  // PBRShader->addUniform("material.metallic");
  // PBRShader->addUniform("material.roughness");
  // PBRShader->addUniform("material.opacity");
  // PBRShader->addUniform("irradianceMap");
  // PBRShader->addUniform("prefilterMap");
  // PBRShader->addUniform("brdfLUT");

  // PBRShader->create();

  // ------------------------------------------------------------------------
  EquiToCubeMapShader = new mgl::ShaderProgram();
  EquiToCubeMapShader->addShader(GL_VERTEX_SHADER, "equi-to-cubemap-vs.glsl");
  EquiToCubeMapShader->addShader(GL_FRAGMENT_SHADER, "equi-to-cubemap-fs.glsl");

  EquiToCubeMapShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);

  EquiToCubeMapShader->addUniform("equirectangularMap");
  EquiToCubeMapShader->addUniform("projection");
  EquiToCubeMapShader->addUniform("view");
  EquiToCubeMapShader->create();

  // ------------------------------------------------------------------------
  SkyboxShader = new mgl::ShaderProgram();
  SkyboxShader->addShader(GL_VERTEX_SHADER, "skybox-vs.glsl");
  SkyboxShader->addShader(GL_FRAGMENT_SHADER, "skybox-fs.glsl");

  SkyboxShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
  SkyboxShader->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
  SkyboxShader->addUniform("environmentMap");
  SkyboxShader->create();

  // ------------------------------------------------------------------------
  IrradianceShader = new mgl::ShaderProgram();
  IrradianceShader->addShader(GL_VERTEX_SHADER, "irradiance-vs.glsl");
  IrradianceShader->addShader(GL_FRAGMENT_SHADER, "irradiance-fs.glsl");

  IrradianceShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);

  IrradianceShader->addUniform("environmentMap");
  IrradianceShader->addUniform("projection");
  IrradianceShader->addUniform("view");
  IrradianceShader->create();

  // ------------------------------------------------------------------------
  PrefilterShader = new mgl::ShaderProgram();
  PrefilterShader->addShader(GL_VERTEX_SHADER, "prefilter-vs.glsl");
  PrefilterShader->addShader(GL_FRAGMENT_SHADER, "prefilter-fs.glsl");

  PrefilterShader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);

  PrefilterShader->addUniform("environmentMap");
  PrefilterShader->addUniform("roughness");
  PrefilterShader->addUniform("projection");
  PrefilterShader->addUniform("view");
  PrefilterShader->create();

  // ------------------------------------------------------------------------
  BrdfShader = new mgl::ShaderProgram();
  BrdfShader->addShader(GL_VERTEX_SHADER, "brdf-vs.glsl");
  BrdfShader->addShader(GL_FRAGMENT_SHADER, "brdf-fs.glsl");

  BrdfShader->create();

}

void MyApp::drawScene() {
  camera->ProcessCameraMovement(elapsedTime);
  root.draw();
  env->drawSkybox();
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::createMeshes() {
  //meshes.reserve(7);
  mgl::Mesh* mesh;
  std::string mesh_dir = "../assets/meshes/";

  std::string mesh_file = "unitcube.obj";
  std::string mesh_fullname = mesh_dir + mesh_file;

  mesh = new mgl::Mesh();
  mesh->calculateTangentSpace();
  mesh->joinIdenticalVertices();
  mesh->create(mesh_fullname);
  meshes.emplace("unitcube", mesh);
}

void MyApp::createEnvironmentMap() {
  env = new Environment(meshes["unitcube"], EquiToCubeMapShader, SkyboxShader, IrradianceShader, PrefilterShader, BrdfShader);
  // env->loadHDRTexture("../assets/hdri/clarens_midday_4k.hdr");
  // env->loadHDRTexture("../assets/hdri/small_cave_4k.hdr");
  // env->loadHDRTexture("../assets/hdri/Newport_Loft_Ref.hdr");
  env->loadHDRTexture("../assets/hdri/rotes_rathaus_4k.hdr");
  // env->loadHDRTexture("../assets/hdri/the_sky_is_on_fire_4k.hdr");

  env->generateEnvMap();
  env->generateIrradianceMap();
  env->generatePreFilterEnvMap();
  env->generateBrdfLUT();
  glViewport(0, 0, initialWindowWidth, initialWindowHeight);
}

void MyApp::createLights() {
  Light light1 = {
    glm::vec3(0.0f, 2.0f, 5.0f), //position
    glm::vec3(1.0f, 1.0f, 1.0f), //color
    glm::vec3(1.0f, 0.1f, 0.0f), //attenuation
  };

  Light light2 = {
    glm::vec3(10.0f, 1.0f, 0.0f), //position
    glm::vec3(1.0f, 1.0f, 1.0f), //color
    glm::vec3(1.0f, 0.1f, 0.0f), //attenuation
  };

  // lights.push_back(light1);
  // lights.push_back(light2);
}

void MyApp::createEntities() {
  root = Entity();
  root.setModelMatrix(Transform {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
    glm::vec3(1.0f, 1.0f, 1.0f)
  });

  // Jacket
  // auto jacket = std::make_shared<Entity>();
  // jacket->setModelMatrix(Transform {
  //   glm::vec3(2.0f, 0.0f, 0.0f),
  //   glm::angleAxis(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
  //   glm::vec3(0.2f),
  // });
  // jacket->loadModel("../assets/models/jacket/scene.gltf", env);
  // root.addChild(jacket);

  // Shield
  auto shield = std::make_shared<Entity>();
  shield->setModelMatrix(Transform{
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::angleAxis(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
      glm::vec3(0.1f),
      });
  shield->loadModel("../assets/models/shield/scene.gltf", env);
  root.addChild(shield);

  //  Revolver
  // auto revolver = std::make_shared<Entity>();
  // revolver->setModelMatrix(Transform{
  //     glm::vec3(0.0f, 0.0f, 0.0f),
  //     glm::angleAxis(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
  //     glm::vec3(2.0f),
  //     });
  // revolver->loadModel("../assets/models/revolver/scene.gltf", env);
  // root.addChild(revolver);

  //  PC
  // auto pc = std::make_shared<Entity>();
  // pc->setModelMatrix(Transform{
  //     glm::vec3(0.0f, 0.0f, 0.0f),
  //     glm::angleAxis(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
  //     glm::vec3(2.0f),
  //     });
  // pc->loadModel("../assets/models/pc/scene.gltf", env);
  // root.addChild(pc);

  // Wardrobe
  // auto wardrobe = std::make_shared<Entity>();
  // wardrobe->setModelMatrix(Transform{
  //     glm::vec3(0.0f, 0.0f, 0.0f),
  //     glm::angleAxis(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
  //     glm::vec3(2.0f),
  //     });
  // wardrobe->loadModel("../assets/models/wardrobe/scene.gltf", env);
  // root.addChild(wardrobe);

  // auto other = std::make_shared<Entity>();
  // other->setModelMatrix(Transform{
  //     glm::vec3(0.0f, 0.0f, 0.0f),
  //     glm::angleAxis(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
  //     glm::vec3(2.0f),
  //     });
  // other->preDrawCallback = [other]() {
  //   glEnable(GL_BLEND);
  //   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // };
  // other->postDrawCallback = [other]() {
  //   glDisable(GL_BLEND);
  // };
  // other->loadModel("../assets/models/other/scene.gltf", env);
  // root.addChild(other);

}

void MyApp::createCamera() {
  camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));
  camera->SetCameraViewOptionsPerspective(50.0f, 1.0f, 0.1f, 50.0f);
  camera->initUBO();

}

void MyApp::initCallback(GLFWwindow *win) {
  createMeshes();
  createCamera();
  createShaderPrograms();
  createEnvironmentMap();
  createLights();
  createEntities();
}

void MyApp::windowCloseCallback(GLFWwindow *win) {
  delete PBRShader;
  delete EquiToCubeMapShader;
  delete SkyboxShader;
  delete IrradianceShader;
  delete PrefilterShader;
  delete BrdfShader;
  delete env;

  for (auto const& mesh : meshes) {
    delete mesh.second;
  }
}

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);

  camera->windowResize(winx, winy);
}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) {
  elapsedTime = (float) elapsed;
  drawScene();
}

void MyApp::cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (mouseStart) {
    lastMousePosX = (float)xpos;
    lastMousePosY = (float)ypos;
    mouseStart = false;
  }

  float mouseDeltaX = (float)xpos - lastMousePosX;
  float mouseDeltaY = (float)ypos - lastMousePosY;
  lastMousePosX = (float)xpos;
  lastMousePosY = (float)ypos;

  if (mouseLeftHeld) {
    camera->ProcessMouseMovement(mouseDeltaX, mouseDeltaY);
  }

  if (mouseMiddleHeld) {
    float sensitivity = 0.4f;
    for (auto &children : root.children) {
      children->rotateOnWorldAxis(glm::radians(mouseDeltaX) * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
    }
  }
}

void MyApp::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    mouseLeftHeld = true;
    mouseStart = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    mouseLeftHeld = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    mouseRightHeld = true;
    mouseStart = true;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    mouseRightHeld = false;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    mouseMiddleHeld = true;
    mouseStart = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    mouseMiddleHeld = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void MyApp::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  camera->ProcessMouseScroll((float)yoffset);
}

void MyApp::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_RELEASE))
      camera->ProcessKeyboardInput(FORWARD);
  if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_RELEASE))
      camera->ProcessKeyboardInput(BACKWARD);
  if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_RELEASE))
      camera->ProcessKeyboardInput(LEFT);
  if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_RELEASE))
      camera->ProcessKeyboardInput(RIGHT);
  if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_RELEASE))
      camera->ProcessKeyboardInput(UP);
  if (key == GLFW_KEY_LEFT_CONTROL && (action == GLFW_PRESS || action == GLFW_RELEASE))
      camera->ProcessKeyboardInput(DOWN);
  if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_RELEASE))
      camera->ProcessKeyboardInput(SPEED_UP);


}

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char *argv[]) {
  mgl::Engine &engine = mgl::Engine::getInstance();
  GLsizei initialWindowWidth = 600;
  GLsizei initialWindowHeight = 600;
  engine.setApp(new MyApp(initialWindowWidth, initialWindowHeight));
  engine.setOpenGL(4, 6);
  engine.setWindow(initialWindowWidth, initialWindowHeight, "Hello", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////// END
