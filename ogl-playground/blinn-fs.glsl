#version 330 core

in vec3 pos;
in vec3 normal;

out vec4 outColor;

in vec2 TexCoord;

struct Material {
  sampler2D AlbedoTexture;
  vec3 specularColor;
  vec3 ambientColor;
  float shininess;
};
uniform Material material;

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

vec3 calcLighting(Light light, vec3 normal, vec3 pos, vec3 viewDir, vec3 diffuseColor) {
  vec3 lightPos = (ViewMatrix * vec4(light.position, 1.0)).xyz;

  // ambient
  vec3 ambient = material.ambientColor * diffuseColor;

  // diffuse
  vec3 lightDir = normalize(lightPos - pos);
  vec3 diff = max(dot(normal, lightDir), 0.0) * diffuseColor * light.color;

  // specular
  vec3 halfVec = normalize(lightDir + viewDir);
  vec3 spec = pow(max(dot(normal, halfVec), 0.0), material.shininess) * material.specularColor * light.color;

  // attenuation
  float dist = length(lightPos - pos);

  float att = 1.0;
  if (light.attenuation != vec3(0.0, 0.0, 0.0)) { // if has attenuation
    att = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist  * dist);
  }

  return (ambient + diff + spec) * att;
}

void main(void) {
  vec3 normal_norm = normalize(normal);
  vec3 viewDir = normalize(-pos);

  vec3 diffuseColor = texture(material.AlbedoTexture, TexCoord).rgb;

  vec3 color = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (lights[i].color == vec3(0.0, 0.0, 0.0)) { // not initialized
      break;
    }

    color += calcLighting(lights[i], normal_norm, pos, viewDir, diffuseColor);
  }

  outColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
