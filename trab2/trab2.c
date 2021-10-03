#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "../lib/timer.h"

#define INVALID_INPUT 1
#define INVALID_FILE 2
#define CREATE_THREAD_ERROR 3
#define JOIN_THREAD_ERROR 4
#define CREATE_TID_ERROR 5

#define READING "r"
#define WRITING "w"

#define BUFFER_SIZE 10

int C; // Numero de threads
int N; // Tamanho do bloco
int *buffer[BUFFER_SIZE]; // Buffer
int in; // Index dos produtores 
int out; // Index dos consumidores
int isFileOver; // Condicao do arquivo ter acabado

FILE* inputFile; // Arquivo de entrada
FILE* outputFile; // Arquivo de saida

sem_t fullSlots; // Semaforo de posicoes ocupadas
sem_t emptySlots; // Semaforo de posicoes livres

pthread_mutex_t mutexCons; // Mutex para consumidores
pthread_mutex_t mutexWrite; // Mutex para escrita no arquivo
pthread_mutex_t mutexFileOver; // Mutex para terminação da producao

/*
* FUNCOES RELACIONADAS A ARQUIVO
*/

FILE* getFile(char* path, char* mode) {
  FILE* arq = fopen(path, mode);
  if (arq == NULL) {
    printf("ERRO AO ABRIR ARQUIVO: %s\n", path);
    exit(INVALID_FILE);
  }
  return arq;
}

int getTotalLines(FILE* file){
  int n;
  fscanf(file, "%d", &n);
  return n/N;
}

int* getBlock(FILE* file) {
  int *block = (int *)malloc(sizeof(int) * N);
  for(int i = 0; i < N; i++){
    fscanf(file, "%d", block+i);
  }
  return block;
}

void sort(int* block) {
  for (int i = 0; i < N - 1; i++) {
    for (int j = i + 1; j < N; j++) {
      if (block[i] > block[j]) {
        int aux  = block[i];
          block[i] = block[j];
          block[j] = aux;
      }
    }
  }
}

void write(FILE *file, int* block) {
  pthread_mutex_lock(&mutexWrite);
  for(int i = 0; i < N; i++){
    fprintf(file, "%d ", block[i]);
  }
  fprintf(file, "\n");
  pthread_mutex_unlock(&mutexWrite);
}

void setFileOver(){
  pthread_mutex_lock(&mutexFileOver);
  isFileOver = 1;
  pthread_mutex_unlock(&mutexFileOver);
}

void insert(int *block) {
  sem_wait(&emptySlots);

  buffer[in] = block;
  in = (in + 1) % BUFFER_SIZE;

  sem_post(&fullSlots);
}

int* removeNumber(){
  int *block;
  sem_wait(&fullSlots);
  pthread_mutex_lock(&mutexCons);
 
 
  pthread_mutex_lock(&mutexFileOver);
  if (isFileOver && in == out) {
    pthread_mutex_unlock(&mutexCons);
    pthread_mutex_unlock(&mutexFileOver);
    return NULL;
  }
  pthread_mutex_unlock(&mutexFileOver);

  
  block = buffer[out];
  out = (out + 1) % BUFFER_SIZE;

  pthread_mutex_unlock(&mutexCons);
  sem_post(&emptySlots);
  return block;
}

/*
* FUNCOES RELACIONADAS A THREADS
*/

void* produtor(void* arg) {
  int lines = getTotalLines(inputFile);
  for(int i = 0; i < lines; i++) {
    int* block = getBlock(inputFile);
    insert(block);
  }
  setFileOver();
  for (int i = 0; i < C; i++) 
    sem_post(&fullSlots);
  pthread_exit(NULL);
}

void* consumidor(void* arg) {
  while(1) {
    int* block = removeNumber();
    if (block == NULL) break;
    sort(block);
    write(outputFile,block);
  }
  pthread_exit(NULL);
}
pthread_t* createTid(int n) { 
  pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t) * n);
  if (tid == NULL) {
    printf("ERRO--createTid");
    exit(CREATE_TID_ERROR);
  }
  return tid;
}

void createThreads(pthread_t* tid, int n, void *task) {
  for (int i = 0; i < n; i++) {
    if (pthread_create(tid+i, NULL, task, NULL)) {
      printf("ERRO--createThreads\n");
      exit(CREATE_THREAD_ERROR);
    }
  }
}

void waitThreads(pthread_t* tid, int n) {
  for (int i = 0; i < n; i++) {
    if (pthread_join(tid[i], NULL)) {
      printf("ERRO--pthread_join");
      exit(JOIN_THREAD_ERROR);
    }
  }
}

void run() {
  pthread_t* prod  = createTid(1);
  pthread_t* cons = createTid(C); 

  createThreads(prod, 1, produtor);
  createThreads(cons, C, consumidor);
  
  waitThreads(prod, 1);
  waitThreads(cons, C);
}

/*
* FUNCOES RELACIONADAS A INICIALIZACAO DO PROGRAMA
*/

void initArgs(int argc, char *argv[]){
  if(argc < 5){
    printf("ERRO DE ENTRADA. Modo de usar:\n%s <Numero de threads> <Tamanho do bloco> <Nome do arquivo de entrada> <Nome do arquivo de saida>\n",argv[0]);
    exit(INVALID_INPUT);
  }
  C = atoi(argv[1]);
  N = atoi(argv[2]);
  char *inputFileName = argv[3];
  char *outputFileName = argv[4];

  in = 0;
  out = 0;
  isFileOver = 0;

  sem_init(&fullSlots, 0, 0);
  sem_init(&emptySlots, 0, C);

  pthread_mutex_init(&mutexCons, NULL);
  pthread_mutex_init(&mutexWrite, NULL);
  pthread_mutex_init(&mutexFileOver, NULL);

  inputFile  = getFile(inputFileName, READING);
  outputFile = getFile(outputFileName, WRITING); 
}

void endArgs(){
  for(int i = 0; i < BUFFER_SIZE; i++){
    free(buffer[i]);
  }

  sem_destroy(&fullSlots);
  sem_destroy(&emptySlots);

  pthread_mutex_destroy(&mutexCons);
  pthread_mutex_destroy(&mutexWrite);
  pthread_mutex_destroy(&mutexFileOver);

  fclose(inputFile);
  fclose(outputFile);
}

int main(int argc, char *argv[]){
  initArgs(argc, argv);
  run();
  endArgs();
  return 0;
}
