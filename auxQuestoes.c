#include <math.h>
#include <stdio.h>
#include <stdlib.h>  // Para usar a função exit e free
#include <glad/glad/glad.h>
#include "auxQuestoes.h"
#include "myTools2.h"
#include <string.h>

// Função N_i_k corrigida para maior precisão
float N_i_k(int i, int k, float u, float *x) {
    float a1=0.0, a2=0.0;
    // Caso base (k = 1)
    if (k == 1) {
        if (u >= x[i] && u <= x[i + 1]) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
    if ((u >= x[i] && u <= x[i + k]))  {

        // Cálculo de a1
        if ((x[i + k - 1] - x[i]) == 0.0) {
            a1 = 0.0;
        } else {
            a1 = (u - x[i]) / (x[i + k - 1] - x[i]);
        }

        // Cálculo de a2
        if ((x[i + k] - x[i + 1]) == 0.0) {
            a2 = 0.0;
        } else {
            a2 = (x[i + k] - u) / (x[i + k] - x[i + 1]);
        }
    }
    
    // Recursão com a função base N_i_k
    return a1 * N_i_k(i, k - 1, u, x) + a2 * N_i_k(i + 1, k - 1, u, x);
}



// Função para calcular a curva B-Spline tridimensional com coordenadas homogêneas
GLfloat* B_Spline_Curve(int n, int k, float t, GLfloat *control_points, float *knots) {
    // Alocar dinamicamente o vetor resultante (4 valores: x, y, z, h)
    GLfloat *result = (float *)malloc(4 * sizeof(float));
    if (result == NULL) {
    fprintf(stderr, "Erro ao alocar memória para o resultado da curva B-Spline\n");
    exit(EXIT_FAILURE);
}

    float Px = 0.0f, Py = 0.0f, Pz = 0.0f, Ph = 0.0f;

    // Iterar sobre os pontos de controle e somar o resultado da multiplicação de N_i_k com B_i
    for (int i = 0; i < n; i++) {
        float N_value = N_i_k(i, k, t, knots);  // Função base N_{i,k}(t)

        // Multiplicar o valor de N_i_k pelas coordenadas homogêneas x, y, z, h dos pontos de controle
        Px += control_points[4 * i] * N_value;      // Coordenada x
        Py += control_points[4 * i + 1] * N_value;  // Coordenada y
        Pz += control_points[4 * i + 2] * N_value;  // Coordenada z
        Ph += control_points[4 * i + 3] * N_value;  // Coordenada homogênea h
    }

    // Armazenar o resultado final nas coordenadas tridimensionais divididas por h
    Ph=1;
    if (Ph == 0.0f) {
        // Caso especial: se Ph for zero, atribuir valores padrão (zero ou outro valor)
        result[0] = 0.0f;  // Coordenada x da curva
        result[1] = 0.0f;  // Coordenada y da curva
        result[2] = 0.0f;  // Coordenada z da curva
        result[3] = 1.0f;  // Coordenada homogênea para 1
    } else {
        // Caso normal: dividir cada coordenada por Ph
        result[0] = Px / Ph;  // Coordenada x da curva
        result[1] = Py / Ph;  // Coordenada y da curva
        result[2] = Pz / Ph;  // Coordenada z da curva
        result[3] = 1.0f;     // Coordenada homogênea para 1
    }
    return result;
}

// Função para gerar a malha 1D
void gerarMalha1D(float x_min, float x_max, int n, float *malha) {
    float passo = (x_max - x_min) / (n - 1);  // Tamanho do passo entre os pontos
    // Preencher o array com os pontos da malha
    for (int i = 0; i < n; i++) {
        malha[i] = x_min + i * passo;  // Calcula o ponto i-ésimo da malha
    }
}

// Função para avaliar os pontos da malha 1D na curva B-Spline
void avaliarMalhaBSpline(int n, int k, float *malha, int num_pontos, float *control_points, float *knots, GLfloat *resultados) {
    for (int i = 0; i < num_pontos; i++) {
    GLfloat *result = B_Spline_Curve(n, k, malha[i], control_points, knots);

    if (4 * i + 3 >= 4 * num_pontos) {
        printf("Erro: Acesso fora dos limites no vetor resultados\n");
        exit(EXIT_FAILURE);
    }
    resultados[4 * i] = result[0];
    resultados[4 * i + 1] = result[1];
    resultados[4 * i + 2] = result[2];
    resultados[4 * i + 3] = result[3];
    free(result);
    }    
}


void multiplyRowVectorByMatrix(GLfloat* rowVector,GLfloat* matrix, GLfloat* result) {
    // Multiplicação de um vetor linha por uma matriz 4x4
    for (int i = 0; i < 4; ++i) {
        result[i] = 0.0f;
        for (int j = 0; j < 4; ++j) {
            result[i] += rowVector[j] * matrix[j + i * 4];
            //result[i] += matrix[j] * rowVector[j + i * 4];
        }
    }
}

GLfloat* Hermite_Curve(float t, float *control_points) {// o control_points é column first
    // Alocar dinamicamente o vetor resultante (4 valores: x, y, z, h)
    GLfloat *result = (GLfloat *)malloc(4 * sizeof(GLfloat));
    GLfloat *Hermite_Matrix=(GLfloat *)malloc(16 * sizeof(GLfloat));
    GLfloat *intermediate_result=(GLfloat *)malloc(16* sizeof(GLfloat));

    //Carregando a Matrix de Hermite (Column-First)
    Hermite_Matrix[0] = 2.0f; Hermite_Matrix[1]=-3.0f;Hermite_Matrix[2] = 0.0f; Hermite_Matrix[3]=1.0f;
    Hermite_Matrix[4] = -2.0f; Hermite_Matrix[5]=3.0f;Hermite_Matrix[6] = 0.0f; Hermite_Matrix[7]=0.0f;
    Hermite_Matrix[8] = 1.0f; Hermite_Matrix[9]=-2.0f;Hermite_Matrix[10] = 1.0f; Hermite_Matrix[11]=0.0f;
    Hermite_Matrix[12] = 1.0f; Hermite_Matrix[13]=-1.0f;Hermite_Matrix[14] = 0.0f; Hermite_Matrix[15]= 0.0f;

    // Multiplicar a matriz de Hermite pela matriz de control_points
    matrixMultiply4x4(Hermite_Matrix, control_points, intermediate_result);

    // Montando o vetor [t^3, t^2, t, 1]
    GLfloat tVector[4] = {t * t * t, t * t, t, 1.0f};

    // Multiplicar o vetor [t^3, t^2, t, 1] pelo resultado intermediário
    multiplyRowVectorByMatrix(tVector, intermediate_result, result);

    if (result == NULL || Hermite_Matrix == NULL || intermediate_result == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o resultado da curva de Hermite\n");
        exit(EXIT_FAILURE);
    }

    // Liberar a memória alocada para a matriz de Hermite e resultado intermediário
    free(Hermite_Matrix);
    free(intermediate_result);

    return result;
}

void avaliaHermite(int num_pontos, float *malha, GLfloat *resultados, GLfloat* control_points)
{
    for (int i = 0; i < num_pontos; i++) {
        GLfloat *result = Hermite_Curve(malha[i], control_points);
        resultados[4 * i] = result[0];
        resultados[4 * i + 1] = result[1];
        resultados[4 * i + 2] = result[2];
        resultados[4 * i + 3] = result[3];
        free(result);
    }    
}

// void MontaMalhaHermite(int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices)
// {
//     float *malhaAngulos = (float *)malloc(qtdAngulos * sizeof(float));
//     gerarMalha1D(0,360, qtdAngulos,malhaAngulos);
//     for(int i=0; i<qtdAngulos;i++)
//     {
//         float *resultadoMult = (float *)malloc(4 * num_pontos * sizeof(float));
//         float *resultados = (float *)malloc(4 * num_pontos * sizeof(float)); 
//         GLfloat *rot = (GLfloat *)malloc(16 * sizeof(GLfloat));
//         matrixRotate4x4(malhaAngulos[i], 0, 1, 0, rot);
//         matrixMultiply4x4(PontosControle,rot, resultadoMult);
//         //matrixMultiply4x4(rot, PontosControle, resultadoMult);
//         avaliaHermite(num_pontos, malha, resultados, resultadoMult);
//         memcpy(vertices+4*i*num_pontos, resultados, 4 * num_pontos * sizeof(GLfloat));
//         free(resultadoMult);
//         free(resultados);
//         free(rot);
//     }
// }

void transporMatriz(float *matriz, float *transposta, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            // Coloca o elemento da posição (i, j) na posição (j, i) da transposta
            transposta[i * colunas + j] = matriz[j * linhas + i];
        }
    }
}

