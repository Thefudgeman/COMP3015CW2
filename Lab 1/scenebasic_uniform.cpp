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
#include "helper/aabb.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;




SceneBasic_Uniform::SceneBasic_Uniform() : time(0), plane(30.0f, 30.0f, 200, 2), teapot(14, glm::mat4(1.0f)), torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50), angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), sky(300.0f), shadowMapWidth(512), shadowMapHeight(512)
{
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
    knuckles = ObjMesh::load("media/knuckles/AncientUgandan.obj", true);
    floatingIsland = ObjMesh::load("media/FloatingIsland/FloatingIsland.obj", true);
    floatingIsland->bbox.min += vec3(3.0f, -0.0f, 1.8f);//left,down,forward
    floatingIsland->bbox.max -= vec3(3.0f, -1.0f, 1.1f);//right,up,back

}

void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);


    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);


    angle = glm::half_pi<float>();
    vec4 lightPos = vec4(15.0f, 10.0f, 10.0f, 1.0f);

    lightAngle = 0.0f;
    lightRotationSpeed = 1.5f;
    prog.use();
    prog.setUniform("Light[0].L", vec3(45.0f));
    prog.setUniform("Light[0].Position", view * lightPos);

    prog.setUniform("Light[1].L", vec3(0.3f));
    prog.setUniform("Light[1].Position", view * lightPos);

    prog.setUniform("Light[2].L", vec3(45.0f));
    prog.setUniform("Light[2].Position", view * lightPos);

    animShader.use();
    animShader.setUniform("Light[0].L", vec3(45.0f));
    animShader.setUniform("Light[0].Position", view * lightPos);

    animShader.setUniform("Light[1].L", vec3(0.3f));
    animShader.setUniform("Light[1].Position", view * lightPos);

    animShader.setUniform("Light[2].L", vec3(45.0f));
    animShader.setUniform("Light[2].Position", view * lightPos);



    knuckles->position = cameraPosition;
    floatingIsland->acceleration = vec3(0);
    floatingIsland->position = vec3(0);

    mesh->position = vec3(0, 0, 0);


    float x, z;
    rotateModel = mat4(1.0f);
    rotateModel = glm::translate(rotateModel, vec3(0.0f, 0.26f, 0.0f));




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



    knuckles->velocity += gravity * deltaT;


    float newY = knuckles->position.y + knuckles->velocity.y * deltaT;



    knuckles->bbox.min = knuckles->position - knuckles->bbox.halfSize;
    knuckles->bbox.max = knuckles->position + knuckles->bbox.halfSize;

    Aabb yBBox = knuckles->bbox;
    float dy = newY - knuckles->position.y;
    yBBox.min.y += dy;
    yBBox.max.y += dy; 


    bool below = knuckles->position.y < floatingIsland->bbox.max.y && knuckles->position.y > floatingIsland->bbox.max.y -0.2f;




    if (collision(floatingIsland->bbox, yBBox) && below)
    {

        collisionTrue(floatingIsland->bbox);
    }
    else
    {
        knuckles->position.y = newY;
        knuckles->isGrounded = false;
    }
    float height = knuckles->bbox.max.y - knuckles->bbox.min.y;

    knuckles->bbox.min.y = knuckles->position.y;
    knuckles->bbox.max.y = knuckles->position.y + height;


    //  floatingIsland->bbox.min.y = floatingIsland->position.y;
    //  floatingIsland->bbox.max.y = floatingIsland->position.y + floatingIsland->bbox.max.y - floatingIsland->bbox.min.y;
    rotateModel = glm::rotate(rotateModel, glm::radians(-1.0f), vec3(1.0f, 0.0f, 0.0f));
    barrelModel = glm::rotate(barrelModel, glm::radians(-0.3f), vec3(0.0f, 1.0f, 0.0f));

}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.use();
    drawScene();

}

void SceneBasic_Uniform::drawScene()
{
    vec4 lightPos = vec4(15.0f, 1.0f, 15.0f, 1.0f);


    skyBoxShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, nightCubeTex);

    model = mat4(1.0f);
    setMatricesSkyBox();
    sky.render();




    animShader.use();

    animShader.setUniform("Time", time);



    float metalRough = 0.43f;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waveTex);
    drawFloor();

    prog.use();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cement);
    drawSpot(mesh->position, metalRough, 1, vec3(1, 0.71f, 0.29f));
    mesh->render();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, knucklesTex);
    drawSpot(knuckles->position, 0.0f, 0, vec3(1, 0.71f, 0.29f));
    // knuckles->render();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floatingIslandTex);
    drawSpot(floatingIsland->position, 0.0f, 0, vec3(1, 0.71f, 0.29f));
    floatingIsland->render();



    cameraPosition = knuckles->position;


}


void SceneBasic_Uniform::drawSpot(const vec3& pos, float rough, int metal, const vec3& colour)
{
    model = mat4(1.0f);
    prog.setUniform("Material.Rough", rough);
    prog.setUniform("Material.Metal", metal);
    prog.setUniform("Matherial.Color", colour);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));

    setMatrices();
}

void SceneBasic_Uniform::drawFloor()
{
    model = mat4(1.0f);
    animShader.setUniform("Material.Rough", 0.9f);
    animShader.setUniform("Material.Metal", 0);
    animShader.setUniform("Material.Color", vec3(0.2f));
    model = glm::translate(model, vec3(0.0f, -0.75f, 0.0f));

    setAnimMatrices();
    plane.render();
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
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

bool SceneBasic_Uniform::collision(const Aabb& a, const Aabb& b)
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

void SceneBasic_Uniform::collisionTrue(Aabb groundBbox)
{
    if (knuckles->velocity.y < 0)
    {
        float islandTop = groundBbox.max.y;

        std::cout << knuckles->velocity.y << endl;

        knuckles->position.y = islandTop;

        knuckles->velocity.y = 0;
        knuckles->isGrounded = true;
    }
    else
    {
        knuckles->isGrounded = false;
    }
}