# Visual Studio Version and Operating System
Visual Studio 2022 <br>
Windows 11

# Gameplay explanation
The goal of the game is to jump across the floating islands and climb up to reach the finish line.
The player model can be controlled with WASD to move around, space to jump and move the mouse to look around.

# How it works
The camera is attached to a model that is not rendered so that physics can be applied to the model and the camera just sets its position to the models position each update. 
The model has been given velocity and acceleration and its position is updated by multiplying its velocity by the time difference between updates.
It has a constant acceleration of -9.81 in the y axis to simulate falling.
If the player presses space to jump the player models y velocity is set to 6.0 which moves the model up but because of the constant -9.81 acceleration the player model slows down then start moving downwards.<br>

Using the AABB object created within the ObjMesh class I have added collision detection to the top of the floating island models.
The size of the collision boxes had to be changed manually as the function that was already there did not get the correct size.
The position of the player collision box has to be updated every update to match the position of the player model.
Before the player models position is updated it checks what it will be and checks for a collision.
If it will end up below the top part of the model then the player model will not move down in the y direction that update. 
Because collision is only handled for falling through the top of the model the player can still jump through the side or bottom of the floating islands. 
The trampoline works slighly differently to the floating islands as if there is a collision detected then the player is given a large boost in their y velocity.
The finish line also uses this code but if there is a collision it plays some music instead of adjusting the models velocity. <br>

IrrKlang was used to add music and sound into the game.
For the background music when the sound engine is created it is set to loop the song but for other sound effects a sound is created using the sound engine and it set to pause at the beginning.
An example of this would be the splash sound when you fall into the water.
This sound is paused but when the player models y positon is low enough there is a condition that checks if the splash sound is paused and if it is then the sound is played.
This is done so that the same sound is not played every update.
Once it is complete it is paused again so if the player falls again the sound can be played.
The footsteps sound works similarly to the splash sound but it is set to loop. It also checks for the velocity of the model to see if they are standing still or are jumpin/falling to pause the sound. <br>

Vertex animation was used on a plane to create the ocean at the bottom that the player needs to avoid falling into and PBR was implemented as an improvement for lighting over Blinn-Phong.
The lighting values are set quite high because the light source was placed far away to act like the sun does which prevents parts of the game looking way brighter than other parts.


# Repository Link
[https://github.com/Thefudgeman/COMP3015CW2](https://github.com/Thefudgeman/COMP3015CW2)

# Youtube Video Link