void MontaMalhaHermite(int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices)
{
    float *malhaAngulos = (float *)malloc(qtdAngulos * sizeof(float));
    gerarMalha1D(0,360, qtdAngulos,malhaAngulos);
    for(int i=0; i<qtdAngulos;i++)
    {
        float *resultadoMult = (float *)malloc(4 * num_pontos * sizeof(float));
        float *transposta=(float *)malloc(16 * sizeof(float));
        matrixIdentity4x4(transposta);
        float *resultados = (float *)malloc(4 * num_pontos * sizeof(float)); 
        GLfloat *rot = (GLfloat *)malloc(16 * sizeof(GLfloat));
        matrixRotate4x4(malhaAngulos[i], 0, 1, 0, rot);
        transporMatriz(rot,transposta,4,4);
        matrixMultiply4x4(PontosControle,transposta,resultadoMult);
        avaliaHermite(num_pontos, malha, resultados, resultadoMult);
        memcpy(vertices+4*i*num_pontos, resultados, 4 * num_pontos * sizeof(GLfloat));
        free(resultadoMult);
        free(resultados);
        free(transposta);
        free(rot);
    }
}

GLfloat* Bezier_Curve(float t, float *control_points) {// o control_points é column first
    // Alocar dinamicamente o vetor resultante (4 valores: x, y, z, h)
    GLfloat *result = (GLfloat *)malloc(4 * sizeof(GLfloat));
    GLfloat *Bezier_Matrix=(GLfloat *)malloc(16 * sizeof(GLfloat));
    GLfloat *intermediate_result=(GLfloat *)malloc(16* sizeof(GLfloat));

    //Carregando a Matrix de Bezier (Column-First)
    Bezier_Matrix[0] = -1.0f; Bezier_Matrix[1]=3.0f;Bezier_Matrix[2] = -3.0f; Bezier_Matrix[3]=1.0f;
    Bezier_Matrix[4] = 3.0f; Bezier_Matrix[5]=-6.0f;Bezier_Matrix[6] = 3.0f; Bezier_Matrix[7]=0.0f;
    Bezier_Matrix[8] = -3.0f; Bezier_Matrix[9]=3.0f;Bezier_Matrix[10] = 0.0f; Bezier_Matrix[11]=0.0f;
    Bezier_Matrix[12] = 1.0f; Bezier_Matrix[13]=0.0f;Bezier_Matrix[14] = 0.0f; Bezier_Matrix[15]= 0.0f;

    // Multiplicar a matriz de Bezier pela matriz de control_points
    matrixMultiply4x4(Bezier_Matrix, control_points, intermediate_result);

    // Montando o vetor [t^3, t^2, t, 1]
    GLfloat tVector[4] = {t * t * t, t * t, t, 1.0f};

    // Multiplicar o vetor [t^3, t^2, t, 1] pelo resultado intermediário
    multiplyRowVectorByMatrix(tVector, intermediate_result, result);

    if (result == NULL || Bezier_Matrix == NULL || intermediate_result == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o resultado da curva de Bezier\n");
        exit(EXIT_FAILURE);
    }

    // Liberar a memória alocada para a matriz de Bezier e resultado intermediário
    free(Bezier_Matrix);
    free(intermediate_result);

    return result;
}

