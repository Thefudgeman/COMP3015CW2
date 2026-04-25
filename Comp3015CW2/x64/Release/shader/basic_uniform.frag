#version 460

in vec3 n, pos;
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

      // vec3 texColour = texture(Tex1, TexCoord).rgb;

       return vec4(Colour, 1.0);
}

vec4 pass2()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);

    float s00 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(-1,1)).rgb);
    float s10 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(-1,0)).rgb);
    float s20 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(-1,-1)).rgb);
    float s01 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(0,1)).rgb);
    float s21 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(0,-1)).rgb);
    float s02 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(1,1)).rgb);
    float s12 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(1,0)).rgb);
    float s22 = luminance(texelFetchOffset(Tex1, pix, 0, ivec2(1,-1)).rgb);

    float sx = s00 + 2*s10+s20 - (s02 + 2*s12+s22);
    float sy = s00 + 2*s01+s02 - (s20 + 2*s21+s22);

    float g = sx*sx + sy*sy;
    if(edgeDetection)
    {
       if(g > EdgeThreshold)
       {
           return vec4(1.0);
       }
       else
        {
            return texelFetch(Tex1, pix,0);
        }    
    }
    else
    {
        return texelFetch(Tex1, pix,0);
    }


}


void main() {





   // for (int i = 0; i<3; i++)
  //  {
    //     Colour = phongModel(0, pos, normalize(n));
 //   }

    if(Pass == 1)
    {
        if(texture(Tex1, TexCoord).a < 0.15)
        {
            discard;
        }
        FragColor = pass1();
    }
    if(Pass == 2)
    {
        FragColor = pass2();
    }

   //      vec3 Colour = vec3(0.0f);

   //    float dist = abs(pos.z);

    //   float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist-Fog.MinDist);

    //   fogFactor = clamp(fogFactor, 0.0, 1.0);

       //vec3 shadeColour =blinnPhongSpot(pos,normalize(n));

  //     Colour = mix(Fog.Colour, blinnPhongSpot(pos,normalize(n)), fogFactor);

      // vec3 texColour = texture(Tex1, TexCoord).rgb;

    //   FragColor = vec4(Colour, 1.0);
}

