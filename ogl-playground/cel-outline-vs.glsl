#version 330 core

in vec4 inPosition;
in vec4 inNormal;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix; //in model space

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void) {
    vec3 normal = normalize(NormalMatrix * inNormal.xyz);

    vec3 modelPos = (ModelMatrix * inPosition).xyz;

    vec3 newPos = modelPos + normal * 0.015; // model
    
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(newPos, 1.0);

}
