#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
#include <sstream>
using std::cerr;
using std::endl;
#include "helper/glutils.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;




SceneBasic_Uniform::SceneBasic_Uniform() : time(0), plane(30.0f,30.0f, 10, 2), teapot(14,glm::mat4(1.0f)), torus(1.75f*0.75f, 0.75f*0.75f, 50,50), angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>()/8.0f), sky(300.0f), shadowMapWidth(512), shadowMapHeight(512)
{
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
    barrel = ObjMesh::load("media/knuckles/AncientUgandan.obj", true);
    ogre = ObjMesh::load("media/bs_ears.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    
    
    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    setupFBO();

    angle = glm::half_pi<float>();

    float x, z;
    rotateModel = mat4(1.0f);
    rotateModel = glm::translate(rotateModel, vec3(0.0f, 0.26f, 0.0f));


    GLuint programHandle = prog.getHandle();
    pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
    pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

    shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.0f, 0.5f, 0.5f, 1.0f)
    );

    float c = 1.65f;
    vec3 lightPos = vec3(15.0f, c*5.25f, c*7.5f);
    lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);
    lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

    prog.setUniform("Light.Intensity", vec3(0.85f));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTex);



    prog.use();
    prog.setUniform("Light.L", vec3(1.0f, 0.3f, 0.8f));
    prog.setUniform("Light.La", vec3(0.2f));

    prog.setUniform("EdgeThreshold", 0.05f);


    prog.setUniform("lights[1].L", vec3(1.0f, 1.0f, 1.0f) / 2.0f);
    prog.setUniform("lights[2].L", vec3(1.0f, 1.0f, 1.0f) / 2.0f);

    prog.setUniform("Spot.L", vec3(0.9f));
    prog.setUniform("Spot.La", vec3(0.5f));
    prog.setUniform("Spot.Exponent", 10.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(30.0f));

    prog.setUniform("Fog.MaxDist", 30.0f);
    prog.setUniform("Fog.MinDist", 1.0f);
    prog.setUniform("Fog.Colour", vec3(0.5f,0.5f,0.5f));

    skyBoxShader.use();
    skyBoxShader.setUniform("Fog.MaxDist", 30.0f);
    skyBoxShader.setUniform("Fog.MinDist", 0.1f);
    skyBoxShader.setUniform("Fog.Colour", vec3(0.5f, 0.5f, 0.5f));

    animShader.use();
    animShader.setUniform("Spot.L", vec3(0.9f));
    animShader.setUniform("Spot.La", vec3(0.5f));
    animShader.setUniform("Spot.Exponent", 10.0f);
    animShader.setUniform("Spot.Cutoff", glm::radians(30.0f));

}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/CW2vert.vert");
        prog.compileShader("shader/CW2frag.frag");
        prog.link();
        prog.use();

        skyBoxShader.compileShader("shader/skybox.vert");
        skyBoxShader.compileShader("shader/skybox.frag");
        skyBoxShader.link();
        skyBoxShader.use();

        animShader.compileShader("shader/vertexAnim.vert");
        animShader.compileShader("shader/vertexAnim.frag");
        animShader.link();
        animShader.use();

        solidShader.compileShader("shader/solid.vert");
        solidShader.compileShader("shader/solid.frag");
        solidShader.link();
        solidShader.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    //update your angle here

    float deltaT = t - tPrev;
    if (tPrev == 0.0f)
    {
        deltaT = 0.0f;
    }

    tPrev = t;
    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>())
    {
        angle -= glm::two_pi<float>();
    }

    time = t;

    rotateModel = glm::rotate(rotateModel, glm::radians(-1.0f), vec3(1.0f,0.0f,0.0f));
    barrelModel = glm::rotate(barrelModel, glm::radians(-0.3f), vec3(0.0f, 1.0f, 0.0f));

}

void SceneBasic_Uniform::render()
{
    prog.use();
    view = lightFrustum.getViewMatrix();
    projection = lightFrustum.getProjectionMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    drawScene();
    glCullFace(GL_BACK);
    glFlush();

    float c = 2.0f;
    prog.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0f));
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 600.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
    drawScene();

    solidShader.use();
    solidShader.setUniform("Color", vec4(1.0f, 0.0f,0.0f, 1.0f));
    mat4 mv = view * lightFrustum.getInverseViewMatrix();
    solidShader.setUniform("MVP", projection * mv);
    lightFrustum.render();
}

void SceneBasic_Uniform::drawScene()
{
    prog.use();

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    vec4 lightPos = vec4(15.0f, 1.0f, 15.0f, 1.0f);
    prog.setUniform("Spot.Position", vec3(view * lightPos));
    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 600.0f);

    glm::mat3 normalMatrix = glm::mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, knucklesTex);

    setMatrices();
    barrel->render();

   // animShader.use();
   // animShader.setUniform("Spot.Position", vec3(view * lightPos));
    //animShader.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));


    prog.use();



    prog.setUniform("Material.Kd", 1.0f, 0.4f, 0.72f);
    prog.setUniform("Material.Ks", vec3(1.0f));
    prog.setUniform("Material.Ka", vec3(0.5f));
    prog.setUniform("Material.Shinniness", 180.0f);
    prog.setUniform("Fog.MaxDist", 30.0f * fogScale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cement);


    //  rotateModel = glm::rotate(rotateModel, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
   //   rotateModel += glm::translate(rotateModel, vec3(-0.9f, 0.0f, -0.9f));
    rotateModel = glm::translate(rotateModel, vec3(-0.0f, 0.26f, -0.0f));

    rotateModelMMM();
    mesh->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, 0.0f, 2.0f));



    /*skyBoxShader.use();
    skyBoxShader.setUniform("Fog.MaxDist", 30.0f * fogScale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, nightCubeTex);

    model = mat4(1.0f);
    setMatricesSkyBox();
    sky.render();*/

   // animShader.use();

  //  animShader.setUniform("Time", time);
    model = mat4(1.0f);

    prog.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waveTex);
    model = mat4(1.0f);

    setMatrices();
    plane.render();
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view*model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("ShadowMatrix", lightPV * model);
}

void SceneBasic_Uniform::setAnimMatrices()
{
    mat4 mv = view * model;
    animShader.setUniform("ModelViewMatrix", mv);
    animShader.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    animShader.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::rotateModelMMM()
{
    mat4 mv = view * rotateModel;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("ShadowMatrix", lightPV * model);

}

void SceneBasic_Uniform::setMatricesSkyBox()
{
    mat4 mv = view * model;
    skyBoxShader.setUniform("ModelViewMatrix", mv);
    skyBoxShader.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    skyBoxShader.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 600.0f);
}

void SceneBasic_Uniform::setupFBO()
{
    GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete\n");
    }
    else
    {
        printf("Framebuffer is not complete\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