void avaliaBezier(int num_pontos, float *malha, GLfloat *resultados, GLfloat* control_points)
{
    for (int i = 0; i < num_pontos; i++) {
        GLfloat *result = Bezier_Curve(malha[i], control_points);
        resultados[4 * i] = result[0];
        resultados[4 * i + 1] = result[1];
        resultados[4 * i + 2] = result[2];
        resultados[4 * i + 3] = result[3];
        free(result);
    }    
}

void extrairSubvetor(float *resultados, int j, float *subvetor) {
    for (int i = 0; i < 4; i++) {
        subvetor[i] = resultados[4 * j + i];  // Copiando os 4 elementos para o subvetor
    }
}

void multiplicarMatrizPorVetor(float matriz[16], float vetor[4], float resultado[4]) {
    for (int i = 0; i < 4; i++) {
        resultado[i] = matriz[i] * vetor[0] + 
                       matriz[i + 4] * vetor[1] + 
                       matriz[i + 8] * vetor[2] + 
                       matriz[i + 12] * vetor[3];
    }
}

void MontaMalhaBezier(int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices)
{
    //Avaliar Bezier na malha fornecida
    GLfloat* resultados=(float *)malloc(4*num_pontos * sizeof(float));
    avaliaBezier(num_pontos, malha, resultados, PontosControle);
    float *malhaAngulos = (float *)malloc(qtdAngulos * sizeof(float));
    gerarMalha1D(0,360, qtdAngulos,malhaAngulos);
    
    for(int j=0;j<num_pontos;j++)
    {   float *subvetor=(float *)malloc(4 * qtdAngulos * sizeof(float));
        extrairSubvetor(resultados, j, subvetor);
        float *PontosAndarAtual = (float *)malloc(4 * qtdAngulos * sizeof(float));
        for(int i=0; i<qtdAngulos;i++)
        {   
            float *resultadoRot=(float *)malloc(4 * sizeof(float));
            GLfloat *rot = (GLfloat *)malloc(16 * sizeof(GLfloat));
            matrixRotate4x4(malhaAngulos[i], 0.25, 1, 0, rot);
            multiplicarMatrizPorVetor(rot,subvetor,resultadoRot);
            memcpy(&PontosAndarAtual[4 * i], resultadoRot, 4 * sizeof(float));
            printf("Ponto %d, Ângulo %d: %.2f, %.2f, %.2f, %.2f\n", j, i, resultadoRot[0], resultadoRot[1], resultadoRot[2], resultadoRot[3]);
            free(resultadoRot);
            free(rot);   
        }
        memcpy(&vertices[j * 4 * qtdAngulos], PontosAndarAtual, 4 * qtdAngulos * sizeof(float));
        free(subvetor);
        free(PontosAndarAtual);
    }
    free(malhaAngulos);
    free(resultados);
}

