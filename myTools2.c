// myTools2.c
#include <math.h>
#include "myTools2.h"
#include <stdio.h>
#include <stdlib.h>  // Para usar a função exit


// Função para retornar uma matriz identidade 4x4
void matrixIdentity4x4(float m[16]) {
    for (int i = 0; i < 16; i++) {
        if (i % 5 == 0) { //i=0,5,10,15
            m[i] = 1.0f;  // Definindo os elementos da diagonal principal como 1
        } else {
            m[i] = 0.0f;  // Todos os outros elementos são 0
        }
    }
}

// Função para multiplicar duas matrizes 4x4
void matrixMultiply4x4(float m1[16], float m2[16], float m[16]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                m[i * 4 + j] +=  m1[k * 4 + j]*m2[i * 4 + k];
            }
        }
    }
}

// Função para calcular a matriz de translação 4x4
void matrixTranslate4x4(float x, float y, float z, float m[16]) {
    matrixIdentity4x4(m);
    m[12] = x;
    m[13] = y;
    m[14] = z;
}


//Função para calcular a matriz de rotação 4x4
void matrixRotate4x4(float angle, float x, float y, float z, float m[16]) {
    matrixIdentity4x4(m);
    // Normalizar o eixo de rotação
    float magnitude = sqrt(x * x + y * y + z * z);
    float xn = x / magnitude;
    float yn = y / magnitude;
    float zn = z / magnitude;

    // Calcular seno e cosseno do ângulo (em radianos)
    float c = cos(angle * M_PI / 180.0);  // converte para radianos
    float s = sin(angle * M_PI / 180.0);

    // Preencher a matriz de rotação 4x4 em column-major order
    m[0]  = xn*xn+(1-xn*xn)*c;
    m[1]  = xn * yn * (1 - c) + zn * s;
    m[2]  = xn * zn * (1 - c) - yn * s;

    m[4]  = xn * yn * (1 - c) - zn * s;
    m[5]  = yn * yn + (1 - yn*yn)*c;
    m[6]  = yn * zn * (1 - c) + xn * s;

    m[8]  = xn * zn * (1 - c) + yn * s;
    m[9]  = yn * zn * (1 - c) - xn * s;
    m[10] = zn * zn + (1 - zn*zn)*c;

}

void matrixScale4x4(float x, float y, float z, float m[16])
{
    matrixIdentity4x4(m);
    m[0]=x;
    m[5]=y;
    m[10]=z;
}

void lookAt(float ex, float ey, float ez, float cx, float cy, float cz, float ux , float uy, float uz, float m[16])
{
    //Definindo o epsilon para tratar erros
    float epsilon = 1e-5;

   // Calculando a magnitude do vetor d
    float magnitude_d = sqrt(pow(cx - ex, 2) + pow(cy - ey, 2) + pow(cz - ez, 2));
    
    //Verificando se a magnitude_d=0
    if (magnitude_d < epsilon) {
        printf("Erro: Ponto da câmera e ponto de interesse são iguais.\n");
        exit(EXIT_FAILURE);
    }

    //Cálculo do vetor d (vetor unitário para onde a câmera aponta)
    float dx = (cx-ex)/magnitude_d;
    float dy = (cy-ey)/magnitude_d;
    float dz = (cz-ez)/magnitude_d;

    //Calculando a magnitude do vetor l
    float lx_temp = (dy*uz-dz*uy);
    float ly_temp = (dz*ux-dx*uz);
    float lz_temp = (dx*uy-dy*ux);
    float magnitude_l = sqrt(pow(lx_temp, 2) + pow(ly_temp, 2) + pow(lz_temp, 2));

    //verificando se a magnitude_l==0
    if (magnitude_l < epsilon) {
        printf("Erro: Vetor 'up' é colinear com o vetor de direção.\n");
        exit(EXIT_FAILURE);
    }

    // Cálculo do vetor l (vetor ortogonal aos vetores d e u)    
    float lx = lx_temp/magnitude_l;
    float ly = ly_temp/magnitude_l;
    float lz = lz_temp/magnitude_l;

    // Cálculo do vetor o (vetor unitário de orientação para cima da câmera)
    float ox = ly*dz-lz*dy;
    float oy = lz*dx-lx*dz;
    float oz = lx*dy-ly*dx;

   // Redefinindo a Matriz m
   m[0]=lx;
   m[1]=ox;
   m[2]=-dx;
   m[3]=0.0;

   m[4]=ly;
   m[5]=oy;
   m[6]=-dy;
   m[7]=0.0;

   m[8]=lz;
   m[9]=oz;
   m[10]=-dz;
   m[11]=0.0;

   m[12]=-ex;
   m[13]=-ey;
   m[14]=-ez;
   m[15]=1.0;
}

