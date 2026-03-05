#include <glad/glad.h>
#include "scene.h"
#include <GLFW/glfw3.h>
#include "glutils.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

//using glm::vec3;

//Transformations
//Relative position within world space
glm::vec3 cameraPosition = glm::vec3(3.0f, 5.0f, 10.0f);
//The direction of travel
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Camera sidways rotation
float cameraYaw = -90.0f;
//Camera vertical rotation
float cameraPitch = 0.0f;
//Determines if first entry of mouse into window
bool mouseFirstEntry = true;
//Positions of camera from given last frame
float cameraLastXPos = 800.0f / 2.0f;
float cameraLastYPos = 600.0f / 2.0f;

float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

bool fogUp, fogDown, edgeDetectionDown = false;

class SceneRunner {
private:
    GLFWwindow * window;
    int fbw, fbh;
	bool debug;           // Set true to enable debug messages

public:
    SceneRunner(const std::string & windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0) : debug(true) {
        // Initialize GLFW
        if( !glfwInit() ) exit( EXIT_FAILURE );

#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        if(debug) 
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        if(samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, samples);
        }

        // Open the window
        window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, windowTitle.c_str(), NULL, NULL );
        if( ! window ) {
			std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent(window);

        // Get framebuffer size
        glfwGetFramebufferSize(window, &fbw, &fbh);

        // Load the OpenGL functions.
        if(!gladLoadGL()) { exit(-1); }

        GLUtils::dumpGLInfo();

        // Initialization
        glClearColor(0.5f,0.5f,0.5f,1.0f);
#ifndef __APPLE__
		if (debug) {
			glDebugMessageCallback(GLUtils::debugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		}
#endif
    }

    int run(Scene & scene) {
        scene.setDimensions(fbw, fbh);
        scene.initScene();
        scene.resize(fbw, fbh);

        // Enter the main loop
        mainLoop(window, scene);

#ifndef __APPLE__
		if( debug )
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

		// Close window and terminate GLFW
		glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

    static std::string parseCLArgs(int argc, char ** argv, std::map<std::string, std::string> & sceneData) {
        if( argc < 2 ) {
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        std::string recipeName = argv[1];
        auto it = sceneData.find(recipeName);
        if( it == sceneData.end() ) {
            printf("Unknown recipe: %s\n\n", recipeName.c_str());
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        return recipeName;
    }

private:
    static void printHelpInfo(const char * exeFile,  std::map<std::string, std::string> & sceneData) {
        printf("Usage: %s recipe-name\n\n", exeFile);
        printf("Recipe names: \n");
        for( auto it : sceneData ) {
            printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
        }
    }

    void mainLoop(GLFWwindow * window, Scene & scene) {
        
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetCursorPosCallback(window, mouse_callback);

        while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
            GLUtils::checkForOpenGLError(__FILE__,__LINE__);
			
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            scene.view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction

            ProcessUserInput(window, scene);
 
            scene.update(float(glfwGetTime()));
            scene.render();
            glfwSwapBuffers(window);

            glfwPollEvents();
			int state = glfwGetKey(window, GLFW_KEY_SPACE);
			if (state == GLFW_PRESS)
				scene.animate(!scene.animating());
        }
    }

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        //Initially no last positions, so sets last positions to current positions
        if (mouseFirstEntry)
        {
            cameraLastXPos = (float)xpos;
            cameraLastYPos = (float)ypos;
            mouseFirstEntry = false;
        }

        //Sets values for change in position since last frame to current frame
        float xOffset = (float)xpos - cameraLastXPos;
        float yOffset = cameraLastYPos - (float)ypos;

        //Sets last positions to current positions for next frame
        cameraLastXPos = (float)xpos;
        cameraLastYPos = (float)ypos;

        //Moderates the change in position based on sensitivity value
        const float sensitivity = 0.025f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        //Adjusts yaw & pitch values against changes in positions
        cameraYaw += xOffset;
        cameraPitch += yOffset;

        //Prevents turning up & down beyond 90 degrees to look backwards
        if (cameraPitch > 89.0f)
        {
            cameraPitch = 89.0f;
        }
        else if (cameraPitch < -89.0f)
        {
            cameraPitch = -89.0f;
        }

        //Modification of direction vector based on mouse turning
        glm::vec3 direction;
        direction.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        direction.y = sin(glm::radians(cameraPitch));
        direction.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront = normalize(direction);
    }

    void ProcessUserInput(GLFWwindow* WindowIn, Scene& scene)
    {
        //Closes window on 'exit' key press
        if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(WindowIn, true);
        }

        //Extent to which to move in one instance
        float movementSpeed = 7.0f * deltaTime;
        //WASD controls
        if (glfwGetKey(WindowIn, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            movementSpeed *= 2.0f;
        }
    //    if (glfwGetKey(WindowIn, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    //    {
    //        movementSpeed /= 2.0f;
      //  }
        if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPosition += movementSpeed * cameraFront;
        }
        if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPosition -= movementSpeed * cameraFront;
        }
        if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;
        }
        if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
        }
        if (glfwGetKey(WindowIn, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            cameraPosition += cameraUp * movementSpeed;
        }
        if (glfwGetKey(WindowIn, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            cameraPosition -= cameraUp * movementSpeed;
        }

        if (glfwGetKey(WindowIn, GLFW_KEY_MINUS) == GLFW_PRESS && !fogUp)
        {
            scene.fogScale += 0.1f;
            std::cout << scene.fogScale << std::endl;
            fogUp = true;
        }
        else if (glfwGetKey(WindowIn, GLFW_KEY_MINUS) == GLFW_RELEASE && fogUp)
        {
            fogUp = false;
        }
        if (glfwGetKey(WindowIn, GLFW_KEY_EQUAL) == GLFW_PRESS && scene.fogScale < 100.0f && scene.fogScale > 0.0f && !fogDown)
        {
            scene.fogScale -= 0.1f;
            fogDown = true;
        }
        else if (glfwGetKey(WindowIn, GLFW_KEY_EQUAL) == GLFW_RELEASE && fogDown)
        {
            fogDown = false;
        }
        if (glfwGetKey(WindowIn, GLFW_KEY_E) == GLFW_PRESS && !edgeDetectionDown)
        {
            edgeDetectionDown = true;
            scene.edgeDetection = !scene.edgeDetection;
        }
        else if (glfwGetKey(WindowIn, GLFW_KEY_E) == GLFW_RELEASE && edgeDetectionDown)
        {
            edgeDetectionDown = false;
        }
    }
};
