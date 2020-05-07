#version 330 core

uniform sampler2D tex;

in vec3 vertexNormal;
in vec2 tcs;
in vec3 lightDir;

out vec4 fragColor;

uniform float ambientLight;
uniform vec4 color;

void main() {
    float brightness = clamp( dot( vertexNormal,lightDir ), 0.0,1.0 ) + ambientLight;//min(max(dot(-lightDir, vertexNormal), 0.0) + vec4(ambientLight), 1.0);
    fragColor = color * brightness;
    fragColor.a = 0.5;
    // fragColor.rgb = vertexNormal;
}


