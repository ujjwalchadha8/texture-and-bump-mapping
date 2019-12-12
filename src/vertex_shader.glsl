#version 150 core
in vec3 position; //vertex position
in vec3 normal;
in vec2 texCoord;
in vec3 ov_in;
in vec3 ou_in;
out vec2 TexCoord; //UV coordinates
out vec3 Normal; // Normal
out vec3 FragPos; //Position
out vec3 OU;
out vec3 OV;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal =  normalize(mat3(transpose(inverse(model) )) * normal);
    TexCoord = texCoord;
    OV = normalize(mat3(transpose(inverse(model))) * ov_in);
    OU = normalize(mat3(transpose(inverse(model))) * ou_in);
}