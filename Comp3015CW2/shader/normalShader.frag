#version 460

in vec3 pos;
in vec2 TexCoord;
in vec3 LightDir, ViewDir;

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D NormalMapTex;

uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
}Light;

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shinniness;
} Material;

uniform struct SpotLightInfo
{
    vec3 Position;
    vec3 L;
    vec3 La;
    vec3 Direction;
    float Exponent;
    float Cutoff;
}Spot;

uniform struct FogInfo
{
    float MaxDist;
    float MinDist;
    vec3 Colour;
}Fog;


vec3 blinnPhong(vec3 n)
{

    vec3 diffuse = vec3(0), spec = vec3(0);
    
    vec3 texColour = texture(Tex1, TexCoord).rgb;

    vec3 ambient = Light.La * texColour;
    vec3 s = normalize(LightDir);
    float sDotN = max(dot(s, n), 0.0);

    diffuse = texColour * sDotN;

    if (sDotN > 0.0) {
	    vec3 v = normalize(ViewDir);
		vec3 h = normalize(v + s);
		spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shinniness);
	}
    return ambient + (diffuse + spec) * Light.L;

}

vec3 blinnPhongSpot(vec3 position, vec3 n)
{

   vec3 texColour = texture(Tex1, TexCoord).rgb;


    vec3 ambient = Spot.La * texColour;
    
    vec3 s = normalize(vec3(Spot.Position.xyz-position));

    vec3 diffuse = vec3(0.0f);

    vec3 spec = vec3(0.0f);

    float cosAng = dot(-s, normalize(Spot.Direction));
    float angle = acos(cosAng);
    float spotScale = 0.0;

    if (angle < Spot.Cutoff)
    {
        spotScale = pow(cosAng, Spot.Exponent);
        float sDotN = max(dot(n,s),0.0);
        diffuse = texColour*sDotN;    
        

        if(sDotN > 0.0)
        {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v+s);
            spec = Material.Ks * pow(max(dot(h,n), 0.0), Material.Shinniness);
        }
    }

    return ambient + spotScale *Spot.L*(diffuse + spec);
}



void main() {

 //   vec3 Colour = vec3(0.0f);

 //  float dist = abs(pos.z);

 //  float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist-Fog.MinDist);

  // fogFactor = clamp(fogFactor, 0.0, 1.0);

   //vec3 shadeColour =blinnPhongSpot(pos,normalize(n));

   vec3 norm = texture(NormalMapTex, TexCoord).xyz;
   norm = 2.0* norm -1.0;

  // Colour = mix(Fog.Colour, blinnPhong(normalize(norm)), fogFactor);


    FragColor = vec4(blinnPhong(normalize(norm)), 1.0);

   // FragColor = vec4(Colour, 1.0);
}