void ortho(float l, float r, float b, float t, float n, float f, float m[16])
{
    //Criando uma matriz identidade
    matrixIdentity4x4(m);

    //Preenchendo a matriz m com os valores corretos
    m[0]  = 2.0f/fabs(r - l);
    m[1]  = 0.0f;
    m[5]  = 2.0f/fabs(t - b);
    m[10] = -2.0f/fabs(f - n);
    m[12] = -(r + l)/fabs(r - l);
    m[13] = -(t + b)/fabs(t - b);
    m[14] = -(f + n)/fabs(f - n);
}

void frustum(float l, float r, float b, float t, float n, float f, float m[16]) {
    // Verificações das condições
    if (l == r || t == b || n == f) {
        printf("Erro: As condições l != r, t != b, n != f precisam ser verdadeiras.\n");
        exit(EXIT_FAILURE);  // Encerra o programa com erro
    }

    // Inicializa a matriz de projeção perspectiva (column first)
    m[0]  = (2 * n) / (r - l);
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;

    m[4]  = 0.0f;
    m[5]  = (2 * n) / (t - b);
    m[6]  = 0.0f;
    m[7]  = 0.0f;

    m[8]  = (r + l) / (r - l);
    m[9]  = (t + b) / (t - b);
    m[10] = -(f + n) / (f - n);
    m[11] = -1.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = -(2 * f * n) / (f - n);
    m[15] = 0.0f;
}

void quaternionCopy(float q[4], float r[4])
{
    for (int i = 0; i < 4; i++) {
        r[i] = q[i];
    }
}

