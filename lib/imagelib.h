#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"

#define ll long long int
#define DIMENSOES 3 // Numero de cores na imagem
#define QUALITY 90 // Qualidade a ser salva

char* getFileName(char* inputFolderName, char* fileNumber);
char* getFileNumber(int file);
unsigned char* getImage(char *fileName,int* width,int* height);
unsigned char* convertImage(unsigned char* img, int width, int height);
void writeImage(char *fileName, int width, int height, unsigned char *output);
int compareImages(unsigned char* imgSeq, unsigned char* imgConc, int width, int height);

// Monta o nome do arquivo
char* getFileName(char* inputFolderName, char* fileNumber) {
  char *fileName = (char *)malloc(sizeof(char) * 32);
  strcpy(fileName, inputFolderName);
  strcat(fileName, fileNumber);
  return fileName;
}

// Converte um número para o formato "0000"
char* getFileNumber(int file) {
  char *fileNumber = (char *)malloc(sizeof(char) * 16);
  sprintf(fileNumber, "%.4d.jpg", file);
  return fileNumber;
}

// Abre uma imagem e define os parâmetros width e height
unsigned char* getImage(char *fileName,int* width,int* height) {
  unsigned char *img = stbi_load(fileName, width, height, NULL, DIMENSOES);
  if(!img){
    printf("ERRO--getImage\n");
    exit(1);
  }
  return img;
}

// Converte a imagem para escala de cinza
unsigned char* convertImage(unsigned char* img, int width, int height) {
  unsigned char *output = (unsigned char *)malloc(sizeof(unsigned char) * width * height);
  for(ll i = 0, k = 0; i < width*height*DIMENSOES; i+=DIMENSOES, k++){
    unsigned char red = img[i];
    unsigned char green = img[i + 1];
    unsigned char blue = img[i + 2];
    output[k] = (unsigned char)(red * 30.0/100.0 + green * 59.0/100.0 + blue * 11.0/100.0);
  }
  return output;
}

// Escreve a imagem de saida
void writeImage(char *fileName, int width, int height, unsigned char *output) {
  if(stbi_write_jpg(fileName, width, height, 1, output, QUALITY) == 0){
    printf("ERRO--writeImage\n");
    exit(3);
  }
}

// Retorna se duas imagens sao diferentes
int compareImages(unsigned char* imgSeq, unsigned char* imgConc, int width, int height) {
  for(ll i = 0; i < width*height*DIMENSOES; i++){
    if (imgSeq[i] != imgConc[i])
        return 1;
  }
  return 0;
}

