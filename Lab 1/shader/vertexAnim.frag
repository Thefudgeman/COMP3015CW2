#version 460

in vec3 Normal;
in vec4 Position;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;

uniform float EdgeThreshold;
uniform int Pass;
uniform bool edgeDetection;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

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





void main() 
{

        if(texture(Tex1, TexCoord).a < 0.15)
        {
            discard;
        }

       FragColor = vec4(blinnPhongSpot(Position.xyz,normalize(Normal)), 1.0);
}

