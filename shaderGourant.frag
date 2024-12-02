#version 430 core

in vec4 vColor;  // Cor final interpolada
out vec4 fColor; // Cor final do fragmento

void main(void) {
    // Usar a cor interpolada do Vertex Shader
    fColor = vColor;
}
