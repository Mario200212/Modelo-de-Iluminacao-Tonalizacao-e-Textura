#version 430 core

flat in vec4 vColor;       // Cor constante do triângulo
flat in vec3 fragPos;      // Posição constante do triângulo
flat in vec3 fragNormal;   // Normal constante do triângulo

out vec4 fColor;           // Cor final do fragmento

uniform vec3 lightPos;       // Posição da luz no espaço do mundo
uniform vec3 lightColor;     // Cor da luz
uniform vec3 ambientColor;   // Cor ambiente
uniform vec3 cameraPos;      // Posição da câmera no espaço do mundo
uniform float ka;            // Coeficiente ambiente
uniform float kd;            // Coeficiente difusa
uniform float ks;            // Coeficiente especular
uniform float shininess;     // Exponente de brilho

void main(void) {
    // Componente Ambiente
    vec3 ambiente = ka * ambientColor;

    // Componente Difusa
    vec3 lightDir = normalize(lightPos - fragPos);
    float NdotL = max(dot(normalize(fragNormal), lightDir), 0.0);
    vec3 difusa = kd * NdotL * lightColor;

    // Componente Especular
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(fragNormal));
    float RdotV = max(dot(reflectDir, viewDir), 0.0);
    vec3 especular = ks * pow(RdotV, shininess) * lightColor;

    // Cor Final: Soma das componentes + cor constante do triângulo
    vec3 finalColor = (ambiente + difusa + especular) * vec3(vColor);

    fColor = vec4(finalColor, 1.0);
}