void SuperficieSweepBezier(float r, int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices)
{
    //Calculando os pontos na curva de Bezier
    GLfloat* resultados=(float *)malloc(4*num_pontos * sizeof(float));
    avaliaBezier(num_pontos, malha, resultados, PontosControle);

    //Calculando o vetor de angulos theta
    float *malhaAngulos = (float *)malloc(qtdAngulos * sizeof(float));
    gerarMalha1D(0,360, qtdAngulos,malhaAngulos);

    //Inicializando as componenetes de cada ponto da curva de Bezier
    float x0;
    float y0;
    float z0;
    float theta;
    //Para cada ponto na curva de Bezier, vamos obter os circulos de raio r
    for(int i=0;i<num_pontos;i++)
    {
       x0=resultados[4*i];
       y0=resultados[4*i+1];
       z0=resultados[4*i+2];
       
       for(int j=0; j<qtdAngulos; j++)
       {
           theta=malhaAngulos[j]*(M_PI / 180);; 
           vertices[4*i*qtdAngulos+4*j]=x0+r+r*cos(theta);
           vertices[4*i*qtdAngulos+4*j+1]=y0;
           vertices[4*i*qtdAngulos+4*j+2]=z0+r*sin(theta);
           vertices[4*i*qtdAngulos+4*j+3]=1;
       }
    }
}

void geraMatrizColumnFirst(float P1[], float P2[], float P3[], float P4[], float matriz[16]) {
    // Preenche a matriz no formato column-major para OpenGL
    matriz[0]  = P1[0];
    matriz[1]  = P2[0];
    matriz[2]  = P3[0];
    matriz[3]  = P4[0];
    
    matriz[4]  = P1[1];
    matriz[5]  = P2[1];
    matriz[6]  = P3[1];
    matriz[7]  = P4[1];
    
    matriz[8]  = P1[2];
    matriz[9]  = P2[2];
    matriz[10] = P3[2];
    matriz[11] = P4[2];
    
    matriz[12] = P1[3];
    matriz[13] = P2[3];
    matriz[14] = P3[3];
    matriz[15] = P4[3];
}

GLfloat* Bezier_Surface(float t, float s,  float *control_points_1,float *control_points_2,float *control_points_3,float *control_points_4) 
{// o control_points é column first
    
    // // Montando as curvas
    // float* P_1t=(GLfloat *)malloc(4 * sizeof(GLfloat));
    // float* P_2t=(GLfloat *)malloc(4 * sizeof(GLfloat));
    // float* P_3t=(GLfloat *)malloc(4 * sizeof(GLfloat));
    // float* P_4t=(GLfloat *)malloc(4 * sizeof(GLfloat));

    float* P_1t=Bezier_Curve(t,control_points_1);
    float* P_2t=Bezier_Curve(t,control_points_2);
    float* P_3t=Bezier_Curve(t,control_points_3);
    float* P_4t=Bezier_Curve(t,control_points_4);

    float *MatrizPs=(GLfloat *)malloc(16 * sizeof(GLfloat));
    geraMatrizColumnFirst(P_1t,P_2t,P_3t,P_4t,MatrizPs);
    
    GLfloat tVector[4] = {t * t * t, t * t, t, 1.0f};
    GLfloat sVector[4] = {s * s * s, s * s, s, 1.0f};

    GLfloat *Bezier_Matrix=(GLfloat *)malloc(16 * sizeof(GLfloat));

    //Carregando a Matrix de Bezier (Column-First)
    Bezier_Matrix[0] = -1.0f; Bezier_Matrix[1]=3.0f;Bezier_Matrix[2] = -3.0f; Bezier_Matrix[3]=1.0f;
    Bezier_Matrix[4] = 3.0f; Bezier_Matrix[5]=-6.0f;Bezier_Matrix[6] = 3.0f; Bezier_Matrix[7]=0.0f;
    Bezier_Matrix[8] = -3.0f; Bezier_Matrix[9]=3.0f;Bezier_Matrix[10] = 0.0f; Bezier_Matrix[11]=0.0f;
    Bezier_Matrix[12] = 1.0f; Bezier_Matrix[13]=0.0f;Bezier_Matrix[14] = 0.0f; Bezier_Matrix[15]= 0.0f;

    // Multiplicar a matriz de Bezier pela matriz de control_points
    GLfloat *result1=(GLfloat *)malloc(16 * sizeof(GLfloat));
    GLfloat *result2=(GLfloat *)malloc(4 * sizeof(GLfloat));

    matrixMultiply4x4(Bezier_Matrix,MatrizPs, result1);
    multiplyRowVectorByMatrix(sVector,result1, result2);

    if (result1 == NULL || Bezier_Matrix == NULL || result2 == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o resultado da curva de Bezier\n");
        exit(EXIT_FAILURE);
    }

    // Liberar a memória alocada para a matriz de Bezier e resultado intermediário
    free(Bezier_Matrix);
    free(result1);
    free(P_1t);
    free(P_2t);
    free(P_3t);
    free(P_4t);

    return result2;
}


void avaliaBezierSurface(int num_pontosT, int num_pontosS, float *malhat, float *malhas, GLfloat *resultados, GLfloat *control_points_1, GLfloat *control_points_2, GLfloat *control_points_3, GLfloat *control_points_4) 
{
    for (int i = 0; i < num_pontosT; i++) {
        for (int j = 0; j < num_pontosS; j++) 
        {
            GLfloat *result = Bezier_Surface(malhat[i], malhas[j], control_points_1, control_points_2, control_points_3, control_points_4);
            int index = 4 * (i * num_pontosS + j);  // Corrigindo o índice com base em i e j
            resultados[index] = result[0];
            resultados[index + 1] = result[1];
            resultados[index + 2] = result[2];
            resultados[index + 3] = result[3];
            free(result);
        }
    }
}


