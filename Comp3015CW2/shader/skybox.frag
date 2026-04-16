#version 460

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform samplerCube SkyBoxTex;


uniform struct FogInfo
{
    float MaxDist;
    float MinDist;
    vec3 Colour;
}Fog;

in vec3 pos;
in vec3 Vec;

void main() {

    if(texture(SkyBoxTex, normalize(Vec)).a < 0.15)
    {
        discard;
    }

    vec3 texColour = texture(SkyBoxTex, normalize(Vec)).rgb;
   
 //   float dist = abs(pos.z);

  //  float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist-Fog.MinDist);

    //fogFactor = clamp(fogFactor, 0.0, 1.0);

   //vec3 shadeColour =blinnPhongSpot(pos,normalize(n));

  //  vec3 Colour = mix(Fog.Colour, texColour, fogFactor);

    //    FragColor = vec4(Colour, 1.0);


    FragColor = vec4(texColour, 1.0);
}
