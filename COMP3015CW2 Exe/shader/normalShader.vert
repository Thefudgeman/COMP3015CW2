#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;


uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
}Light;

uniform vec4 LightPosition;
uniform vec3 Kd, Ld;
uniform mat4 ModelViewMatrix, MVP;
uniform mat3 NormalMatrix;

out vec3 LightIntensity;
out vec2 TexCoord;
out vec3 pos;
out vec3 normal;
out vec3 LightDir, ViewDir;


void main()
{
    
vec3 N = normalize(NormalMatrix * VertexNormal);
	vec3 T = normalize(NormalMatrix * vec3(VertexTangent.xyz));

	// Make T orthogonal to N (helps a LOT with noisy normal maps)
	T = normalize(T - N * dot(N, T));

	vec3 B = normalize(cross(N, T)) * VertexTangent.w;

	// Columns are T, B, N
	mat3 TBN = mat3(T, B, N);

vec3 Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

pos = Position;
LightDir = normalize(TBN * (Light.Position.xyz - Position));
ViewDir  = normalize(TBN * (-Position));
	TexCoord = VertexTexCoord;

    gl_Position = MVP*vec4(VertexPosition,1.0);
}

