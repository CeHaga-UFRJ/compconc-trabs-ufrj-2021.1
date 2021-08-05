#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "lib/timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

#define ll long long int

#define DIMENSOES 3
#define QUALITY 90

char *inputFolderName;
char *outputFolderName;
int nthreads;
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

void writeImage(char *fileName, int width, int height, unsigned char *output) {
  if(stbi_write_jpg(fileName, width, height, 1, output, QUALITY) == 0){
    printf("ERRO--writeImage\n");
    exit(3);
  }
}

void *task(void *arg){
  ll id = (ll)arg;
  printf("Começou thread %lld\n",id);

  for(int file = id+1; file <= numFiles; file+=nthreads){
    int width, height;

    char* fileNumber = getFileNumber(file);
    char* fileName   = getFileName(inputFolderName, fileNumber);

    unsigned char *inputImage  = getImage(fileName, &width, &height);
    unsigned char *outputImage = convertImage(inputImage, width, height);


    fileName = getFileName(outputFolderName, fileNumber);
    writeImage(fileName, width, height, outputImage);
  }

  pthread_exit(NULL);
}

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

pthread_t* createTid() {
  pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if (tid == NULL) {
    printf("ERRO--createTid");
    exit(1);
  }
  return tid;
}

void createThreads(pthread_t* tid) {
  for(ll i = 0; i < nthreads; i++){
    if(pthread_create(tid+i, NULL, task, (void *)i)){
      printf("ERRO--pthread_create\n");
      exit(2);
    }
  }
}

void waitThreads(pthread_t* tid) {
  for(ll i = 0; i < nthreads; i++){
    if(pthread_join(tid[i],NULL)){
      printf("ERRO--pthread_join\n");
      exit(2);
    }
  }
}

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
  printf("Tudo levou %f segundos\n",delta);

  return 0;
}

