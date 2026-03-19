#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;


uniform mat4 ModelViewMatrix, MVP;
uniform mat3 NormalMatrix;
uniform mat4 ShadowMatrix;

out vec3 n, pos;
out vec2 TexCoord;
out vec4 ShadowCoord;


void main()
{
    
    n = normalize(NormalMatrix * VertexNormal);
    pos=(ModelViewMatrix*vec4(VertexPosition,1.0)).xyz;
    
    TexCoord = VertexTexCoord;
    ShadowCoord = ShadowMatrix *vec4(VertexPosition,1.0);

    gl_Position = MVP*vec4(VertexPosition,1.0);
}

