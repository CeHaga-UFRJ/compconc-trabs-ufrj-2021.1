#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "../lib/imagelib.h"

char *seqFolderName;
char *concFolderName;
int numFiles;


// Compara as imagens geradas pelo codigo sequencial e pelo codigo concorrente
void compare() {
    for (int file = 1; file <= numFiles; file++) {
        int width, height;

        char* fileNumber   = getFileNumber(file);

        char* seqFileName  = getFileName(seqFolderName, fileNumber);
        char* concFileName = getFileName(concFolderName, fileNumber);

        // Le as imagens
        unsigned char* seqImage  = getImage(seqFileName, &width, &height);
        unsigned char* concImage = getImage(concFileName, &width, &height);

        // Compara
        int different = compareImages(seqImage, concImage, width, height);

        // Se for diferente, pode parar
        if (different == 1) {
            printf("ERRO-- Imagens diferentes\n fileNumber: %d\n", file);
            return;
        }
    }
    // Se chegou ate aqui, nao houve diferencas
    printf("Todas as imagens estao iguais\n");
}

// Inicializa as variaveis
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