void gerarIndicesSuperficieBezier(int num_pontosT, int num_pontos_s, GLuint *indices) {
    int idx = 0;  // Índice para preencher o array de índices
    for (int i = 0; i < num_pontosT - 1; i++) {
        for (int j = 0; j < num_pontos_s - 1; j++) {
            // Índices dos pontos do quadrado
            int p1 = i * num_pontos_s + j;       // P1
            int p2 = i * num_pontos_s + (j + 1); // P2
            int p3 = (i + 1) * num_pontos_s + j; // P3
            int p4 = (i + 1) * num_pontos_s + (j + 1); // P4

            // Primeiro triângulo (P1, P2, P4)
            indices[idx++] = p1;
            indices[idx++] = p2;
            indices[idx++] = p4;

            // Segundo triângulo (P1, P4, P3)
            indices[idx++] = p1;
            indices[idx++] = p4;
            indices[idx++] = p3;
        }
    }
}

GLfloat* Reta_Curve(float t, float *control_points_Cone)
{
    GLfloat *result = (GLfloat *)malloc(4 * sizeof(GLfloat));
    float x1=control_points_Cone[0];
    float y1=control_points_Cone[1];
    float z1=control_points_Cone[2];

    float x2=control_points_Cone[4];
    float y2=control_points_Cone[5];
    float z2=control_points_Cone[6];

    result[0]=x1+t*(x2-x1);
    result[1]=y1+t*(y2-y1);
    result[2]=z1+t*(z2-z1);
    result[3]=1.0f;

    return result;
}

void avaliaCurvaReta(int num_pontos, float *malha, GLfloat *resultados, GLfloat* control_points_Cone)
{
    for (int i = 0; i < num_pontos; i++)
    {
        GLfloat *result = Reta_Curve(malha[i], control_points_Cone);
        resultados[4 * i] = result[0];
        resultados[4 * i + 1] = result[1];
        resultados[4 * i + 2] = result[2];
        resultados[4 * i + 3] = result[3];
        free(result);
    }    
}

void MontaMalhaReta(int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices)
{
    float *malhaAngulos = (float *)malloc(qtdAngulos * sizeof(float));
    gerarMalha1D(0,360, qtdAngulos,malhaAngulos);
    for(int i=0; i<qtdAngulos;i++)
    {
        float *resultadoMult = (float *)malloc(4 * num_pontos * sizeof(float));
        float *transposta=(float *)malloc(16 * sizeof(float));
        matrixIdentity4x4(transposta);
        float *resultados = (float *)malloc(4 * num_pontos * sizeof(float)); 
        GLfloat *rot = (GLfloat *)malloc(16 * sizeof(GLfloat));
        matrixRotate4x4(malhaAngulos[i], 0, 1, 0, rot);
        matrixMultiply4x4(rot,PontosControle,resultadoMult);
        avaliaCurvaReta(num_pontos, malha, resultados, resultadoMult);
        memcpy(vertices+4*i*num_pontos, resultados, 4 * num_pontos * sizeof(GLfloat));
        free(resultadoMult);
        free(resultados);
        free(transposta);
        free(rot);
    }
}

void GeraIndicesConeComTampa(int num_pontos, int qtdAngulos, GLuint *indices)
{
    int idx = 0;
    for (int i = 0; i < qtdAngulos - 1; i++) { // Para cada faixa de ângulos (curva)
        for (int j = 0; j < num_pontos - 1; j++) { // Para cada ponto na curva atual

            // Cada quadrado é dividido em dois triângulos
            int idx1 = i * num_pontos + j;            // Ponto (i, j)
            int idx2 = (i + 1) * num_pontos + j;      // Ponto (i+1, j)
            int idx3 = (i + 1) * num_pontos + (j + 1);// Ponto (i+1, j+1)
            int idx4 = i * num_pontos + (j + 1);      // Ponto (i, j+1)

            // Triângulo 1
            indices[idx++] = idx1;
            indices[idx++] = idx2;
            indices[idx++] = idx3;

            // Triângulo 2
            indices[idx++] = idx1;
            indices[idx++] = idx3;
            indices[idx++] = idx4;
        }
}

    // Índice do ponto central da base
    int centerIndex = num_pontos * qtdAngulos;
    // Adicionar os índices para os triângulos da tampa da base usando o último círculo
    for (int i = 0; i < qtdAngulos - 1; i++) {
        int idx1 = i * num_pontos + (num_pontos - 1);        // Ponto atual no círculo da base
        int idx2 = (i + 1) * num_pontos + (num_pontos - 1);  // Próximo ponto no círculo da base

        // Triângulo entre o ponto central e dois pontos consecutivos do círculo
        indices[idx++] = centerIndex;
        indices[idx++] = idx1;
        indices[idx++] = idx2;
    }

    // Último triângulo para fechar o círculo na base
    int idx1 = (qtdAngulos - 1) * num_pontos + (num_pontos - 1); // Último ponto da base
    int idx2 = 0 * num_pontos + (num_pontos - 1);                // Primeiro ponto da base

    indices[idx++] = centerIndex;
    indices[idx++] = idx1;
    indices[idx++] = idx2;
}

