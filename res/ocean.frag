#version 330 core

uniform sampler2D tex;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 vertexNormal;
in vec2 tcs;
in vec3 pos;
in vec3 lightDir;

out vec4 fragColor;

uniform vec3 viewPos;
uniform Light light;

uniform float heightMax = 0;
uniform float heightMin = 0;

void main() {
    vec3 norm = vertexNormal;
    vec3 viewDir  = normalize(viewPos - pos);

    vec3 ambientFactor = vec3(0.0);
    vec3 diffuseFactor = vec3(0.2);

    vec3 skyColor = vec3(0.49, 0.73, 0.91);

    // Ambient color
    vec3 ambient = light.ambient * ambientFactor;

    // Height color
    vec3 shallowColor = vec3(0.0, 0.64, 0.68);
    vec3 deepColor = vec3(0.02, 0.05, 0.10);

    float relativeHeight = clamp((pos.y - heightMin) / (heightMax - heightMin), 0, 1);
    vec3 heightColor = relativeHeight * shallowColor + (1 - relativeHeight) * deepColor;

    // Spray color
    vec3 sprayBaseColor = vec3(1.0);

    float sprayThresholdUpper = 1.0;
    float sprayThresholdLower = 0.9;
    float sprayRatio = 0;
    if (relativeHeight > sprayThresholdLower) sprayRatio = (relativeHeight - sprayThresholdLower) / (sprayThresholdUpper - sprayThresholdLower);
    vec3 sprayColor = sprayRatio * sprayBaseColor;

    // Diffuse color
    float diff = clamp(dot(norm, lightDir), 0, 1);
    vec3 diffuse = diffuseFactor * light.diffuse * diff;

    // if (dot(norm, viewDir) > 0) norm = -norm;

    // Pseudo reflection
    float refCoeff = clamp(pow(clamp(dot(norm, viewPos), 0, 1), 0.3), 0, 1);
    vec3 reflectCol = (1 - refCoeff) * skyColor;

    // Specular color
    vec3 reflectDir = reflect(lightDir, norm);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = light.specular * specCoeff;

    vec3 combinedColor = ambient + diffuse + heightColor + 0.3 * reflectCol;

    // sprayRatio = clamp(sprayRatio, 0, 1);
    // combinedColor *= (1 - sprayRatio);
    // combinedColor += sprayColor;

    specCoeff = clamp(specCoeff, 0, 1);
    combinedColor *= (1 - specCoeff);
    combinedColor += specular;

    fragColor = vec4(combinedColor, 1.0);
}


