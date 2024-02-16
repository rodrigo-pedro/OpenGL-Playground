#version 330 core

in vec3 pos;
in vec3 normal;

out vec4 outColor;

in vec2 TexCoord;

struct Material {
  sampler2D AlbedoTexture;
  vec3 ambientColor;
};

uniform Material material;

const int cel_levels = 3;

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
};

vec3 calcLighting(Light light, vec3 normal, vec3 pos, vec3 diffuseColor) {
  vec3 lightPos = (ViewMatrix * vec4(light.position, 1.0)).xyz;
  vec3 lightDir = normalize(lightPos - pos);
  float intensity = max(dot(normal, lightDir), 0.0);
  intensity = ceil(intensity * cel_levels) * (1.0 / cel_levels);

  vec3 diff = intensity * diffuseColor * light.color;

  // attenuation
  float dist = length(lightPos - pos);
  float att = 1.0;
  if (light.attenuation != vec3(0.0, 0.0, 0.0)) { // if has attenuation
    att = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist  * dist);
  }

  // ambient
  vec3 ambient = material.ambientColor * light.color;

  return (diff + ambient) * att;
}

void main(void) {
  vec3 normal_norm = normalize(normal);
  vec3 diffuseColor = texture(material.AlbedoTexture, TexCoord).rgb;
  
  vec3 color = vec3(0.0, 0.0, 0.0);

  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].color == vec3(0.0, 0.0, 0.0)) { // not initialized
      break;
    }

    color += calcLighting(lights[i], normal_norm, pos, diffuseColor);
  }

  outColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}

