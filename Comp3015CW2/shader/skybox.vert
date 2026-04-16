#version 460

layout (location = 0) in vec3 VertexPosition;

uniform mat4  ModelViewMatrix, MVP;

out vec3 Vec;
out vec3 pos;


void main()
{
    
    Vec = VertexPosition;
    

    pos=(ModelViewMatrix*vec4(VertexPosition,1.0)).xyz;

    gl_Position = MVP*vec4(VertexPosition,1.0);
}

