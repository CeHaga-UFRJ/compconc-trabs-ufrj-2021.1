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

unsigned char **output;
char *inputFolderName;
char *outputFolderName;
int nthreads;
int width;
int height;
int numFiles;

void *task(void *arg){
  ll id = (ll)arg;
  printf("Começou thread %lld\n",id);
  for(int t = id+1; t <= numFiles; t+=nthreads){
    char *fileName = (char *)malloc(sizeof(char));
    strcpy(fileName, inputFolderName);
    char *fileNumber = (char *)malloc(sizeof(char) * 16);
    sprintf(fileNumber, "%.4d.jpg", t);
    strcat(fileName, fileNumber);
    // printf("Reading %s\n",fileName);
    unsigned char *img = stbi_load(fileName, &width, &height, NULL, 3);
    if(!img){
      printf("Deu merda\n");
      exit(1);
    }
    unsigned char *output = (unsigned char *)malloc(sizeof(unsigned char) * width * height);
    for(ll i = 0, k = 0; i < width*height*3; i+=3, k++){
      unsigned char red = img[i];
      unsigned char green = img[i + 1];
      unsigned char blue = img[i + 2];
      output[k] = (unsigned char)(red * 30.0/100.0 + green * 59.0/100.0 + blue * 11.0/100.0);
    }
    strcpy(fileName, outputFolderName);
    strcat(fileName, fileNumber);
    if(stbi_write_jpg(fileName, width, height, 1, output, 90) == 0){
      printf("Não para de dar merda\n");
      exit(3);
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  double start, end, delta;

  GET_TIME(start);

  inputFolderName = argv[1];
  numFiles = atoi(argv[2]);
  outputFolderName = argv[3];
  nthreads = atoi(argv[4]);

  pthread_t tid[nthreads];



  output = (unsigned char **)malloc(sizeof(unsigned char *) * numFiles);
  if(!output){
    printf("Não aguento mais tanta merda\n");
    exit(3);
  }

  for(ll i = 0; i < nthreads; i++){
    if(pthread_create(tid+i, NULL, task, (void *)i)){
      printf("Continua dando merda\n");
      exit(2);
    }
  }

  for(ll i = 0; i < nthreads; i++){
    if(pthread_join(tid[i],NULL)){
      printf("Merda mais uma vez\n");
      exit(2);
    }
  }

  // for(int i = 1; i <= numFiles; i++){
  //   char *fileName = (char *)malloc(sizeof(char));
  //   strcpy(fileName, outputFolderName);
  //   char *fileNumber = (char *)malloc(sizeof(char) * 16);
  //   sprintf(fileNumber, "%.4d.jpg", i);
  //   strcat(fileName, fileNumber);
  //   // printf("Writing %s\n",fileName);
  //   if(stbi_write_jpg(fileName, width, height, 1, output[i-1], 90) == 0){
  //     printf("Não para de dar merda\n");
  //     exit(3);
  //   }
  // }


  printf("Foi\n");

  GET_TIME(end);
  delta = end - start;
  printf("Tudo levou %f segundos\n",delta);

  return 0;
}
