#version 330 core

in vec4 inPosition;
in vec4 inNormal;
in vec2 inTexcoord;
in vec3 inTangent;
in vec3 inBitangent;

// view space position, normal and light position
out vec3 pos;
out vec2 TexCoord;
out mat3 TBN;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix; //model space

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
   vec3 CamPosition;
};

void main(void) {
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * inPosition;

  pos = (ModelMatrix * inPosition).xyz;

  TexCoord = inTexcoord;

  // view space
  vec3 T = normalize(NormalMatrix * inTangent);
  vec3 B = normalize(NormalMatrix * inBitangent);
  vec3 N = normalize(NormalMatrix * inNormal.xyz);
  TBN = mat3(T, B, N);
}
