#pragma once

#include <glm/glm.hpp>
#include <iostream>
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
#include "helper/frustum.h"
class Scene
{
protected:
	glm::mat4 model, projection;

public:
    float fogScale = 1.0f;
    int width;
    int height;
    glm::mat4 view;
    glm::vec3 cameraPosition;
    std::unique_ptr<ObjMesh> knuckles;


	Scene() : m_animate(true), width(1920), height(1080) { }
	virtual ~Scene() {}

	void setDimensions( int w, int h ) {
	    width = w;
	    height = h;
	}
	
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(int, int) = 0;
    
    void animate( bool value ) { m_animate = value; }
    bool animating() { return m_animate; }
    
protected:
	bool m_animate;
};
