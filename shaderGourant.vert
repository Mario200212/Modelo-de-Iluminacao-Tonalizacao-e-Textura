#version 430 core

layout(location = 0) in vec4 vertex;  // Posição do vértice
layout(location = 1) in vec4 color;   // Cor do vértice
layout(location = 2) in vec4 normal;  // Normal do vértice

uniform mat4 u_projMatrix;       // Matriz de projeção
uniform mat4 u_modelViewMatrix;  // Matriz de modelo-visão
uniform mat4 u_modelMatrix;      // Matriz do modelo (para transformar normais)
uniform vec3 lightPos;           // Posição da luz no espaço do mundo
uniform vec3 lightColor;         // Cor da luz
uniform vec3 ambientColor;       // Cor ambiente
uniform vec3 cameraPos;          // Posição da câmera no espaço do mundo
uniform float ka;                // Coeficiente ambiente
uniform float kd;                // Coeficiente difusa
uniform float ks;                // Coeficiente especular
uniform float shininess;         // Exponente de brilho

out vec4 vColor;                 // Cor final calculada no vértice

void main(void) {
    // Transformar o vértice para o espaço de projeção
    gl_Position = u_projMatrix * u_modelViewMatrix * vertex;

    // Posição do vértice no espaço do mundo
    vec3 fragPos = vec3(u_modelMatrix * vertex);

    // Transformar a normal para o espaço do mundo
    vec3 fragNormal = normalize(mat3(transpose(inverse(u_modelMatrix))) * vec3(normal));

    // Componente Ambiente
    vec3 ambiente = ka * ambientColor;

    // Componente Difusa
    vec3 lightDir = normalize(lightPos - fragPos);
    float NdotL = max(dot(normalize(fragNormal), lightDir), 0.0);
    vec3 difusa = kd * NdotL * lightColor;

    // Componente Especular
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = normalize(-lightDir + 2.0 * dot(lightDir, fragNormal) * fragNormal);//reflect(-lightDir, normalize(fragNormal));
    float RdotV = max(dot(reflectDir, viewDir), 0.0);
    vec3 especular = ks * pow(RdotV, shininess) * lightColor;

    // Cor Final
    vec3 finalColor = (ambiente + difusa + especular) * vec3(color);

    // Passar a cor final para o Fragment Shader
    vColor = vec4(finalColor, 1.0);
}
