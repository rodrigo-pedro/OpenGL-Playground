#pragma once
#include <string>
#include "mgl/mgl.hpp"

class Environment {
private:
    mgl::ShaderProgram* equiToCubemapShader;
    mgl::ShaderProgram* skyboxShader;
    mgl::ShaderProgram* irradianceShader;
    mgl::ShaderProgram* prefilterShader;
    mgl::ShaderProgram* brdfShader;
    mgl::TextureHDRi hdrTexture;
    mgl::Mesh* cubeMesh;
    GLuint envCubemap = 0;
    GLuint irradianceMap = 0;
    GLuint prefilterMap = 0;
    GLuint captureFBO;
    GLuint captureRBO;
    GLuint brdfLUTTexture;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    glm::mat4 captureProjection;
    std::vector<glm::mat4> captureViews;

    int cubemapFaceSize = 1024;
    int prefilterSize = 1024;


    void renderQuad();
public:
    Environment(mgl::Mesh* cubeMesh, mgl::ShaderProgram* equiToCubemapShader, mgl::ShaderProgram* skyboxShader, mgl::ShaderProgram* irradianceShader, mgl::ShaderProgram* prefilterShader, mgl::ShaderProgram* brdfShader);
    ~Environment();
    void loadHDRTexture(std::string path);
    void generateEnvMap();
    void generateIrradianceMap();
    void drawSkybox();
    void bindIrradianceMap(int unit);
    void unbindIrradianceMap();
    void generatePreFilterEnvMap();
    void generateBrdfLUT();
    void bindPreFilterMap(int unit);
    void unbindPreFilterMap();
    void bindBrdfLUT(int unit);
    void unbindBrdfLUT();
};
