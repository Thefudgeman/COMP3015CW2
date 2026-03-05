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




SceneBasic_Uniform::SceneBasic_Uniform() : plane(30.0f,30.0f, 1, 1), teapot(14,glm::mat4(1.0f)), torus(1.75f*0.75f, 0.75f*0.75f, 50,50), angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>()/8.0f), sky(300.0f)
{
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
    barrel = ObjMesh::load("media/knuckles/AncientUgandan.obj", true);
    ogre = ObjMesh::load("media/bs_ears.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

   // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    
    
    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    setupFBO();

    float x, z;
    rotateModel = mat4(1.0f);
    rotateModel = glm::translate(rotateModel, vec3(0.0f, 0.26f, 0.0f));

    GLfloat verts[] = {
        -1.0f,-1.0f,0.0f,1.0f,-1.0f,0.0f,1.0f,1.0f,0.0f,
        -1.0f,-1.0f,0.0f,1.0f,1.0f,0.0f,-1.0f,1.0f,0.0f
    };

    GLfloat tc[] = {
        0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,
        0.0f,0.0f,1.0f,1.0f,0.0f,1.0f
    };

    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);



    for (int i = 0; i < 3; i++)
    {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf(glm::two_pi<float>() / 3 * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        prog.use();
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, -1.0f));
    }
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

    rotateModel = glm::rotate(rotateModel, glm::radians(-1.0f), vec3(1.0f,0.0f,0.0f));
    barrelModel = glm::rotate(barrelModel, glm::radians(-0.3f), vec3(0.0f, 1.0f, 0.0f));

}

void SceneBasic_Uniform::render()
{

    pass1();
    glFlush();
    pass2();

}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view*model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::rotateModelMMM()
{
    mat4 mv = view * rotateModel;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
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
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO not complete: " << status << std::endl;
    }
    else
    {
        std::cout << "FBO complete: " << status << std::endl;

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void SceneBasic_Uniform::pass1()
{
    prog.use();

    prog.setUniform("Pass", 1);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    vec4 lightPos = vec4(15.0f * cos(angle), 1.0f, 15.0f * sin(angle), 1.0f);
    prog.setUniform("Spot.Position", vec3(view * lightPos));

    glm::mat3 normalMatrix = glm::mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    projection = glm::perspective(glm::radians(70.0f), (float)width / height, 0.3f, 600.0f);


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
    model = glm::translate(model, vec3(2.0f,0.0f,2.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, knucklesTex);

    setMatrices();
    barrel->render();


    skyBoxShader.use();
    skyBoxShader.setUniform("Fog.MaxDist", 30.0f * fogScale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, nightCubeTex);

    model = mat4(1.0f);
    setMatricesSkyBox();
    sky.render();


}

void SceneBasic_Uniform::pass2()
{
    prog.use();
    prog.setUniform("Pass", 2);
    if (edgeDetection)
    {
        prog.setUniform("edgeDetection", true);
    }
    else
    {
        prog.setUniform("edgeDetection", false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices();

    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
