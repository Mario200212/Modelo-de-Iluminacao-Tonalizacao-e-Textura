/*
 * mytools.c
 *
 *  Created on: 29 de jan de 2021
 *      Author: JMario
 */
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad/glad.h>


// generateShader: reads shader code from ASCII file and compiles code
// ---------------------------------------------------------------------------------------------
unsigned int generateShader(int type, char* filename ) {
	int shader;

	fprintf(stdout, "generateShader::start\n");
	fflush(stdout);

	// input shader file
	FILE *file;
	if ((file = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "ERROR: generateShader::fopen::%s\n", filename);
		exit(1);
	}

	// get size
	fseek(file, 0L, SEEK_END);
	int fileSize = ftell(file);
	rewind(file);

	// read file. One aditional place to close the string (null)
	char *shaderText = malloc((fileSize + 1) * sizeof(char));
	char *shaderSource[] = {(char *) shaderText};

	if(fread(shaderText, sizeof(char), fileSize, file) != fileSize) {
		fprintf(stderr, "ERROR generateShader::fread::%s\n", filename);
		exit(1);
	}
	
	shaderText[fileSize] = '\0';
	
	// close file
	if(fclose(file)) {
		fprintf(stderr, "ERROR generateShader::fclose::%s\n", filename);
		exit(1);
	}

	// Create GPU shader handle
	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char * const*) shaderSource, NULL);

	glCompileShader(shader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		fprintf(stderr, "ERROR generateShader::shader compilation failed:: %s\n", infoLog);
		exit(1);
	}

	printf("generateShader::end\n");
	fflush(stdout);
	return shader;
}




