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

using namespace irrklang;


SceneBasic_Uniform::SceneBasic_Uniform() : time(0), plane(300.0f, 300.0f, 200, 2), teapot(14, glm::mat4(1.0f)), torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50), angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), sky(300.0f), shadowMapWidth(512), shadowMapHeight(512)
{
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
    knuckles = ObjMesh::load("media/knuckles/AncientUgandan.obj", true);
    floatingIsland = ObjMesh::load("media/FloatingIsland/FloatingIsland.obj", true);
    floatingIsland->bbox.min += vec3(3.0f, -0.0f, 1.8f);//left,down,forward
    floatingIsland->bbox.max -= vec3(3.0f, -1.0f, 1.5f);//right,up,back

    floatingIsland2 = ObjMesh::load("media/FloatingIsland/FloatingIsland.obj", true);
    floatingIsland2->bbox.min += vec3(3.0f, -0.0f, 1.8f);//left,down,forward
    floatingIsland2->bbox.max -= vec3(3.0f, -1.0f, 1.5f);//right,up,back

    floatingIsland3 = ObjMesh::load("media/FloatingIsland/FloatingIsland.obj", true);
    floatingIsland3->bbox.min += vec3(3.0f, -0.0f, 1.8f);//left,down,forward
    floatingIsland3->bbox.max -= vec3(3.0f, -1.0f, 1.5f);//right,up,back

    floatingIsland4 = ObjMesh::load("media/FloatingIsland/FloatingIsland.obj", true);
    floatingIsland4->bbox.min += vec3(3.0f, -0.0f, 1.8f);//left,down,forward
    floatingIsland4->bbox.max -= vec3(3.0f, -1.0f, 1.5f);//right,up,back

    floatingIsland5 = ObjMesh::load("media/FloatingIsland/FloatingIsland.obj", true);
    floatingIsland5->bbox.min += vec3(3.0f, -0.0f, 1.8f);//left,down,forward
    floatingIsland5->bbox.max -= vec3(3.0f, -1.0f, 1.5f);//right,up,back

    pathFloatingIsland = ObjMesh::load("media/BlankFloatingIsland/model.obj", true); //this model will be at 3x scale
    pathFloatingIsland->bbox.min -= vec3(-1.0f, -1.0f, 0.3f);//left,down,forward
    pathFloatingIsland->bbox.max += vec3(-1.0f, 2.0f, 0.3f);//right,up,back

    blankFloatingIsland = ObjMesh::load("media/Island/model.obj", true);
    blankFloatingIsland->bbox.min += vec3(110.2f, 97.0f, 112.8f);//left,down,forward
    blankFloatingIsland->bbox.max -= vec3(110.1f, 95.0f, 113.0f);//right,up,back

    blankFloatingIsland2 = ObjMesh::load("media/Island/model.obj", true);
    blankFloatingIsland2->bbox.min += vec3(110.2f, 97.0f, 112.8f);//left,down,forward
    blankFloatingIsland2->bbox.max -= vec3(110.1f, 95.0f, 113.0f);//right,up,back

    blankFloatingIsland3 = ObjMesh::load("media/Island/model.obj", true);
    blankFloatingIsland3->bbox.min += vec3(110.2f, 97.0f, 112.8f);//left,down,forward
    blankFloatingIsland3->bbox.max -= vec3(110.1f, 95.0f, 113.0f);//right,up,back

    blankFloatingIsland4 = ObjMesh::load("media/Island/model.obj", true);
    blankFloatingIsland4->bbox.min += vec3(110.2f, 97.0f, 112.8f);//left,down,forward
    blankFloatingIsland4->bbox.max -= vec3(110.1f, 95.0f, 113.0f);//right,up,back

}

