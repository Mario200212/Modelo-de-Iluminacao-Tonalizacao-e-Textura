/*
 * TestGLU.c
 *
 * Test geometric transformation functions.
 *
 *  Created on: September 07 2024
 *      Author: JMario
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>
#include "myTools2.h"
#include "auxQuestoes.h"

// Constant
// ---------------------------------------
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
const char* BASEDIR = "C:\\Users\\marce\\OneDrive\\Documentos\\ComputacaoGrafica\\Exercicio5";

// Function declaration
// ---------------------------------------
void display(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int generateShader(int type, char* filename);
void init(void);
void processInput(GLFWwindow *window);
GLFWwindow* setupEnvironment(void);


//Definicoes para as superficies B-Splines
int num_pontos;
int qtdAngulos;

// Variable declaration
// ---------------------------------------
GLuint shaderProgram;
GLuint VAO[5];
GLuint VBO1[2];
GLuint EBO1[1];
GLuint VBO2[2];
GLuint EBO2[1];
GLuint VBO3[3];
GLuint EBO3[1];
GLuint VBO4[3];
GLuint EBO4[1];
GLuint VBO5[2];
GLuint EBO5[1];

// Main: it starts here
// ---------------------------------------
int main(void) {
	fprintf(stdout, "main::start\n");
	fflush(stdout);

    // change base directory
    chdir(BASEDIR);

    // create window and load OpenGL functions
	 GLFWwindow* window = setupEnvironment();

    // set up data to display
    // ---------------------------------------
    init();

    // render data
    // ----------------------------------------
    display(window);

	fprintf(stdout, "main::end\n");
	fflush(stdout);

    exit(0);
}

// Render and display data
// ---------------------------------------
void display(GLFWwindow* window) {
    fprintf(stdout, "display::start\n");
    fflush(stdout);

    // Define a cor de fundo
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Habilita o teste de profundidade
    glEnable(GL_DEPTH_TEST);

    // Configura a matriz de projeção
    GLfloat projMatrix[16];
    frustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 5.0f, projMatrix);
    int projMatrixLoc = glGetUniformLocation(shaderProgram, "u_projMatrix");
    glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, projMatrix);

    // Configura a matriz de visualização
    GLfloat viewMatrix[16];
    lookAt(0.0f, 0.0f, 4.0f,  // Posição da câmera
           0.0f, 0.0f, 0.0f,  // Ponto de interesse
           0.0f, 1.0f, 0.0f,  // Vetor "up"
           viewMatrix);

    // Variáveis de transformação
    float angle = 0.0f;
    float step = 0.005f;

    // Localiza a variável uniforme da matriz de modelo-visão
    int modelViewMatrixLoc = glGetUniformLocation(shaderProgram, "u_modelViewMatrix");
    // Loop de renderização
    while (!glfwWindowShouldClose(window)) {
        // Processa a entrada do usuário
        processInput(window);

        // Limpa os buffers de cor e profundidade
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Dados para o shader:
        // Atualizar a posição da câmera
        GLfloat cameraX = 0.0f, cameraY = 0.0f, cameraZ = 4.0f; // Posição fixa ou dinâmica
        glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"), cameraX, cameraY, cameraZ);

        // Atualizar a posição da luz (se for dinâmica)
       GLfloat lightX = 0.0f, lightY = 2.0f, lightZ = 2.0f; // Posição fixa ou dinâmica
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightX, lightY, lightZ);

        // Atualiza o ângulo de rotação
        angle -= step;
        if (angle < -360.0f) {
            angle += 360.0f;
        }

        // ==========================
        // Renderiza o Cone
        // ==========================

        // Matrizes de transformação para o cone
        GLfloat rotationMatrixCone[16], translationMatrixCone[16];
        GLfloat modelViewMatrixCone[16], tempMatrixCone[16];

        // Rotação do cone
        matrixRotate4x4(angle, 0.0f, 1.0f, 0.0f, rotationMatrixCone);

        // Translação do cone para a posição desejada (por exemplo, x = -1.0)
        matrixTranslate4x4(-1.0f, 1.0f, 0.0f, translationMatrixCone);

        // Combina as transformações: modelViewMatrixCone = viewMatrix * translation * rotation
        matrixMultiply4x4(translationMatrixCone, rotationMatrixCone, tempMatrixCone);
        matrixMultiply4x4(viewMatrix, tempMatrixCone, modelViewMatrixCone);

        // Envia a modelViewMatrix do cone para o shader
        glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, modelViewMatrixCone);

        // Desenha o cone
        glBindVertexArray(VAO[0]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, (num_pontos - 1) * (qtdAngulos - 1) * 6 + 3 * qtdAngulos, GL_UNSIGNED_INT, 0);

        // ==========================
        // Renderiza o Cilindro
        // ==========================

        // Matrizes de transformação para o cilindro
        GLfloat rotationMatrixCilindro[16], translationMatrixCilindro[16];
        GLfloat modelViewMatrixCilindro[16], tempMatrixCilindro[16];

        // Rotação do cilindro (pode ser o mesmo ângulo ou diferente)
        matrixRotate4x4(angle, 0.0f, 1.0f, 0.0f, rotationMatrixCilindro);

        // Translada o cilindro para a posição desejada (por exemplo, x = 1.0)
        matrixTranslate4x4(1.0f, 0.5f, 0.0f, translationMatrixCilindro);

        // Combina as transformações: modelViewMatrixCilindro = viewMatrix * translation * rotation
        matrixMultiply4x4(translationMatrixCilindro, rotationMatrixCilindro, tempMatrixCilindro);
        matrixMultiply4x4(viewMatrix, tempMatrixCilindro, modelViewMatrixCilindro);

        // Envia a modelViewMatrix do cilindro para o shader
        glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, modelViewMatrixCilindro);

        // Desenha o cilindro
        glBindVertexArray(VAO[1]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, (num_pontos - 1) * (qtdAngulos - 1) * 6 + 6 * qtdAngulos, GL_UNSIGNED_INT, 0);


        // ==========================
        // Renderiza a Esfera
        // ==========================

        // Matrizes de transformação para a Esfera
        GLfloat rotationMatrixEsfera[16], translationMatrixEsfera[16];
        GLfloat modelViewMatrixEsfera[16], tempMatrixEsfera[16];

        // Rotação da Esfera
        matrixRotate4x4(angle, 0.0f, 1.0f, 0.0f, rotationMatrixEsfera);

        // Translação da Esfera para a posição desejada (por exemplo, x = -1.0)
        matrixTranslate4x4(-1.0f, 0.0f, 0.0f, translationMatrixEsfera);

        // Combina as transformações: modelo = translação * rotação
        matrixMultiply4x4(translationMatrixEsfera, rotationMatrixEsfera, tempMatrixEsfera);

        // Envia a matriz de modelo (u_modelMatrix) para o shader
        int modelMatrixLoc = glGetUniformLocation(shaderProgram, "u_modelMatrix");
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, tempMatrixEsfera);

        // Combina com a matriz de visualização: modelViewMatrix = viewMatrix * modelo
        matrixMultiply4x4(viewMatrix, tempMatrixEsfera, modelViewMatrixEsfera);

        // Envia a matriz de modelo-visão (u_modelViewMatrix) para o shader
        glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, modelViewMatrixEsfera);

        // Desenha a Esfera
        glBindVertexArray(VAO[2]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, (num_pontos - 1) * (qtdAngulos) * 6, GL_UNSIGNED_INT, 0);

        // ==========================
        // Renderiza o Toro
        // ==========================

        // Matrizes de transformação para o toro
        GLfloat rotationMatrixToro[16], translationMatrixToro[16];
        GLfloat modelViewMatrixToro[16], tempMatrixToro[16];

        // Rotação do toro
        matrixRotate4x4(angle, 0.0f, 1.0f, 0.0f, rotationMatrixToro);

        // Translação do toro para a posição desejada (por exemplo, x = -1.0)
        matrixTranslate4x4(1.0f, -1.4f, 0.0f, translationMatrixToro);

        // Combina as transformações: modelViewMatrixCone = viewMatrix * translation * rotation
        matrixMultiply4x4(translationMatrixToro, rotationMatrixToro, tempMatrixToro);
        matrixMultiply4x4(viewMatrix, tempMatrixToro, modelViewMatrixToro);

        // Envia a modelViewMatrix do toro para o shader
        glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, modelViewMatrixToro);

        // Desenha o toro
        glBindVertexArray(VAO[3]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, (num_pontos)*(qtdAngulos)*6, GL_UNSIGNED_INT, 0);


        // ==========================
        // Renderiza o Cubo
        // ==========================

        // Matrizes de transformação para o cubo
        GLfloat rotationMatrixCubo[16], translationMatrixCubo[16];
        GLfloat modelViewMatrixCubo[16], tempMatrixCubo[16];

        // Rotação do toro
        matrixRotate4x4(angle, 0.0f, 1.0f, 0.0f, rotationMatrixCubo);

        // Translação do toro para a posição desejada (por exemplo, x = -1.0)
        matrixTranslate4x4(-1.0f, -1.0f, 0.0f, translationMatrixCubo);

        // Combina as transformações: modelViewMatrixCone = viewMatrix * translation * rotation
        matrixMultiply4x4(translationMatrixCubo, rotationMatrixCubo, tempMatrixCubo);
        matrixMultiply4x4(viewMatrix, tempMatrixCubo, modelViewMatrixCubo);

        // Envia a modelViewMatrix do toro para o shader
        glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, modelViewMatrixCubo);

        // Desenha o cubo
        glBindVertexArray(VAO[4]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_POINTS,0,8);

        // Troca os buffers e processa eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Libera recursos
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(2, VBO1);
    glDeleteVertexArrays(1, EBO1);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    fprintf(stdout, "display::end\n");
    fflush(stdout);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "framebuffer_size_callback::start\n");
	fflush(stdout);

    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);

	fprintf(stdout, "framebuffer_size_callback::end\n");
	fflush(stdout);

	return;
}

// Set up OpenGL data
// ---------------------------------------
void init(void) {
	fprintf(stdout, "init::start\n");
	fflush(stdout);

	fprintf(stderr, "Renderer %s\n", glGetString(GL_RENDERER));
	fprintf(stderr, "Vendor %s\n", glGetString(GL_VENDOR));
	fprintf(stderr, "Version %s\n", glGetString(GL_VERSION));
	fprintf(stderr, "Shading Language Version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fflush(stderr);

    // Create vertex shader
	// ---------------------------------------
	GLuint vertexShader = generateShader(GL_VERTEX_SHADER, "./shaderGourant.vert");

    // Create fragment shader
	GLuint fragmentShader = generateShader(GL_FRAGMENT_SHADER, "./shaderGourant.frag");

    // Create Program and link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    // ---------------------------------------
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "init::shader program linking failed::%s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Use Program
    //-------------------------------
    glUseProgram(shaderProgram);

	// Get position of shader variables
	int vertexLoc = glGetAttribLocation(shaderProgram, "vertex");
	int colorLoc = glGetAttribLocation(shaderProgram, "color");

    //Mandando informacoes para os shaders
    glUniform3f(glGetUniformLocation(shaderProgram, "ambientColor"), 0.5f, 0.5f, 0.5f);  // Ambiente mais claro
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);  // Luz branca
    glUniform1f(glGetUniformLocation(shaderProgram, "ka"), 0.3f);  // Ambiente
    glUniform1f(glGetUniformLocation(shaderProgram, "kd"), 0.8f);  // Difuso
    glUniform1f(glGetUniformLocation(shaderProgram, "ks"), 0.5f);  // Especular
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), 32.0f);

    // Vertex position data
    // -------------------------------
    GLfloat PontosControleCone[] = {
    		0.0f, 0.5f, 0.0f, 1.0f,
			0.5f, 0.0f, 0.0f, 1.0f, 
    };

    GLfloat PontosControleCilindro[] = {
    		0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 1.0f, 0.0f, 1.0f, 
    };

    // Parâmetros da malha 1D
    num_pontos = 10;  // Número de pontos na malha
    qtdAngulos=20;
    // Calculando uma malha de pontos para as curvas que formarao as figuras por Rotacao
    float *malha = (float *)malloc(num_pontos * sizeof(float));
    // Gerar a malha 1D
    gerarMalha1D(0.0, 1.0, num_pontos, malha);

    //Calculando os Vértices e as normais das Figuras
    //Cone
    GLfloat *verticesCone = (GLfloat *)malloc(4 *(qtdAngulos* num_pontos+1) * sizeof(GLfloat));
    MontaMalhaReta(num_pontos, PontosControleCone, malha , qtdAngulos, verticesCone);
    
    //Cilindro
    GLfloat *verticesCilindro = (GLfloat *)malloc(4 *(qtdAngulos* num_pontos+2) * sizeof(GLfloat));
    MontaMalhaReta(num_pontos, PontosControleCilindro, malha , qtdAngulos, verticesCilindro);
    
    //Esfera
    GLfloat *verticesEsfera = (GLfloat *)malloc(4 *qtdAngulos* num_pontos * sizeof(GLfloat));
    MontaMalhaEsferaPorRevolucao(num_pontos, 0.5,qtdAngulos, verticesEsfera);
    GLfloat *normalsEsfera = (GLfloat *)malloc(4 * num_pontos * qtdAngulos * sizeof(GLfloat));
    GerarNormaisEsfera(num_pontos, qtdAngulos, verticesEsfera, normalsEsfera);

    //TORO
    GLfloat *verticesToro = (GLfloat *)malloc(4 *qtdAngulos* num_pontos * sizeof(GLfloat));
    MontaMalhaToroPorRevolucao(num_pontos, qtdAngulos,0.5, 0.25, verticesToro);
    GLfloat *normalsToro = (GLfloat *)malloc(4 * num_pontos * qtdAngulos * sizeof(GLfloat));
    GerarNormaisToro(num_pontos, qtdAngulos, 0.5f, 0.25f, verticesToro, normalsToro);
    
    //Cubo
    float L=0.5f;
    GLfloat verticesCubo[] = {
    // Coordenadas dos vértices (x, y, z, w)
    -L/2, -L/2, -L/2, 1.0f, // Vértice 0
     L/2, -L/2, -L/2, 1.0f, // Vértice 1
     L/2,  L/2, -L/2, 1.0f, // Vértice 2
    -L/2,  L/2, -L/2, 1.0f, // Vértice 3
    -L/2, -L/2,  L/2, 1.0f, // Vértice 4
     L/2, -L/2,  L/2, 1.0f, // Vértice 5
     L/2,  L/2,  L/2, 1.0f, // Vértice 6
    -L/2,  L/2,  L/2, 1.0f  // Vértice 7
};
    ///////////////

    // Acrescentando os pontos das tampas das figuras

    //Cone
    verticesCone[4*qtdAngulos*num_pontos]=0.0f;
    verticesCone[4*qtdAngulos*num_pontos+1]=0.0f;
    verticesCone[4*qtdAngulos*num_pontos+2]=0.0f;
    verticesCone[4*qtdAngulos*num_pontos+3]=1.0f;

    //Cilindro
    verticesCilindro[4*qtdAngulos*num_pontos]=0.0f;
    verticesCilindro[4*qtdAngulos*num_pontos+1]=0.0f;
    verticesCilindro[4*qtdAngulos*num_pontos+2]=0.0f;
    verticesCilindro[4*qtdAngulos*num_pontos+3]=1.0f;

    verticesCilindro[4*(qtdAngulos*num_pontos+1)]=0.0f;
    verticesCilindro[4*(qtdAngulos*num_pontos+1)+1]=1.0f;
    verticesCilindro[4*(qtdAngulos*num_pontos+1)+2]=0.0f;
    verticesCilindro[4*(qtdAngulos*num_pontos+1)+3]=1.0f;

    ///////////

    // Vertex color Data
    // -------------------------------
    // Alocando as Cores das figuras

    //Cone
    GLfloat *colorsCone = (GLfloat *)malloc(4* (num_pontos * qtdAngulos+1) * sizeof(GLfloat));
   // Preencher as cores para cada ponto (no exemplo, todos serão azuis)
    for (int i = 0; i < num_pontos*qtdAngulos+1; i++) {
        colorsCone[4 * i + 0] = 0.0f;  // Red
        colorsCone[4 * i + 1] = 0.0f;  // Green
        colorsCone[4 * i + 2] = 1.0f;  // Blue
        colorsCone[4 * i + 3] = 1.0f; 
    }

    //Cilindro
    GLfloat *colorsCilindro = (GLfloat *)malloc(4* (num_pontos * qtdAngulos+2) * sizeof(GLfloat));
   // Preencher as cores para cada ponto (no exemplo, todos serão azuis)
    for (int i = 0; i < num_pontos*qtdAngulos+2; i++) {
        colorsCilindro[4 * i + 0] = 1.0f;  // Red
        colorsCilindro[4 * i + 1] = 0.0f;  // Green
        colorsCilindro[4 * i + 2] = 0.0f;  // Blue
        colorsCilindro[4 * i + 3] = 1.0f; 
    }

    //Esfera
    GLfloat *colorsEsfera = (GLfloat *)malloc(4* num_pontos * qtdAngulos * sizeof(GLfloat));
    //GLfloat *colors = (GLfloat *)malloc(4* num_pontos * sizeof(GLfloat));

    // Preencher as cores para cada ponto (no exemplo, todos serão azuis)
    for (int i = 0; i < num_pontos*qtdAngulos; i++) {
        colorsEsfera[4 * i + 0] = 0.0f;  // Red
        colorsEsfera[4 * i + 1] = 1.0f;  // Green
        colorsEsfera[4 * i + 2] = 0.0f;  // Blue
        colorsEsfera[4 * i + 3] = 1.0f; 
    }

    //TORO
    GLfloat *colorsToro = (GLfloat *)malloc(4* num_pontos * qtdAngulos * sizeof(GLfloat));
    // Preencher as cores para cada ponto (no exemplo, todos serão azuis)
    for (int i = 0; i < num_pontos*qtdAngulos; i++) {
        colorsToro[4 * i + 0] = 0.0f;  // Red
        colorsToro[4 * i + 1] = 1.0f;  // Green
        colorsToro[4 * i + 2] = 1.0f;  // Blue
        colorsToro[4 * i + 3] = 1.0f; 
    }

    //Cubo
    GLfloat *colorsCubo = (GLfloat *)malloc(4* 8 * sizeof(GLfloat));
    // Preencher as cores para cada ponto (no exemplo, todos serão azuis)
    for (int i = 0; i < 8; i++) {
        colorsCubo[4 * i + 0] = 1.0f;  // Red
        colorsCubo[4 * i + 1] = 0.0f;  // Green
        colorsCubo[4 * i + 2] = 1.0f;  // Blue
        colorsCubo[4 * i + 3] = 1.0f; 
    }
	// Create Vertex Array Object ID
	// ---------------------------------------
    glGenVertexArrays(5, VAO);

    //CONE:

    // Create/activate a Vertex Array Object and bind to ID
    // ---------------------------------------
    glBindVertexArray(VAO[0]);

	// Generate Buffer IDs for 2 buffers
    // ---------------------------------------
    glGenBuffers(2, VBO1);

    // Create/activate a Buffer and bind to ID VBO1[0]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO1[0]);

    // Initialize Buffer with vertex position data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4 * (num_pontos* qtdAngulos+1) * sizeof(GLfloat), verticesCone, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos* sizeof(GLfloat), curva1, GL_STATIC_DRAW);

    // Associate vertex data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(0);

    // Create/activate a Buffer and bind to ID VBO1[1]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO1[1]);

    // Initialize Buffer with color data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4*(num_pontos*qtdAngulos+1)* sizeof(GLfloat), colorsCone, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, num_pontos * 4 * sizeof(GLfloat), colorsCone, GL_STATIC_DRAW);

    // Associate color data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(1);

    GLuint *indicesCone = (GLuint *)malloc(((num_pontos - 1) * (qtdAngulos - 1) * 6+3*qtdAngulos) * sizeof(GLuint));
    GeraIndicesConeComTampa(num_pontos, qtdAngulos, indicesCone);
    
    // Initialize Buffer 
    // ---------------------------------------
    glGenBuffers(1, EBO1);

    // Create/activate a Buffer and bind to ID VBO1[0]
    // ---------------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1[0]);

    // Initialize Buffer 
    // ---------------------------------------
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((num_pontos - 1) * (qtdAngulos - 1) * 6 + qtdAngulos * 3) * sizeof(GLuint), indicesCone, GL_STATIC_DRAW);

    //CILINDRO:

    // Create/activate a Vertex Array Object and bind to ID
    // ---------------------------------------
    glBindVertexArray(VAO[1]);

	// Generate Buffer IDs for 2 buffers
    // ---------------------------------------
    glGenBuffers(2, VBO2);

    // Create/activate a Buffer and bind to ID VBO1[0]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO2[0]);

    // Initialize Buffer with vertex position data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4 * (num_pontos* qtdAngulos+2) * sizeof(GLfloat), verticesCilindro, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos* sizeof(GLfloat), curva1, GL_STATIC_DRAW);

    // Associate vertex data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(0);

    // Create/activate a Buffer and bind to ID VBO1[1]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO2[1]);

    // Initialize Buffer with color data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4*(num_pontos*qtdAngulos+2)* sizeof(GLfloat), colorsCilindro, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, num_pontos * 4 * sizeof(GLfloat), colorsCone, GL_STATIC_DRAW);

    // Associate color data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(1);

    GLuint *indicesCilindro = (GLuint *)malloc(((num_pontos - 1) * (qtdAngulos - 1) * 6+6*qtdAngulos) * sizeof(GLuint));
    GeraIndicesCilindroComTampa(num_pontos, qtdAngulos, indicesCilindro);
    
    // Initialize Buffer 
    // ---------------------------------------
    glGenBuffers(1, EBO2);

    // Create/activate a Buffer and bind to ID VBO1[0]
    // ---------------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2[0]);

    // Initialize Buffer 
    // ---------------------------------------
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((num_pontos - 1) * (qtdAngulos - 1) * 6 + qtdAngulos * 6) * sizeof(GLuint), indicesCilindro, GL_STATIC_DRAW);
	
    
    //ESFERA
    // Create/activate a Vertex Array Object and bind to ID
    // ---------------------------------------
    glBindVertexArray(VAO[2]);

	// Generate Buffer IDs for 2 buffers
    // ---------------------------------------
    glGenBuffers(3, VBO3);

    // Create/activate a Buffer and bind to ID VBO[0]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO3[0]);

    // Initialize Buffer with vertex position data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos* qtdAngulos * sizeof(GLfloat), verticesEsfera, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos* sizeof(GLfloat), curva1, GL_STATIC_DRAW);

    // Associate vertex data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(0);

    // Create/activate a Buffer and bind to ID VBO[1]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO3[1]);

    // Initialize Buffer with color data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, num_pontos * 4 *qtdAngulos* sizeof(GLfloat), colorsEsfera, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, num_pontos * 4 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

    // Associate color data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(1);

    //Normais Esfera:
    // Cria e ativa o buffer para as normais
    glBindBuffer(GL_ARRAY_BUFFER, VBO3[2]);

    // Inicializa o buffer com os dados das normais
    glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos * qtdAngulos * sizeof(GLfloat), normalsEsfera, GL_STATIC_DRAW);

    // Associa os dados de normais ao atributo no shader (location = 2)
    int normalLoc = glGetAttribLocation(shaderProgram, "normal");
    glVertexAttribPointer(normalLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(normalLoc);

    // Libera a memória usada para calcular as normais
    free(normalsEsfera);

    GLuint *indicesEsfera = (GLuint *)malloc((num_pontos - 1) * (qtdAngulos) * 6 * sizeof(GLuint));
    MontaIndicesEsferaPorRevolucao(num_pontos,qtdAngulos,indicesEsfera);

    
    // Initialize Buffer 
    // ---------------------------------------
    glGenBuffers(1, EBO3);

    // Create/activate a Buffer and bind to ID VBO[0]
    // ---------------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3[0]);

    // Initialize Buffer 
    // ---------------------------------------
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (num_pontos - 1) * (qtdAngulos) * 6*sizeof(GLint), indicesEsfera, GL_STATIC_DRAW);

    //Toro
    // Create/activate a Vertex Array Object and bind to ID
    // ---------------------------------------
    glBindVertexArray(VAO[3]);

	// Generate Buffer IDs for 2 buffers
    // ---------------------------------------
    glGenBuffers(2, VBO4);

    // Create/activate a Buffer and bind to ID VBO4[0]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO4[0]);

    // Initialize Buffer with vertex position data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos* qtdAngulos * sizeof(GLfloat), verticesToro, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos* sizeof(GLfloat), curva1, GL_STATIC_DRAW);

    // Associate vertex data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(0);

    // Create/activate a Buffer and bind to ID VBO4[1]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO4[1]);

    // Initialize Buffer with color data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, num_pontos * 4 *qtdAngulos* sizeof(GLfloat), colorsToro, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, num_pontos * 4 * sizeof(GLfloat), colorsToro, GL_STATIC_DRAW);

    // Associate color data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(1);

    GLuint *indicesToro = (GLuint *)malloc((num_pontos) * (qtdAngulos) * 6 * sizeof(GLuint));
    MontaIndicesToroPorRevolucao(num_pontos,qtdAngulos,indicesToro);

    
    // Initialize Buffer 
    // ---------------------------------------
    glGenBuffers(1, EBO4);

    // Create/activate a Buffer and bind to ID VBO4[0]
    // ---------------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4[0]);

    // Initialize Buffer 
    // ---------------------------------------
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (num_pontos) * (qtdAngulos) * 6*sizeof(GLint), indicesToro, GL_STATIC_DRAW);


    //CUBO:
    // Create/activate a Vertex Array Object and bind to ID
    // ---------------------------------------
    glBindVertexArray(VAO[4]);

	// Generate Buffer IDs for 2 buffers
    // ---------------------------------------
    glGenBuffers(2, VBO5);

    // Create/activate a Buffer and bind to ID VBO1[0]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO5[0]);

    // Initialize Buffer with vertex position data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(GLfloat), verticesCubo, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 4 * num_pontos* sizeof(GLfloat), curva1, GL_STATIC_DRAW);

    // Associate vertex data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(0);

    // Create/activate a Buffer and bind to ID VBO1[1]
    // ---------------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO5[1]);

    // Initialize Buffer with color data
    // ---------------------------------------
    glBufferData(GL_ARRAY_BUFFER, 4*8* sizeof(GLfloat), colorsCubo, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, num_pontos * 4 * sizeof(GLfloat), colorsCone, GL_STATIC_DRAW);

    // Associate color data to vertex shader input
    // ---------------------------------------
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    // Enable vertex data to be transfered to shader
    // ---------------------------------------
    glEnableVertexAttribArray(1);

    GLuint indicesCubo[] = {
    // Face frontal
    4, 5, 6,
    6, 7, 4,
    // Face traseira
    0, 1, 2,
    2, 3, 0,
    // Face esquerda
    0, 4, 7,
    7, 3, 0,
    // Face direita
    1, 5, 6,
    6, 2, 1,
    // Face superior
    3, 2, 6,
    6, 7, 3,
    // Face inferior
    0, 1, 5,
    5, 4, 0
};
    
    // Initialize Buffer 
    // ---------------------------------------
    glGenBuffers(1, EBO5);

    // Create/activate a Buffer and bind to ID VBO1[0]
    // ---------------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5[0]);

    // Initialize Buffer 
    // ---------------------------------------
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), indicesCubo, GL_STATIC_DRAW);


    fprintf(stdout, "init::end\n");
	fflush(stdout);
     // Liberar memória
    free(verticesCone);
    free(colorsCone);
    free(verticesCilindro);
    free(colorsCilindro);
    return;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
//	fprintf(stdout, "processInput::start\n");
//	fflush(stdout);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

//	fprintf(stdout, "processInput::end\n");
//	fflush(stdout);
}

// Create window and load OpenGL library
// ---------------------------------------
    GLFWwindow* setupEnvironment(void) {
	fprintf(stdout, "prepare::start\n");
	fflush(stdout);

    // glfw: initialize and configure
    // ---------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // ---------------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TestGLU", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "prepare::failed to create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
    	fprintf(stderr, "prepare::failed to initialize GLAD");
        exit(EXIT_FAILURE);
    }
    return window;
}
