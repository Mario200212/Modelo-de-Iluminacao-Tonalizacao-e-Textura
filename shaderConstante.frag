#version 430 core

flat in vec4 vColor;       // Cor constante do triângulo
flat in vec3 fragPos;      // Posição constante do triângulo
flat in vec3 fragNormal;   // Normal constante do triângulo

out vec4 fColor;           // Cor final do fragmento

// Uniforms que recebem as propriedades de iluminação
uniform vec3 lightPos;       // Posição da luz no espaço do mundo
uniform vec3 lightColor;     // Cor da luz
uniform vec3 ambientColor;   // Cor ambiente
uniform vec3 cameraPos;      // Posição da câmera no espaço do mundo
uniform float ka;            // Coeficiente ambiente
uniform float kd;            // Coeficiente difusa
uniform float ks;            // Coeficiente especular
uniform float shininess;     // Exponente de brilho

void main(void) {
    // Componente Ambiente: luz ambiente que afeta toda a superfície de maneira uniforme
    vec3 ambiente = ka * ambientColor;

    // Componente Difusa: luz que depende da direção da luz e da normal da superfície
    vec3 lightDir = normalize(lightPos - fragPos); //Vetor direção da luz
    float NdotL = max(dot(normalize(fragNormal), lightDir), 0.0); //Produto escalar normalizado
    vec3 difusa = kd * NdotL * lightColor; //Contribuição da luz difusa

    // Componente Especular: brilho/reflexo da luz que depende da direção da câmera
    vec3 viewDir = normalize(cameraPos - fragPos); // Vetor direção para a câmera
    vec3 reflectDir = normalize(-lightDir + 2.0 * dot(lightDir, fragNormal) * fragNormal); //reflect(-lightDir, normalize(fragNormal)); // Vetor de reflexão da luz
    float RdotV = max(dot(reflectDir, viewDir), 0.0); // Produto escalar entre reflexão e visão
    vec3 especular = ks * pow(RdotV, shininess) * lightColor; // Contribuição da luz especular

    // Cor Final: Combinação das três componentes com a cor do triângulo
    vec3 finalColor = (difusa) * vec3(vColor); //(ambiente + difusa + especular) * vec3(vColor);
    fColor = vec4(finalColor, 1.0); // Atribui a cor final ao fragmento
}
