/*
 * gizmo.h
 *
 *  Created on: 29 de jan de 2021
 *      Author: JMario
 */

#ifndef INCLUDE_MYTOOLS_H_
#define INCLUDE_MYTOOLS_H_

float N_i_k(int i, int k, float u, float *x);
GLfloat* B_Spline_Curve(int n, int k, float t, GLfloat *control_points, float *knots);
void gerarMalha1D(float x_min, float x_max, int n, float *malha);
void avaliarMalhaBSpline(int n, int k, float *malha, int num_pontos, float *control_points, float *knots, GLfloat *resultados);
void multiplyRowVectorByMatrix(GLfloat* rowVector,GLfloat* matrix, GLfloat* result);
GLfloat* Hermite_Curve(float t, float *control_points);
void avaliaHermite(int num_pontos, float *malha, GLfloat *resultados, GLfloat* control_points);
void transporMatriz(float *matriz, float *transposta, int linhas, int colunas);
void MontaMalhaHermite(int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices);
GLfloat* Bezier_Curve(float t, float *control_points);
void avaliaBezier(int num_pontos, float *malha, GLfloat *resultados, GLfloat* control_points);
void extrairSubvetor(float *resultados, int j, float *subvetor);
void multiplicarMatrizPorVetor(float matriz[16], float vetor[4], float resultado[4]);
void MontaMalhaBezier(int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices);
void geraMatrizColumnFirst(float P1[], float P2[], float P3[], float P4[], float matriz[16]);
GLfloat* Bezier_Surface(float t, float s,  float *control_points_1,float *control_points_2,float *control_points_3,float *control_points_4);
void avaliaBezierSurface(int num_pontosT,int num_pontosS, float *malhat,float *malhas, GLfloat *resultados, GLfloat* control_points_1,GLfloat *control_points_2, GLfloat* control_points_3 ,GLfloat *control_points_4);
void gerarIndicesSuperficieBezier(int num_pontosT, int num_pontos_s, GLuint *indices);
void SuperficieSweepBezier(float r, int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices);
GLfloat* Reta_Curve(float t, float *control_points_Cone);
void avaliaCurvaReta(int num_pontos, float *malha, GLfloat *resultados, GLfloat* control_points_Cone);
void MontaMalhaReta(int num_pontos, GLfloat* PontosControle, float *malha,int qtdAngulos, GLfloat* vertices);
void GeraIndicesConeComTampa(int num_pontos, int qtdAngulos, GLuint *indices);
void GeraIndicesCilindroComTampa(int num_pontos, int qtdAngulos, GLuint *indices);
void MontaMalhaEsfera(int num_pontos, float radius, float *malha, int qtdAngulos, GLfloat* vertices);
void GeraIndicesEsfera(int num_pontos, int qtdAngulos, GLuint *indices);
void gerarSemicirculo(int num_pontos, float radius, float* semicirculo);
void MontaMalhaEsferaPorRevolucao(int num_pontos, float radius, int qtdAngulos, GLfloat* vertices);
void MontaIndicesEsferaPorRevolucao(int num_pontos, int qtdAngulos, GLuint* indices);
void MontaMalhaToroPorRevolucao(int num_pontos_circulo, int num_pontos_rotacao, float R, float r, GLfloat* vertices);
void MontaIndicesToroPorRevolucao(int num_pontos_circulo, int num_pontos_rotacao, GLuint* indices);
void GerarNormaisEsfera(int num_pontos, int qtdAngulos, const GLfloat* vertices, GLfloat* normals);
void GerarNormaisToro(int num_pontos_circulo, int num_pontos_rotacao, float R, float r, const GLfloat* vertices, GLfloat* normals);
void GerarNormaisConeComTampa(int num_pontos, int qtdAngulos, GLfloat *vertices, GLfloat *normals);
void MontaMalhaENormaisToroPorRevolucao(int num_pontos_circulo, int num_pontos_rotacao, 
                                float R, float r, 
                                GLfloat* vertices, GLfloat* normais);
#endif /* INCLUDE_MYTOOLS_H_ */
