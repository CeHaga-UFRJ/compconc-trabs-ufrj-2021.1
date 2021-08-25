#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "../lib/timer.h"
#include "../lib/imagelib.h"

char *inputFolderName;
char *outputFolderName;
int numFiles;

// Converte todas as imagens para escala de cinza
void convert(){
  for(int file = 1; file <= numFiles; file++){
    printf("Arquivo %d\n",file);
    int width, height;

    // Le a imagem
    char* fileNumber = getFileNumber(file);
    char* fileName   = getFileName(inputFolderName, fileNumber);

    // Converte
    unsigned char *inputImage  = getImage(fileName, &width, &height);
    unsigned char *outputImage = convertImage(inputImage, width, height);

    // E escreve
    fileName = getFileName(outputFolderName, fileNumber);
    writeImage(fileName, width, height, outputImage);
  }


}

// Inicializa as variaveis
void initialization(int argc, char *argv[]) {

  if (argc < 4) {
    printf("Digite: %s <Nome da pasta de entrada> <numero de fotos> <nome da pasta de saida>\n", argv[0]);
    exit(1);
  }

  inputFolderName = argv[1];
  numFiles = atoi(argv[2]);
  outputFolderName = argv[3];
}

int main(int argc, char *argv[]){
  double start, end, delta;

  GET_TIME(start);

  initialization(argc, argv);
  convert();

  GET_TIME(end);

  delta = end - start;
  printf("Sequencial levou %f segundos\n",delta);

  return 0;
}