float quaternionNorm(float q[4]) {
    return sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

void quaternionNormalize(float q[4])
{
    float norma=quaternionNorm(q);
    q[0] /= norma;
    q[1] /= norma;
    q[2] /= norma;
    q[3] /= norma;
}

void quaternionAdd(float q[4], float p[4], float r[4])
{
    r[0] = q[0] + p[0];
    r[1] = q[1] + p[1];
    r[2] = q[2] + p[2];
    r[3] = q[3] + p[3];
}

void Somavetores(float v[3], float u[3], float soma[3])
{
    soma[0] = v[0] + u[0];
    soma[1] = v[1] + u[1];
    soma[2] = v[2] + u[2];
}

void MultiplicaVetorPorEscalar(float v[3],float a,float r[3])
{
    r[0]=v[0]*a;
    r[1]=v[1]*a;
    r[2]=v[2]*a;
}

float produtoInterno(float v[3],float u[3])
{
    return v[0]*u[0]+v[1]*u[1]+v[2]*u[2];
}

void produtovetorial(float q[3],float p[3], float r1[3])
{
    r1[0]=q[1]*p[2]-p[1]*q[2];
    r1[1]=p[0]*q[2]-q[0]*p[2];
    r1[2]=q[0]*p[1]-p[0]*q[1];
}

void quaternionMultiply(float q[4], float p[4], float r[4])
{
    //Pegando a parte imaginaria dos quatérnios a serem multiplicados
    float v[3]={q[1],q[2],q[3]};
    float u[3]={p[1],p[2],p[3]};

    //Calculando a parte real do quaternio
    r[0]=q[0]*p[0]-produtoInterno(v,u);

    //Calculando a parte imaginaria do quaternio resposta
    float q0p[3];
    float p0q[3];
    float pxq[3];
    float soma1[3];//soma de q0p com p0q
    float soma2[3];//simboliza a parte imaginaria do quaternio resposta
    MultiplicaVetorPorEscalar(u,q[0],q0p);
    MultiplicaVetorPorEscalar(v,p[0],p0q);
    produtovetorial(v,u,pxq);
    Somavetores(q0p,p0q,soma1);
    Somavetores(soma1,pxq,soma2);

    //Juntando a parte real com a parte imaginaria
    r[1]=soma2[0];
    r[2]=soma2[1];
    r[3]=soma2[2];
}

void quaternionConjugate(float q[4], float r[4])
{
    r[0] = q[0];
    r[1] = -q[1];
    r[2] = -q[2];
    r[3] = -q[3];
}

void quaternionRotation(float angle, float x, float y,float z, float q[4])
{
    float u[4]={0,x,y,z};
    quaternionNormalize(u);//normalizando o vetor us
    //Montando o quaternion q
    q[0]=cos((angle/2)* M_PI / 180.0);
    q[1]=u[1]*sin((angle/2)* M_PI / 180.0);   
    q[2]=u[2]*sin((angle/2)* M_PI / 180.0);
    q[3]=u[3]*sin((angle/2)* M_PI / 180.0);
}

void quaternionRotationX(float angle, float q[4])
{
    float u[4]={0,1,0,0};
    quaternionNormalize(u);//normalizando o vetor us
    //Montando o quaternion q
    q[0]=cos((angle/2)* M_PI / 180.0);
    q[1]=u[1]*sin((angle/2)* M_PI / 180.0);   
    q[2]=u[2]*sin((angle/2)* M_PI / 180.0);
    q[3]=u[3]*sin((angle/2)* M_PI / 180.0);
}

void quaternionRotationY(float angle, float q[4])
{
    float u[4]={0,0,1,0};
    quaternionNormalize(u);//normalizando o vetor us
    //Montando o quaternion q
    q[0]=cos((angle/2)* M_PI / 180.0);
    q[1]=u[1]*sin((angle/2)* M_PI / 180.0);   
    q[2]=u[2]*sin((angle/2)* M_PI / 180.0);
    q[3]=u[3]*sin((angle/2)* M_PI / 180.0);
}

void quaternionRotationZ(float angle, float q[4])
{
    float u[4]={0,0,0,1};
    quaternionNormalize(u);//normalizando o vetor us
    //Montando o quaternion q
    q[0]=cos((angle/2)* M_PI / 180.0);
    q[1]=u[1]*sin((angle/2)* M_PI / 180.0);   
    q[2]=u[2]*sin((angle/2)* M_PI / 180.0);
    q[3]=u[3]*sin((angle/2)* M_PI / 180.0);
}

void quaternion2Matrix4x4(float q[4], float m[16])
{
    quaternionNormalize(q);
    // Componentes do quaternion
    float q0 = q[0];  
    float q1 = q[1];  
    float q2 = q[2];  
    float q3 = q[3];  

    // Preenche a matriz de rotação 4x4 (column first)
    m[0]  = 2.0f * q0 * q0 - 1.0f + 2.0f * q1 * q1;
    m[1]  = 2.0f * q1 * q2 + 2.0f * q0 * q3;
    m[2]  = 2.0f * q1 * q3 - 2.0f * q0 * q2;
    m[3]  = 0.0f;

    m[4]  = 2.0f * q1 * q2 - 2.0f * q0 * q3;
    m[5]  = 2.0f * q0 * q0 - 1.0f + 2.0f * q2 * q2;
    m[6]  = 2.0f * q2 * q3 + 2.0f * q0 * q1;
    m[7]  = 0.0f;

    m[8]  = 2.0f * q1 * q3 + 2.0f * q0 * q2;
    m[9]  = 2.0f * q2 * q3 - 2.0f * q0 * q1;
    m[10] = 2.0f * q0 * q0 - 1.0f + 2.0f * q3 * q3;
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
}

void matrix2Quaternion4x4(float m[16], float q[4])
{
    float r[4];
    r[0]=m[6]-m[9];
    r[1]=m[2]-m[8];
    r[2]=m[1]-m[4];

    float norma=sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
    r[0]=r[0]/norma;
    r[1]=r[1]/norma;
    r[2]=r[2]/norma;

    float angle=acos((m[0]+m[5]+m[10]-1)/2);

    q[0]=cos(angle/2);
    q[1]=r[0]*sin(angle/2);
    q[2]=r[1]*sin(angle/2);
    q[3]=r[2]*sin(angle/2);
}