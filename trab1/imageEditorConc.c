#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "../lib/timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"

#define ll long long int

#define DIMENSOES 3 // Numero de cores na imagem
#define QUALITY 90 // Qualidade a ser salva

char *inputFolderName;
char *outputFolderName;
int nthreads;
int numFiles;

// Monta o nome do arquivo
char* getFileName(char* inputFolderName, char* fileNumber) {
  char *fileName = (char *)malloc(sizeof(char));
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

// Tarefa assincrona
void *task(void *arg){
  ll id = (ll)arg;
  printf("Começou thread %lld\n",id);

  for(int file = id+1; file <= numFiles; file+=nthreads){
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

  pthread_exit(NULL);
}

// Inicializa as variaveis
void initialization(int argc, char *argv[]) {

  if (argc < 4) {
    printf("Digite: %s <Nome da pasta de entrada> <numero de fotos> <nome da pasta de saida> <numero de threads>\n", argv[0]);
    exit(1);
  }

  inputFolderName = argv[1];
  numFiles = atoi(argv[2]);
  outputFolderName = argv[3];
  nthreads = atoi(argv[4]);
}

// Cria o vetor tid
pthread_t* createTid() {
  pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if (tid == NULL) {
    printf("ERRO--createTid");
    exit(1);
  }
  return tid;
}

// Cria as threads
void createThreads(pthread_t* tid) {
  for(ll i = 0; i < nthreads; i++){
    if(pthread_create(tid+i, NULL, task, (void *)i)){
      printf("ERRO--pthread_create\n");
      exit(2);
    }
  }
}

// Une as threads
void waitThreads(pthread_t* tid) {
  for(ll i = 0; i < nthreads; i++){
    if(pthread_join(tid[i],NULL)){
      printf("ERRO--pthread_join\n");
      exit(2);
    }
  }
}

// Processo completo das threads
void convert() {
  pthread_t* tid = createTid();
  createThreads(tid);
  waitThreads(tid);
}


int main(int argc, char *argv[]){
  double start, end, delta;

  GET_TIME(start);

  initialization(argc, argv);
  convert();

  GET_TIME(end);

  delta = end - start;
  printf("Concorrente levou %f segundos\n",delta);

  return 0;
}

