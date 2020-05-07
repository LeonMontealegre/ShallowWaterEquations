#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inDisplacement;

out vec3 vertexNormal;
out vec2 tcs;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main() {
    gl_Position = (projMatrix) * (viewMatrix * modelMatrix * vec4(inPosition + inDisplacement, 1.0));
    vertexNormal = normalize(viewMatrix * modelMatrix * vec4(-inNormal, 0.0)).xyz;
    tcs = inTexCoords;
}