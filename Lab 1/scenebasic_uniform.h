#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/cube.h"
#include "helper/texture.h"
#include "helper/skybox.h"
#include <iostream>

using glm::vec3;

class SceneBasic_Uniform : public Scene
{
private:
    
    glm::mat4 rotationMatrix;
    Torus torus;
    Teapot teapot;
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    std::unique_ptr<ObjMesh> barrel, ogre;
    Cube cube;
    glm::mat4 rotateModel;
    glm::mat4 barrelModel;
    SkyBox sky;

    GLuint fsQuad, fboHandle, renderTex;

    GLuint brick = Texture::loadTexture("media/texture/brick1.jpg");
    GLuint moss = Texture::loadTexture("media/texture/moss.png");
    GLuint ogreDiffuse = Texture::loadTexture("media/texture/ogre_diffuse.png");
    GLuint ogreNorm = Texture::loadTexture("media/texture/ogre_normalmap.png");
    GLuint flower = Texture::loadTexture("media/texture/flower.png");
    GLuint fire = Texture::loadTexture("media/texture/fire.png");
    GLuint cement = Texture::loadTexture("media/texture/cement.jpg");
    GLuint barrelTex = Texture::loadTexture("media/texture/WoodenBarrel/Barrel_Lowpoly_DefaultMaterial_AlbedoTransparency.png");
    GLuint barrelNorm = Texture::loadTexture("media/texture/WoodenBarrel/Barrel_Lowpoly_DefaultMaterial_Normal.png");
    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/pisa");
    GLuint nightCubeTex = Texture::loadCubeMap("media/texture/cube/NightSky/night");
    GLuint knucklesTex = Texture::loadTexture("media/knuckles/AncientUgandan.png");


    void setupFBO();
    void pass1();
    void pass2();

    void compile();

public:
    GLSLProgram prog, mixShader, normalShader, skyBoxShader;

    SceneBasic_Uniform();
    void rotateModelMMM();
    void setMatrices();

    void setMatricesSkyBox();
  

    void initScene();
    void update( float t);
    void render();
    void resize(int, int);

    float tPrev = 0.0f;
    float angle = 0.0f;
    float rotSpeed = 0.0f;
};

#endif // SCENEBASIC_UNIFORM_H
