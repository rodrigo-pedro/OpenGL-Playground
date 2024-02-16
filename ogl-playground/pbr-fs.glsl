#version 330 core

in vec3 pos;
in mat3 TBN;

out vec4 outColor;

in vec2 TexCoord;


struct Material {
  sampler2D AlbedoTexture;
  vec3 albedoColor; // if no texture
  sampler2D NormalTexture;
  // sampler2D RoughnessTexture;
  // sampler2D MetallicTexture;
  sampler2D MetallicRoughnessTexture;
  float metallic; // if no texture
  float roughness; // if no texture
  float opacity; //TODO if no texture
  // sampler2D AOTexture;
  vec3 ambientColor;
};
uniform Material material;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;


const float PI = 3.1415926535898;

const int MAX_LIGHTS = 10;
struct Light {
  vec3 position;
  vec3 color;
  vec3 attenuation;
};
uniform Light lights[MAX_LIGHTS];

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
   vec3 CamPosition;
};


// normal distribution function
float ggxNormalDistributionFunc(vec3 n, vec3 h, float rough) {
  float a_2 = pow(rough, 4.0);
  float NdotH = max(dot(n, h), 0.0);

  float numerator = a_2;
  float denominator = (NdotH * NdotH) * (a_2 - 1.0) + 1.0;
  denominator = denominator * denominator * PI;

  return numerator/denominator;
}

// geometry function
float ggxGeomFunction(vec3 n, vec3 v, float k) {
  float NdotV = max(dot(n, v), 0.0);

  float numerator = NdotV;
  float denominator = NdotV * (1.0 - k) + k;

  return numerator / denominator;
}

float ggxGeomSmith(vec3 n, vec3 v, vec3 l, float rough) {
  // only direct lighting, so k becomes
  float k = (rough + 1.0);
  k *= k;
  k /= 8.0;

  return ggxGeomFunction(n, v, k) * ggxGeomFunction(n, l, k);
}

vec3 fresnel(vec3 h, vec3 v, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - max(dot(h, v), 0.0), 0.0, 1.0), 5.0);
}

vec3 fresnelRoughness(vec3 n, vec3 v, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - max(dot(n, v), 0.0), 0.0, 1.0), 5.0);
}

vec3 calcLighting(Light light, vec3 v, vec3 n, vec3 albedo, float roughness, float metallic, vec3 F0) {
  vec3 l = normalize(light.position - pos);
  vec3 h = normalize(l + v);

  float D = ggxNormalDistributionFunc(n, h, roughness);
  float G = ggxGeomSmith(n, v, l, roughness);
  vec3 F = fresnel(h, v, F0);

  float dist = length(light.position - pos);
  float att = 1.0;
  if (light.attenuation.z != 0.0) { // if has attenuation
    att = 1.0 / (light.attenuation.z * dist * dist);  // for physical accuracy, only use quadratic attenuation
  }
  vec3 radiance = light.color * att;

  // specular
  vec3 numerator = D * F * G;
  float denominator = max(4.0 * max(dot(n, l), 0.0) * max(dot(n, v), 0.0), 0.001);
  vec3 specular = numerator / denominator;

  // diffuse
  vec3 ks = F;
  vec3 kd = vec3(1.0) - ks;
  kd *= 1.0 - metallic;

  return (kd * albedo / PI + specular) * radiance * max(dot(n, l), 0.0);
}

void main(void) {
  // Using white texture trick, for materials without textures. If there is a texture, we multiply by 1.0, so it doesn't change anything. Else, we multiply the white texture by the color/roughness/metallic.
  vec3 albedo = pow(texture(material.AlbedoTexture, TexCoord).rgb * material.albedoColor, vec3(2.2));  // 2.2 por causa do gamma correction.
  float roughness = texture(material.MetallicRoughnessTexture, TexCoord).g * material.roughness;
  float metallic = texture(material.MetallicRoughnessTexture, TexCoord).b * material.metallic;
  float ao = texture(material.MetallicRoughnessTexture, TexCoord).r;

  vec3 n = texture(material.NormalTexture, TexCoord).rgb;
  n = normalize(TBN * (n * 2.0 - 1.0));

  vec3 v = normalize(CamPosition - pos);

  vec3 F0 = mix(vec3(0.04), albedo, metallic);

  // Lo
  vec3 outRadiance = vec3(0.0, 0.0, 0.0);

  for (int i = 0; i < MAX_LIGHTS; i++) {
     if (lights[i].color == vec3(0.0, 0.0, 0.0)) { // not initialized
      break;
    }

    outRadiance += calcLighting(lights[i], v, n, albedo, roughness, metallic, F0);
  }

  // ambient
  vec3 f = fresnelRoughness(n, v, F0, roughness);
  vec3 kS = f;  // nao temos h, portanto nao podemos usar normaldistributionfunction, logo temos de usar um fresnel modificado que tem em conta a roughness
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;
  vec3 irradiance = texture(irradianceMap, n).rgb;
  vec3 diffuse = irradiance * albedo;

  vec3 R = reflect(-v, n);
  // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
  vec2 brdf  = texture(brdfLUT, vec2(max(dot(n, v), 0.0), roughness)).rg;
  vec3 specular = prefilteredColor * (f * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * ao;

  vec3 tempColor = outRadiance + ambient;

  tempColor = tempColor / (tempColor + 1.0);
  tempColor = pow(tempColor, vec3(1.0/2.2));

  outColor = vec4(tempColor, 1.0);
}