void GeraIndicesCilindroComTampa(int num_pontos, int qtdAngulos, GLuint *indices)
{
    int idx = 0;

    // Gerar índices para as laterais (mesmo código que você já tem)
    for (int i = 0; i < qtdAngulos - 1; i++) {
        for (int j = 0; j < num_pontos - 1; j++) {
            int idx1 = i * num_pontos + j;
            int idx2 = (i + 1) * num_pontos + j;
            int idx3 = (i + 1) * num_pontos + (j + 1);
            int idx4 = i * num_pontos + (j + 1);

            // Triângulo 1
            indices[idx++] = idx1;
            indices[idx++] = idx2;
            indices[idx++] = idx3;

            // Triângulo 2
            indices[idx++] = idx1;
            indices[idx++] = idx3;
            indices[idx++] = idx4;
        }
    }

    // Índice do ponto central da base inferior
    int bottomIndex = num_pontos * qtdAngulos;

    // Gerar índices para a tampa inferior usando o primeiro círculo (j = 0)
    for (int i = 0; i < qtdAngulos - 1; i++) {
        int idx1 = i * num_pontos;        // Ponto atual no círculo inferior
        int idx2 = (i + 1) * num_pontos;  // Próximo ponto no círculo inferior

        // Triângulo entre o ponto central e dois pontos consecutivos do círculo
        indices[idx++] = bottomIndex;
        indices[idx++] = idx1;
        indices[idx++] = idx2;
    }

    // Último triângulo para fechar o círculo inferior
    int idx1 = (qtdAngulos - 1) * num_pontos; // Último ponto do círculo inferior
    int idx2 = 0;                             // Primeiro ponto do círculo inferior

    indices[idx++] = bottomIndex;
    indices[idx++] = idx1;
    indices[idx++] = idx2;

    // Índice do ponto central da base superior
    int topIndex = num_pontos * qtdAngulos + 1;

    // Gerar índices para a tampa superior usando o último círculo (j = num_pontos - 1)
    for (int i = 0; i < qtdAngulos - 1; i++) {
        int idx1 = i * num_pontos + (num_pontos - 1);        // Ponto atual no círculo superior
        int idx2 = (i + 1) * num_pontos + (num_pontos - 1);  // Próximo ponto no círculo superior

        // Triângulo entre o ponto central e dois pontos consecutivos do círculo
        // Invertendo idx1 e idx2 para garantir a orientação correta da face
        indices[idx++] = topIndex;
        indices[idx++] = idx2;
        indices[idx++] = idx1;
    }

    // Último triângulo para fechar o círculo superior
    idx1 = (qtdAngulos - 1) * num_pontos + (num_pontos - 1); // Último ponto do círculo superior
    idx2 = (num_pontos - 1);                                 // Primeiro ponto do círculo superior

    indices[idx++] = topIndex;
    indices[idx++] = idx2;
    indices[idx++] = idx1;
}



void gerarSemicirculo(int num_pontos, float radius, float* semicirculo) {
    for (int i = 0; i < num_pontos; i++) {
        float theta = M_PI * ((float)i / (num_pontos - 1)); // De 0 a PI
        float x = radius * sin(theta); // Coordenada x no semicírculo
        float y = radius * cos(theta); // Coordenada y no semicírculo
        semicirculo[i * 2] = x;
        semicirculo[i * 2 + 1] = y;
    }
}

void MontaMalhaEsferaPorRevolucao(int num_pontos, float radius, int qtdAngulos, GLfloat* vertices) {
    float* semicirculo = (float*)malloc(num_pontos * 2 * sizeof(float));
    gerarSemicirculo(num_pontos, radius, semicirculo);

    int index = 0;
    for (int j = 0; j < qtdAngulos; j++) {
        float theta = 2.0f * M_PI * ((float)j / qtdAngulos); // De 0 a 2PI
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (int i = 0; i < num_pontos; i++) {
            float x = semicirculo[i * 2];
            float y = semicirculo[i * 2 + 1];
            float z = 0.0f;

            // Rotaciona o ponto ao redor do eixo Y
            float x_rot = x * cosTheta + z * sinTheta;
            float z_rot = -x * sinTheta + z * cosTheta;

            // Salva o ponto no array de vértices
            vertices[index++] = x_rot;
            vertices[index++] = y;
            vertices[index++] = z_rot;
            vertices[index++] = 1.0f; // Coordenada homogênea
        }
    }
    free(semicirculo);
}

