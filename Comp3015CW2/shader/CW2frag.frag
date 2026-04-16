#version 460

in vec3 n, pos;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;


uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
    vec3 Intensity;
}Light[3];

uniform struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shinniness;
    float Rough;
    bool Metal;
    vec3 Color;
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

const float PI = 3.14459265358979323846;



float ggxDistribution(float nDotH)
{
    float alpha2 = Material.Rough * Material.Rough * Material.Rough * Material.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

float geomSmith(float dotProd)
{
    float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
    float denom = dotProd * (1 - k) + k;
    return 1.0 / denom;
}

vec3 schlickFresnel(float lDotH)
{
    vec3 texColour = texture(Tex1, TexCoord).rgb;
    vec3 f0 = vec3(0.04);
    if(Material.Metal)
    {
        f0 = texColour;
    }
    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

vec3 microfacetModel(int lightIdx, vec3 position, vec3 n)
{
    vec3 texColour = texture(Tex1, TexCoord).rgb;

    vec3 diffuseBrdf = vec3(0.0);
    if(!Material.Metal)
    {
        diffuseBrdf = texColour;
    }

    vec3 I = vec3(0.0);
    vec3 lightI = Light[lightIdx].L;
    if(Light[lightIdx].Position.w == 0)
    {
         I = normalize(Light[lightIdx].Position.xyz);
    }
    else
    {
        I = Light[lightIdx].Position.xyz - position;
        float dist = length(I);
        I = normalize(I);
        lightI /= (dist * dist);
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v+I);
    float nDotH = dot(n,h);
    float IDotH = dot(I,h);
    float nDotL = max(dot(n,I), 0.0);
    float nDotV = dot(n,v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * schlickFresnel(IDotH) * geomSmith(nDotL) * geomSmith(nDotV);

    return (diffuseBrdf + PI * specBrdf) * lightI * nDotL;

}




//vec3 phongModel(int light, vec3 position, vec3 n)
//{
 //   vec3 ambient = Light.La * Material.Ka;
    
//    vec3 s = normalize(vec3(Light.Position.xyz-position));
//    float sDotN = max(dot(n,s),0.0);
 //   vec3 diffuse = Material.Kd*sDotN;

//    vec3 spec = vec3(0.0f);

//    if(sDotN > 0.0)
//    {
 //       vec3 v = normalize(-position.xyz);
//        vec3 r = reflect(-s,n);
//        spec = Material.Ks * pow(max(dot(r,v), 0.0), Material.Shinniness);
//    }

  //  return ambient + Light.L * (diffuse + spec);

//}

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
        float sDotN = max(dot(s,n),0.0);
        diffuse = texColour*sDotN;    
        

        if(sDotN > 0.0)
        {
            vec3 v = normalize(-position.xyz);
            vec3 r = reflect(-s, n);
            spec = Material.Ks * pow(max(dot(r,v), 0.0), Material.Shinniness);
        }
    }

    return ambient + spotScale *Spot.L*(diffuse + spec);
}


void main() {





   // for (int i = 0; i<3; i++)
  //  {
    //     Colour = phongModel(0, pos, normalize(n));
 //   }

//    if(Pass == 1)
//    {
 //       if(texture(Tex1, TexCoord).a < 0.15)
//        {
 //           discard;
//        }
 //       FragColor = pass1();
//    }
//    if(Pass == 2)
//    {
 //       FragColor = pass2();
//    }

   //      vec3 Colour = vec3(0.0f);

   //    float dist = abs(pos.z);

    //   float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist-Fog.MinDist);

    //   fogFactor = clamp(fogFactor, 0.0, 1.0);

       //vec3 shadeColour =blinnPhongSpot(pos,normalize(n));

      // Colour = mix(Fog.Colour, blinnPhongSpot(pos,normalize(n)), fogFactor);

      // vec3 texColour = texture(Tex1, TexCoord).rgb;

     //  FragColor = vec4( blinnPhongSpot(pos,normalize(n)), 1.0);


     vec3 sum = vec3(0);
     vec3 norm = normalize(n);
     for(int i = 0; i < 3; i++)
     {
        sum+= microfacetModel(i, pos, norm);
     }

     sum = pow(sum, vec3(1.0/2.2));

     FragColor = vec4(sum,1);




}

