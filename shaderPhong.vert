#version 430 core

layout(location = 0) in vec4 vertex;  // Posição do vértice
layout(location = 1) in vec4 color;   // Cor do vértice
layout(location = 2) in vec4 normal;  // Normal do vértice

uniform mat4 u_projMatrix;       // Matriz de projeção
uniform mat4 u_modelViewMatrix;  // Matriz de modelo-visão
uniform mat4 u_modelMatrix;      // Matriz do modelo (para transformar normais) (obs.: Mesma transformacoes aplicadas ao objeto)

out vec4 vColor;       // Cor intermediária enviada ao fragment shader
out vec3 fragPos;      // Posição do fragmento no espaço do mundo
out vec3 fragNormal;   // Normal transformada para o espaço do mundo

void main(void) {
    // Transformar o vértice para o espaço de projeção
    gl_Position = u_projMatrix * u_modelViewMatrix * vertex;

    // Posição do fragmento no espaço do mundo
    fragPos = vec3(u_modelMatrix * vertex);

    // Normal transformada para o espaço do mundo
    fragNormal = normalize(mat3(transpose(inverse(u_modelMatrix))) * vec3(normal));

    // Passar a cor adiante (pode ser modificada posteriormente no fragment shader)
    vColor = color;
}