void MontaIndicesEsferaPorRevolucao(int num_pontos, int qtdAngulos, GLuint* indices) {
    int index = 0;
    for (int j = 0; j < qtdAngulos; j++) {
        int nextJ = (j + 1) % qtdAngulos;
        for (int i = 0; i < num_pontos - 1; i++) {
            int current = j * num_pontos + i;
            int next = j * num_pontos + i + 1;
            int currentNextJ = nextJ * num_pontos + i;
            int nextNextJ = nextJ * num_pontos + i + 1;

            // Primeiro triângulo
            indices[index++] = current;
            indices[index++] = next;
            indices[index++] = currentNextJ;

            // Segundo triângulo
            indices[index++] = next;
            indices[index++] = nextNextJ;
            indices[index++] = currentNextJ;
        }
    }
}

void MontaMalhaToroPorRevolucao(int num_pontos_circulo, int num_pontos_rotacao, float R, float r, GLfloat* vertices) {
    // Aloca memória para os ângulos
    float* malhaCirculo = (float*)malloc(num_pontos_circulo * sizeof(float));
    float* malhaRotacao = (float*)malloc(num_pontos_rotacao * sizeof(float));

    // Gera os ângulos para o círculo e para a rotação
    gerarMalha1D(0.0f, 2.0f * M_PI, num_pontos_circulo, malhaCirculo);  // Ângulos do círculo (φ)
    gerarMalha1D(0.0f, 2.0f * M_PI, num_pontos_rotacao, malhaRotacao);  // Ângulos de rotação (θ)

    int index = 0;
    for (int i = 0; i < num_pontos_rotacao; i++) {
        float theta = malhaRotacao[i];
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (int j = 0; j < num_pontos_circulo; j++) {
            float phi = malhaCirculo[j];
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);

            // Coordenadas do ponto no círculo (plano XY, deslocado de R)
            float x = (R + r * cosPhi);
            float y = r * sinPhi;
            float z = 0.0f;

            // Rotaciona o ponto ao redor do eixo Y
            float x_rot = x * cosTheta + z * sinTheta;
            float y_rot = y;
            float z_rot = -x * sinTheta + z * cosTheta;

            // Salva o ponto no array de vértices
            vertices[index++] = x_rot;
            vertices[index++] = y_rot;
            vertices[index++] = z_rot;
            vertices[index++] = 1.0f; // Coordenada homogênea
        }
    }

    // Libera a memória alocada
    free(malhaCirculo);
    free(malhaRotacao);
}


void MontaIndicesToroPorRevolucao(int num_pontos_circulo, int num_pontos_rotacao, GLuint* indices) {
    int index = 0;
    for (int i = 0; i < num_pontos_rotacao; i++) {
        int nextI = (i + 1) % num_pontos_rotacao;

        for (int j = 0; j < num_pontos_circulo; j++) {
            int nextJ = (j + 1) % num_pontos_circulo;

            int current = i * num_pontos_circulo + j;
            int next = i * num_pontos_circulo + nextJ;
            int currentNextI = nextI * num_pontos_circulo + j;
            int nextNextI = nextI * num_pontos_circulo + nextJ;

            // Primeiro triângulo
            indices[index++] = current;
            indices[index++] = currentNextI;
            indices[index++] = nextNextI;

            // Segundo triângulo
            indices[index++] = current;
            indices[index++] = nextNextI;
            indices[index++] = next;
        }
    }
}

//// Gerando as normais de cada figura //////

//Esfera:

void GerarNormaisEsfera(int num_pontos, int qtdAngulos, const GLfloat* vertices, GLfloat* normals) {
    int index = 0;
    for (int j = 0; j < qtdAngulos; j++) {
        for (int i = 0; i < num_pontos; i++) {
            // Obtém as coordenadas do vértice
            float x = vertices[index];
            float y = vertices[index + 1];
            float z = vertices[index + 2];

            // Calcula o comprimento do vetor
            float length = sqrt(x * x + y * y + z * z);

            // Normaliza e salva no array de normais
            normals[index] = x / length;
            normals[index + 1] = y / length;
            normals[index + 2] = z / length;
            normals[index + 3] = 0.0f; // Componente homogênea para vetores

            index += 4; // Pula para o próximo vértice (4 elementos por vértice: x, y, z, w)
        }
    }
}

//Toro

void GerarNormaisToro(int num_pontos_circulo, int num_pontos_rotacao, float R, float r, const GLfloat* vertices, GLfloat* normals) {
    int index = 0;
    for (int i = 0; i < num_pontos_rotacao; i++) {
        float theta = 2.0f * M_PI * ((float)i / num_pontos_rotacao); // Ângulo θ
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        //Centro do círculo base
        float cx = R * cosTheta;
        float cy = 0.0f;
        float cz = -R * sinTheta;

        for (int j = 0; j < num_pontos_circulo; j++) {
            //Obtém o vértice
            float vx = vertices[index];
            float vy = vertices[index + 1];
            float vz = vertices[index + 2];

            //Calcula o vetor normal (V - C)
            float nx = vx - cx;
            float ny = vy - cy;
            float nz = vz - cz;

            //Normaliza o vetor
            float length = sqrt(nx * nx + ny * ny + nz * nz);
            nx /= length;
            ny /= length;
            nz /= length;

            //Salva a normal no array
            normals[index] = nx;
            normals[index + 1] = ny;
            normals[index + 2] = nz;
            normals[index + 3] = 0.0f; // Componente homogênea para vetores

            index += 4; // Próximo vértice
        }
    }
}

