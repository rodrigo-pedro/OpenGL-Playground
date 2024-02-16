#version 330 core

in vec4 inPosition;
out vec3 localPos;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main()
{
    localPos = vec3(inPosition);

    mat4 rotView = mat4(mat3(ViewMatrix)); // remove translation from the view matrix
    vec4 clipPos = ProjectionMatrix * rotView * vec4(localPos, 1.0);

     gl_Position = clipPos.xyww;
}