#version 460

in vec3 n, pos;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;

uniform float EdgeThreshold;
uniform int Pass;
uniform bool edgeDetection;

layout(binding=1) uniform sampler2DShadow ShadowMap;
in vec4 ShadowCoord;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L;
    vec3 Intensity;
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


vec3 phongModelDiffAndSpec()
{
   vec3 texColour = texture(Tex1, TexCoord).rgb;

    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(-pos.xyz);
    vec3 r = reflect(-s,n);
    float sDotN = max(dot(s,n),0.0);
    vec3 diffuse = Light.Intensity * texColour * sDotN;
    vec3 spec = vec3(0.0);
    if(sDotN > 0.0)
    {
        spec = Light.Intensity * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shinniness);
    }
    return diffuse + spec;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
   vec3 texColour = texture(Tex1, TexCoord).rgb;
   
   vec3 ambient = texColour * Light.Intensity;
   vec3 diffAndSpec = phongModelDiffAndSpec();

   float shadow = 1.0f;
   if(ShadowCoord.z >= 0)
   {
        shadow = textureProj(ShadowMap, ShadowCoord); 
   }

    FragColor = vec4(diffAndSpec * shadow + ambient, 1.0);

    FragColor = pow(FragColor, vec4(1.0/2.2));
}

subroutine (RenderPassType)
void recordDepth()
{
    if(texture(Tex1, TexCoord).a < 0.15)
        discard;
    float d = gl_FragCoord.z;
}




vec3 phongModel(int light, vec3 position, vec3 n)
{
    vec3 ambient = Light.La * Material.Ka;
    
    vec3 s = normalize(vec3(Light.Position.xyz-position));
    float sDotN = max(dot(n,s),0.0);
    vec3 diffuse = Material.Kd*sDotN;

    vec3 spec = vec3(0.0f);

    if(sDotN > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s,n);
        spec = Material.Ks * pow(max(dot(r,v), 0.0), Material.Shinniness);
    }

    return ambient + Light.L * (diffuse + spec);

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

float luminance(vec3 colour)
{
    return dot(lum,colour);
}


vec4 pass1()
{
       vec3 Colour = vec3(0.0f);

       float dist = abs(pos.z);

       float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist-Fog.MinDist);

       fogFactor = clamp(fogFactor, 0.0, 1.0);

       //vec3 shadeColour =blinnPhongSpot(pos,normalize(n));

       Colour = mix(Fog.Colour, blinnPhongSpot(pos,normalize(n)), fogFactor);

       vec3 texColour = texture(Tex1, TexCoord).rgb;

       return vec4(blinnPhongSpot(pos,normalize(n)), 1.0);
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

  //     Colour = mix(Fog.Colour, blinnPhongSpot(pos,normalize(n)), fogFactor);

      // vec3 texColour = texture(Tex1, TexCoord).rgb;

    //   FragColor = vec4(Colour, 1.0);




    RenderPass();


}

