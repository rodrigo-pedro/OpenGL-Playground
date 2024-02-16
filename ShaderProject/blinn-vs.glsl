#version 330 core

in vec4 inPosition;
in vec4 inNormal;
in vec2 inTexcoord;

// view space position, normal and light position
out vec3 pos;
out vec3 normal;
out vec2 TexCoord;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix; //model space

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void) {
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * inPosition;

  pos = (ViewMatrix * ModelMatrix * inPosition).xyz;

  mat3 normalMatrixView = mat3(ViewMatrix) * NormalMatrix;
  normal = normalMatrixView * inNormal.xyz;

  TexCoord = inTexcoord;
}
