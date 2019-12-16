#version 150 core
in vec3 position;
in vec3 normal;
in vec2 textureCoordinates;
in vec3 ovIn;
in vec3 ouIn;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 OU;
out vec3 OV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);

    TexCoord = textureCoordinates;
    Normal =  normalize(mat3(transpose(inverse(model))) * normal);
    FragPos = vec3(model * vec4(position, 1.0f));


    OV = normalize(mat3(transpose(inverse(model))) * ovIn);
    OU = normalize(mat3(transpose(inverse(model))) * ouIn);
}