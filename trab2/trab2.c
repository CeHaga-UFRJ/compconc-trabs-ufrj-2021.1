#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "../lib/timer.h"

#define INVALID_INPUT 1
#define INVALID_FILE 2
#define READING "r"
#define WRITING "w"

int C; // Numero de threads
int N; // Tamanho do bloco
char *inputFileName; // Arquivo de entrada
char *outputFileName; // Arquivo de saida
sem_t fullSlots; // Semaforo de posicoes ocupadas
sem_t emptySlots; // Semaforo de posicoes livres
pthread_mutex_t mutexCons; // Mutex para consumidores
pthread_mutex_t mutexWrite; // Mutex para escrita no arquivo
pthread_mutex_t mutexFileOver;
int *buffer[10]; // Buffer
int in; // Index dos produtores 
int out; // Index dos consumidores
int isFileOver;
FILE* inputFile;
FILE* outputFile;


FILE* getFile(char* path, char* mode) {
  FILE* arq = fopen(path, mode);
  if (arq == NULL) {
    printf("ERRO AO ABRIR ARQUIVO: %s\n", path);
    exit(INVALID_FILE);
  }
  return arq;
}

char* getLine(FILE* file) {
  char* line = (char *) malloc(sizeof(char) * 500);
  line = fgets(line, 500, file);
  return line;
}

int* stringToBlock(char* string) {
  const char* delim = " ";
  int* block = (int * ) malloc(sizeof(int) * N);
  char* token = strtok(string, delim);
  int i = 0;
  while (token != NULL) {
    block[i] = atoi(token);
    i++;
    token = strtok(NULL, delim);
  }
  return block;
}

char* blockToString(int* block) {
  char* line = (char *) malloc(sizeof(char) * 500);
  int index = 0;
  for (int i = 0; i < N; i++) {
    char aux[20];
    if (i > 0)
      line = strcat(line, " ");
    sprintf(aux, "%d", block[i]);
    line = strcat(line, aux);
    
  }
  return line;
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

void write(char* line) {
  pthread_mutex_lock(&mutexWrite);
  fprintf(outputFile, "%s\n", line);
  pthread_mutex_unlock(&mutexWrite);
}

void insert(int *block) {
  sem_wait(&emptySlots);

  buffer[in] = block;
  in = (in + 1) % N;

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
  out = (out + 1) % N;

  pthread_mutex_unlock(&mutexCons);
  sem_post(&emptySlots);
  return block;
}

void* produtor(void* arg) {
  char* line = getLine(inputFile);
  while(1) {
    line = getLine(inputFile);
    if (feof(inputFile)) {
      pthread_mutex_lock(&mutexFileOver);
      isFileOver = 1;
      pthread_mutex_unlock(&mutexFileOver);
      for (int i = 0; i < C; i++) 
        sem_post(&fullSlots);
      break;
    }
    int* block = stringToBlock(line);
    insert(block);
  }
  pthread_exit(NULL);
}

void* consumidor(void* arg) {
  while(1) {
    int* block = removeNumber();
    if (block == NULL) break;
    sort(block);
    
    char* line = blockToString(block);
    write(line);
  }
  pthread_exit(NULL);
}



void initArgs(int argc, char *argv[]){
  if(argc < 5){
    printf("ERRO DE ENTRADA. Modo de usar:\n%s <Numero de threads> <Tamanho do bloco> <Nome do arquivo de entrada> <Nome do arquivo de saida>\n",argv[0]);
    exit(INVALID_INPUT);
  }
  C = atoi(argv[1]);
  N = atoi(argv[2]);
  inputFileName = argv[3];
  outputFileName = argv[4];

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

pthread_t* createTid() { 
  pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t) * C);
  if (tid == NULL) {
    printf("ERRO--createTid");
    exit(1);
  }
  return tid;
}

pthread_t* createTidProd() {
  pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t));
  if (tid == NULL) {
    printf("ERRO--createTidProd\n");
    exit(1);
  } 
  return tid;
}

void createThreads(pthread_t* tid, pthread_t* prod) {
  if (pthread_create(prod, NULL, produtor, NULL)) {
    printf("ERRO--createThreads\n");
    exit(1);
  }
  for (int i = 0; i < C; i++) {
    if (pthread_create(tid+i, NULL, consumidor, NULL)) {
      printf("ERRO--createThreads\n");
      exit(1);
    }
  }
}

void waitThreads(pthread_t* tid, pthread_t* prod) {
  if (pthread_join(*prod, NULL)) {
    printf("ERRO--pthread_join\n");
    exit(2);
  }
  for (int i = 0; i < C; i++) {
    if (pthread_join(tid[i], NULL)) {
      printf("ERRO--pthread_join");
      exit(2);
    }
  }
}


void init() {
  pthread_t* tid  = createTid();
  pthread_t* prod = createTidProd(); 
  createThreads(tid, prod);
  waitThreads(tid, prod);
}


void endArgs(){
  for(int i = 0; i < 10; i++){
    free(buffer[i]);
  }
  //free(buffer);

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
  init();
  endArgs();
  return 0;
}