// Cone

void GerarNormaisConeComTampa(int num_pontos, int qtdAngulos, GLfloat *vertices, GLfloat *normals) {
    int idx = 0;

    // Altura e raio do cone (calculados a partir dos pontos de controle)
    float h = 0.5f; // Altura do cone
    float r = 0.5f; // Raio da base do cone

    // Fator de normalização
    float hipotenusa = sqrt(r * r + h * h);

    // Normais para a superfície lateral
    for (int i = 0; i < qtdAngulos; i++) {
        float theta = 2.0f * M_PI * i / qtdAngulos;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (int j = 0; j < num_pontos; j++) {
            // Inclinação da lateral do cone
            float nx = r * cosTheta / hipotenusa;
            float ny = h / hipotenusa;
            float nz = -r * sinTheta / hipotenusa;

            // Armazenar a normal
            normals[idx] = nx;
            normals[idx + 1] = ny;
            normals[idx + 2] = nz;
            normals[idx + 3] = 0.0f; // Componente W para vetores

            idx += 4; // Próximo vértice
        }
    }

    // Normais para a base do cone
    int baseStartIdx = qtdAngulos * num_pontos * 4; // Início dos vértices da base
    for (int i = 0; i < qtdAngulos; i++) {
        //Cada vértice da base aponta para baixo (y = -1)
        normals[baseStartIdx + i * 4] = 0.0f;    // X
        normals[baseStartIdx + i * 4 + 1] = -1.0f; // Y
        normals[baseStartIdx + i * 4 + 2] = 0.0f;  // Z
        normals[baseStartIdx + i * 4 + 3] = 0.0f;  // W
    }

    // Normal para o centro da base (último ponto da base)
    int baseCenterIdx = baseStartIdx + qtdAngulos * 4; // Índice do ponto central da base
    normals[baseCenterIdx] = 0.0f;    // X
    normals[baseCenterIdx + 1] = -1.0f; // Y
    normals[baseCenterIdx + 2] = 0.0f;  // Z
    normals[baseCenterIdx + 3] = 0.0f;  // W
}

// Função modificada para gerar vértices e normais do toro
void MontaMalhaENormaisToroPorRevolucao(int num_pontos_circulo, int num_pontos_rotacao, 
                                float R, float r, 
                                GLfloat* vertices, GLfloat* normais) {
    // Aloca memória para os ângulos
    float* malhaCirculo = (float*)malloc(num_pontos_circulo * sizeof(float));
    float* malhaRotacao = (float*)malloc(num_pontos_rotacao * sizeof(float));

    // Gera os ângulos para o círculo e para a rotação
    gerarMalha1D(0.0f, 2.0f * M_PI, num_pontos_circulo, malhaCirculo);  // Ângulos do círculo (φ)
    gerarMalha1D(0.0f, 2.0f * M_PI, num_pontos_rotacao, malhaRotacao);  // Ângulos de rotação (θ)

    int indexVertices = 0;
    int indexNormais = 0;
    for (int i = 0; i < num_pontos_rotacao; i++) {
        float theta = malhaRotacao[i];
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (int j = 0; j < num_pontos_circulo; j++) {
            float phi = malhaCirculo[j];
            float cosPhi = cos(phi);
            float sinPhi = sin(phi);

            // Coordenadas do ponto no círculo (plano XY, deslocado de R)
            float x = (R + r * cosPhi);
            float y = r * sinPhi;
            float z = 0.0f;

            // Rotaciona o ponto ao redor do eixo Y
            float x_rot = x * cosTheta + z * sinTheta;
            float y_rot = y;
            float z_rot = -x * sinTheta + z * cosTheta;

            // Salva o ponto no array de vértices
            vertices[indexVertices++] = x_rot;
            vertices[indexVertices++] = y_rot;
            vertices[indexVertices++] = z_rot;
            vertices[indexVertices++] = 1.0f; // Coordenada homogênea

            // Cálculo das normais
            // A normal de um toro é dada por (cosPhi * cosTheta, sinPhi, cosPhi * sinTheta)
            float nx = cosPhi * cosTheta;
            float ny = sinPhi;
            float nz = cosPhi * sinTheta;

            // Normaliza a normal (opcional, mas recomendado)
            float comprimento = sqrt(nx * nx + ny * ny + nz * nz);
            if (comprimento != 0.0f) {
                nx /= comprimento;
                ny /= comprimento;
                nz /= comprimento;
            }

            // Salva a normal no array de normais
            normais[indexNormais++] = nx;
            normais[indexNormais++] = ny;
            normais[indexNormais++] = nz;
            normais[indexNormais++] = 0.0f; // W da normal (geralmente 0 para vetores)
        }
    }

    // Libera a memória alocada
    free(malhaCirculo);
    free(malhaRotacao);
}