void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);


    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);


    angle = glm::half_pi<float>();
    vec4 lightPos = vec4(15.0f, 100.0f, 10.0f, 1.0f);

    lightAngle = 0.0f;
    lightRotationSpeed = 1.5f;
    prog.use();
    prog.setUniform("Light[0].L", vec3(45.0f)*50.0f);
    prog.setUniform("Light[0].Position", view * lightPos);

    prog.setUniform("Light[1].L", vec3(0.3f) * 50.0f);
    prog.setUniform("Light[1].Position", view * lightPos);

    prog.setUniform("Light[2].L", vec3(45.0f) * 50.0f);
    prog.setUniform("Light[2].Position", view * lightPos);

    animShader.use();
    animShader.setUniform("Light[0].L", vec3(45.0f) * 50.0f);
    animShader.setUniform("Light[0].Position", view * lightPos);

    animShader.setUniform("Light[1].L", vec3(0.3f) * 50.0f);
    animShader.setUniform("Light[1].Position", view * lightPos);

    animShader.setUniform("Light[2].L", vec3(45.0f) * 50.0f);
    animShader.setUniform("Light[2].Position", view * lightPos);



    knuckles->position = cameraPosition;
    floatingIsland->position = vec3(0, -50, 0);
    floatingIsland->bbox.min += vec3(0, -50, 0);
    floatingIsland->bbox.max += vec3(0, -50, 0);

    floatingIsland2->position = vec3(4, -49, 0);
    floatingIsland2->bbox.min += vec3(4, -49, 0);
    floatingIsland2->bbox.max += vec3(4, -49, 0);

    floatingIsland3->position = vec3(3, -47.5, 2);
    floatingIsland3->bbox.min += vec3(3, -47.5, 2);
    floatingIsland3->bbox.max += vec3(3, -47.5, 2);

    floatingIsland4->position = vec3(5, -46, 6);
    floatingIsland4->bbox.min += vec3(5, -46, 6);
    floatingIsland4->bbox.max += vec3(5, -46, 6);

    floatingIsland5->position = vec3(2, -44.5, 4);
    floatingIsland5->bbox.min += vec3(2, -44.5, 4);
    floatingIsland5->bbox.max += vec3(2, -44.5, 4);

    pathFloatingIsland->position = vec3(-1, -44, 9);
    pathFloatingIsland->bbox.min += vec3(-1, -44, 9);
    pathFloatingIsland->bbox.max += vec3(-1, -44, 9);

    blankFloatingIsland->position = vec3(-2, -42, 15);
    blankFloatingIsland->bbox.min += vec3(-2, -42, 15);
    blankFloatingIsland->bbox.max += vec3(-2, -42, 15);

    blankFloatingIsland2->position = vec3(-4, -40.5, 18);
    blankFloatingIsland2->bbox.min += vec3(-4, -40.5, 18);
    blankFloatingIsland2->bbox.max += vec3(-4, -40.5, 18);

    blankFloatingIsland3->position = vec3(-9, -39, 18);
    blankFloatingIsland3->bbox.min += vec3(-9, -39, 18);
    blankFloatingIsland3->bbox.max += vec3(-9, -39, 18);

    blankFloatingIsland4->position = vec3(-13, -20, 18);
    blankFloatingIsland4->bbox.min += vec3(-13, -20, 18);
    blankFloatingIsland4->bbox.max += vec3(-13, -20, 18);

    mesh->position = vec3(0, 0, 0);

  //  ISoundEngine* engine = createIrrKlangDevice();
   // engine->play2D("music/Daisuke Ishiwatari, Naoki Hashimoto, Arc System Works - Smell of the Game.mp3", true);
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

    if (knuckles->position.y < -50.0f)
    {
        if (splashNoise->getIsPaused())
        {
            splashNoise->setIsPaused(false);
        }
        if (knuckles->position.y < -52)
        {
            knuckles->position = vec3(0, -48, 0);

        }
    }

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


    bool belowfloatingIsland = knuckles->position.y < floatingIsland->bbox.max.y && knuckles->position.y > floatingIsland->bbox.max.y -0.2f;

    bool belowfloatingIsland2 = knuckles->position.y < floatingIsland2->bbox.max.y && knuckles->position.y > floatingIsland2->bbox.max.y - 0.2f;

    bool belowfloatingIsland3 = knuckles->position.y < floatingIsland3->bbox.max.y && knuckles->position.y > floatingIsland3->bbox.max.y - 0.2f;

    bool belowfloatingIsland4 = knuckles->position.y < floatingIsland4->bbox.max.y && knuckles->position.y > floatingIsland4->bbox.max.y - 0.2f;

    bool belowfloatingIsland5 = knuckles->position.y < floatingIsland5->bbox.max.y && knuckles->position.y > floatingIsland5->bbox.max.y - 0.2f;

    bool belowpathFloatingIsland = knuckles->position.y < pathFloatingIsland->bbox.max.y && knuckles->position.y > pathFloatingIsland->bbox.max.y - 0.2f;
    
    bool belowblankFloatingIsland = knuckles->position.y < blankFloatingIsland->bbox.max.y && knuckles->position.y > blankFloatingIsland->bbox.max.y - 0.2f;

    bool belowblankFloatingIsland2 = knuckles->position.y < blankFloatingIsland2->bbox.max.y && knuckles->position.y > blankFloatingIsland2->bbox.max.y - 0.2f;

    bool belowblankFloatingIsland3 = knuckles->position.y < blankFloatingIsland3->bbox.max.y && knuckles->position.y > blankFloatingIsland3->bbox.max.y - 0.2f;

    bool belowblankFloatingIsland4 = knuckles->position.y < blankFloatingIsland4->bbox.max.y && knuckles->position.y > blankFloatingIsland4->bbox.max.y - 0.2f;






    if (collision(floatingIsland->bbox, yBBox) && belowfloatingIsland)
    {

        collisionTrue(floatingIsland->bbox);
        std::cout << knuckles->isGrounded << endl;

    }
    else if (collision(floatingIsland2->bbox, yBBox) && belowfloatingIsland2)
    {
        collisionTrue(floatingIsland2->bbox);
    }
    else if (collision(floatingIsland3->bbox, yBBox) && belowfloatingIsland3)
    {
        collisionTrue(floatingIsland3->bbox);
    }
    else if (collision(floatingIsland4->bbox, yBBox) && belowfloatingIsland4)
    {
        collisionTrue(floatingIsland4->bbox);
    }
    else if (collision(floatingIsland5->bbox, yBBox) && belowfloatingIsland5)
    {
        collisionTrue(floatingIsland5->bbox);
    }
    else if (collision(pathFloatingIsland->bbox, yBBox) && belowpathFloatingIsland)
    {
        collisionTrue(pathFloatingIsland->bbox);
    }
    else if (collision(blankFloatingIsland->bbox, yBBox) && belowblankFloatingIsland)
    {
        collisionTrue(blankFloatingIsland->bbox);
    }
    else if (collision(blankFloatingIsland2->bbox, yBBox) && belowblankFloatingIsland2)
    {
        collisionTrue(blankFloatingIsland2->bbox);
    }
    else if (collision(blankFloatingIsland3->bbox, yBBox) && belowblankFloatingIsland3)
    {
        collisionTrue(blankFloatingIsland3->bbox);
        knuckles->velocity = vec3(0, 20, 0);
        knuckles->jumping = true;
    }
    else if (collision(blankFloatingIsland4->bbox, yBBox) && belowblankFloatingIsland4)
    {
        collisionTrue(blankFloatingIsland4->bbox);
    }
    else
    {
        knuckles->position.y = newY;
        knuckles->isGrounded = false;
    }

    knuckles->position.z += knuckles->velocity.z * deltaT;
    knuckles->position.x += knuckles->velocity.x * deltaT;

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
    glDisable(GL_CULL_FACE);
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
    drawSpot(mesh->position, metalRough, 1, vec3(1, 0.71f, 0.29f), 1.0f);
    mesh->render();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, knucklesTex);
    drawSpot(knuckles->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 1.0f);
    // knuckles->render();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floatingIslandTex);
    drawSpot(floatingIsland->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 1.0f);
    floatingIsland->render();

    drawSpot(floatingIsland2->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 1.0f);
    floatingIsland2->render();

    drawSpot(floatingIsland3->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 1.0f);
    floatingIsland3->render();

    drawSpot(floatingIsland4->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 1.0f);
    floatingIsland4->render();

    drawSpot(floatingIsland5->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 1.0f);
    floatingIsland5->render();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pathFloatingIslandTex);
    drawSpot(pathFloatingIsland->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 3.0f);
    pathFloatingIsland->render();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blankFloatingIslandTex);
    drawSpot(blankFloatingIsland->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 0.01f);
    blankFloatingIsland->render();

    drawSpot(blankFloatingIsland2->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 0.01f);
    blankFloatingIsland2->render();

    drawSpot(blankFloatingIsland3->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 0.01f);
    blankFloatingIsland3->render();

    drawSpot(blankFloatingIsland4->position, 0.0f, 0, vec3(1, 0.71f, 0.29f), 0.01f);
    blankFloatingIsland4->render();

    cameraPosition = knuckles->position;


}


