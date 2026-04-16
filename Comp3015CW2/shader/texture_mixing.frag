#version 460

in vec3 n, pos;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D Tex2;

uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
}lights[3];

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


vec3 phongModel(int light, vec3 position, vec3 n)
{
    vec3 ambient = lights[light].La * Material.Ka;
    
    vec3 s = normalize(vec3(lights[light].Position.xyz-position));
    float sDotN = max(dot(n,s),0.0);
    vec3 diffuse = Material.Kd*sDotN;

    vec3 spec = vec3(0.0f);

    if(sDotN > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s,n);
        spec = Material.Ks * pow(max(dot(r,v), 0.0), Material.Shinniness);
    }

    return ambient + lights[light].L * (diffuse + spec);

}

vec3 blinnPhongSpot(vec3 position, vec3 n)
{

    vec4 tex1Colour = texture(Tex1, TexCoord);
    vec4 tex2Colour = texture(Tex2, TexCoord);

    vec4 texColour = mix(tex1Colour, tex2Colour, tex2Colour.a);

    vec3 ambient = Spot.La * texColour.rgb;
    
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
        diffuse = texColour.rgb*sDotN;    
        

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


    if(texture(Tex1, TexCoord).a < 0.15) //discarding Tex2 would cause Tex1 pixel underneath to be removed too
    {
        discard;
    }


    vec3 Colour = vec3(0.0f);

   // for (int i = 0; i<3; i++)
  //  {
    //     Colour = phongModel(0, pos, normalize(n));
 //   }

   float dist = abs(pos.z);

   float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist-Fog.MinDist);

   fogFactor = clamp(fogFactor, 0.0, 1.0);

   vec3 shadeColour =blinnPhongSpot(pos,normalize(n));

   Colour = mix(Fog.Colour, blinnPhongSpot(pos,normalize(n)), fogFactor);

  // vec3 texColour = texture(Tex1, TexCoord).rgb;

    FragColor = vec4(Colour, 1.0);
}
