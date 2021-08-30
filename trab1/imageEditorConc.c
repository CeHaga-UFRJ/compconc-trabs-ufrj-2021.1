#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "../lib/timer.h"
#include "../lib/imagelib.h"

char *inputFolderName;
char *outputFolderName;
int nthreads;
int numFiles;

// Tarefa assincrona
void *task(void *arg){
  ll id = (ll)arg;
  printf("Comecou thread %lld\n",id);

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

  if (argc != 5) {
    printf("Digite: %s <Nome da pasta de entrada> <Numero de fotos> <Nome da pasta de saida> <Numero de threads>\n", argv[0]);
    exit(1);
  }

  inputFolderName = formatFolderName(argv[1]);
  numFiles = atoi(argv[2]);
  outputFolderName = formatFolderName(argv[3]);
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

