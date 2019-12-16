#version 150 core
out vec4 outColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 OU;
in vec3 OV;
uniform vec3 viewPos;
uniform vec3 lightPos;

uniform float height_scale;
uniform sampler2D textureMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform bool isBumpMapping;
uniform bool noMapping;

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) {
    float height =  texture(depthMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}

void main() {
    float ambientStrength = 0.05f;
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float lightPower = 10.0;
    if (noMapping) {
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        vec3 ambient = ambientStrength * lightColor;

        float diff = max(dot(norm, lightDir), 0);
        vec3 diffuse = diff * lightColor;

        float specularStrength = 0.5f;
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 100);
        vec3 specular = specularStrength * spec * lightColor;

        vec3 lightIntensity = (ambient + diffuse + specular);

        outColor =  texture(textureMap, TexCoord) * vec4(lightIntensity, 1.0);

    } else if (!isBumpMapping) {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec2 TexCoordMapped = parallaxMapping(TexCoord,  viewDir);

        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 normal = texture(normalMap, TexCoordMapped).rgb;
        vec3 norm = normalize(normal * 2.0 - 1.0);
        vec3 reflectDir = reflect(-lightDir, norm);

        vec3 ambient = ambientStrength * lightColor;

        float diff = max(dot(norm, lightDir), 0);
        vec3 diffuse = diff * lightColor;

        float specularStrength = 0.5f;
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 100);
        vec3 specular = specularStrength * spec * lightColor;

        vec3 result = (ambient + diffuse + specular);
        outColor =  texture(textureMap, TexCoordMapped) * vec4(result, 1.0);
    } else {
        float Bu = dFdx(texture(normalMap, TexCoord).x);
        float Bv = dFdx(texture(normalMap, TexCoord).y);
        vec3 A = cross(Normal, OV);
        vec3 B = cross(Normal, OU);
        vec3 bumpNormal = Bu*A - Bv*B;
        vec3 norm = normalize(Normal + bumpNormal);

        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        vec3 ambient = ambientStrength * lightColor;

        float diff = max(dot(norm, lightDir), 0);
        vec3 diffuse = diff * lightColor;

        float specularStrength = 0.5f;
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 96);
        vec3 specular = specularStrength * spec * lightColor * vec3(0.5, 0.5 ,0.5);

        vec3 result = (ambient + diffuse + specular);
        outColor = texture(textureMap, TexCoord) * vec4(result, 1.0);
    }

}