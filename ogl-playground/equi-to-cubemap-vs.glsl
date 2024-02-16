#version 330 core

in vec4 inPosition;
out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    localPos = vec3(inPosition);  
    gl_Position =  projection * view * vec4(localPos, 1.0);
}