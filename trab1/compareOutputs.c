#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define ll long long int

#define DIMENSOES 3
#define QUALITY 90

char *seqFolderName;
char *concFolderName;
int numFiles;

char* getFileName(char* inputFolderName, char* fileNumber) {
  char *fileName = (char *)malloc(sizeof(char));
  strcpy(fileName, inputFolderName);
  strcat(fileName, fileNumber);
  return fileName;
}

char* getFileNumber(int file) {
  char *fileNumber = (char *)malloc(sizeof(char) * 16);
  sprintf(fileNumber, "%.4d.jpg", file);
  return fileNumber;
}

unsigned char* getImage(char *fileName,int* width,int* height) {
  unsigned char *img = stbi_load(fileName, width, height, NULL, DIMENSOES);
  if(!img){
    printf("ERRO--getImage\n");
    exit(1);
  }
  return img;
}

int compareImages(unsigned char* imgSeq, unsigned char* imgConc, int width, int height) {
  for(ll i = 0; i < width*height*DIMENSOES; i++){
    if (imgSeq[i] != imgConc[i])
        return 1;
  }
  return 0;
}

void compare() {
    for (int file = 1; file <= numFiles; file++) {
        int width, height;

        char* fileNumber   = getFileNumber(file);

        char* seqFileName  = getFileName(seqFolderName, fileNumber);
        char* concFileName = getFileName(concFolderName, fileNumber);

        unsigned char* seqImage  = getImage(seqFileName, &width, &height);
        unsigned char* concImage = getImage(concFileName, &width, &height);

        int different = compareImages(seqImage, concImage, width, height);

        if (different == 1) {
            printf("ERRO-- Imagens diferentes\n fileNumber: %d\n", file);
            return;
        }
    }
    printf("Todas as imagens estao iguais\n");
}

void initialization(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Digite: %s <Nome da pasta do output sequencial> <numero de fotos> <Nome da pasta do output concorrente>\n", argv[0]);
    exit(1);
  }

  seqFolderName = argv[1];
  numFiles = atoi(argv[2]);
  concFolderName = argv[3];
}

int main(int argc, char *argv[]){

  initialization(argc, argv);
  compare();

  return 0;
}
