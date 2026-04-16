#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;


uniform vec4 LightPosition;
uniform vec3 Kd, Ld;
uniform mat4 ModelViewMatrix, MVP;
uniform mat3 NormalMatrix;

out vec3 LightIntensity;
out vec3 n, pos;
out vec2 TexCoord;


void main()
{
    
    n = normalize(NormalMatrix * VertexNormal);
    pos=(ModelViewMatrix*vec4(VertexPosition,1.0)).xyz;
    
    TexCoord = VertexTexCoord;

    gl_Position = MVP*vec4(VertexPosition,1.0);
}

