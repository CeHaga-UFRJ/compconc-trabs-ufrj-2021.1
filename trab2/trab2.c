#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "../lib/timer.h"

#define INVALID_INPUT 1

int C; // Numero de threads
int N; // Tamanho do bloco
char *inputFileName; // Arquivo de entrada
char *outputFileName; // Arquivo de saida
sem_t fullSlots; // Semaforo de posicoes ocupadas
sem_t emptySlots; // Semaforo de posicoes livres
pthread_mutex_t mutexCons; // Mutex para consumidores
int *buffer[10]; // Buffer
int in; // Index dos produtores 
int out; // Index dos consumidores

void insert(int *block){
  sem_wait(&emptySlots);

  sem_post(&fullSlots);
}

int* remove(){
  int *block;
  sem_wait(&fullSlots);
  pthread_mutex_lock(&mutexCons);

  pthread_mutex_unlock(&mutexCons);
  sem_post(&emptySlots);
  return block;
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

  sem_init(&fullSlots, 0, 0);
  sem_init(&emptySlots, 0, C);

  pthread_mutex_init(&mutexCons);
}

void endArgs(){
  for(int i = 0; i < 10; i++){
    free(buffer[i]);
  }
  free(buffer);

  sem_destroy(&fullSlots);
  sem_destroy(&emptySlots);

  pthread_mutex_destroy(&mutexCons);
}

int main(int argc, char *argv[]){
  initArgs(argc, argv);

  return 0;
}