void SceneBasic_Uniform::drawSpot(const vec3& pos, float rough, int metal, const vec3& colour, float scale)
{
    model = mat4(1.0f);
    prog.setUniform("Material.Rough", rough);
    prog.setUniform("Material.Metal", metal);
    prog.setUniform("Matherial.Color", colour);
    model = glm::translate(model, pos);
    model = glm::scale(model, vec3(scale));

    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));

    setMatrices();
}

void SceneBasic_Uniform::drawFloor()
{
    model = mat4(1.0f);
    animShader.setUniform("Material.Rough", 0.9f);
    animShader.setUniform("Material.Metal", 0);
    animShader.setUniform("Material.Color", vec3(0.2f));
    model = glm::translate(model, vec3(0.0f, -50.0f, 0.0f));

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
        
        if (knuckles->jumping && knuckles->velocity.y < -7.0f)
        {
            ISoundEngine* JumpLandingSound = createIrrKlangDevice();
           JumpLandingSound->play2D("music/JumpLanding.mp3", false);
        }
        knuckles->jumping = false;
        float islandTop = groundBbox.max.y;
        knuckles->position.y = islandTop;

        knuckles->velocity.y = 0;
        knuckles->isGrounded = true;
    }
    else
    {
        knuckles->isGrounded = false;
    }
}