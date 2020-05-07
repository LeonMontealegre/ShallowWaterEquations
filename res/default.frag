#version 330 core

uniform sampler2D tex;

in vec3 vertexNormal;
in vec2 tcs;

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0);
}


