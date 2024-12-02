#version 430 core

layout(location = 0) in vec4 vertex;   // Posição do vértice
layout(location = 1) in vec4 color;    // Cor do vértice
layout(location = 2) in vec4 normal;   // Normal do vértice

uniform mat4 u_projMatrix;       // Matriz de projeção
uniform mat4 u_modelViewMatrix;  // Matriz de modelo-visão
uniform mat4 u_modelMatrix;      // Matriz do modelo (para transformar normais)

flat out vec4 vColor;            // Cor constante do triângulo
flat out vec3 fragPos;           // Posição constante do triângulo no espaço do mundo
flat out vec3 fragNormal;        // Normal constante do triângulo no espaço do mundo

void main(void) {
    // Transformar o vértice para o espaço de projeção
    gl_Position = u_projMatrix * u_modelViewMatrix * vertex;

    // Passar os valores uniformes para o triângulo
    fragPos = vec3(u_modelMatrix * vertex); // Posição no espaço do mundo
    fragNormal = normalize(mat3(transpose(inverse(u_modelMatrix))) * vec3(normal)); // Normal transformada
    vColor = color; // Cor constante
}
