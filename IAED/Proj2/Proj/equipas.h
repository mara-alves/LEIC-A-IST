/*
 * Ficheiro equipas.h
 * Aluna 95625, Mara Gomes Alves
 * Ficheiro com a estrutura equipa, um vetor e uma hashtable de equipas
 */

#ifndef EQUIPAS_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define TRUE 1
#define FALSE 0

/* Estrutura Equipa -------------------------------------------------------- */

typedef struct equipa{
    char * nome;
    int vitorias;
} * Equipa;


/* Vetor de Equipas -------------------------------------------------------- */

typedef struct vetor{
  Equipa * vetor;
  int N; /* numero de posicoes ocupadas no vetor */
  int M; /* tamanho do vetor */
} * Vetor;


/* Hashtable --------------------------------------------------------------- */

typedef struct hashtable_eq{
    Equipa * tabela;
    int N; /* numero de elementos */
    int M; /* tamanho da hashtable */
} * Hashtable_eq;


/* chave eh o nome da equipa */
typedef char* Chave_Equipas; 
#define iguais(a,b) (strcmp(a,b)==0)

#define vitoria(eq) (eq->vitorias++) /* adiciona uma vitoria a uma equipa */
#define perde_vitoria(eq) (eq->vitorias--) /* remove uma vitoria a uma equipa */


/* Prototipos -------------------------------------------------------------- */

/* imprime uma equipa */
void imprime_equipa(Equipa equipa);

/* funcoes para encontrar o menor numero primo a seguir a num */
int prox_primo(int num);
int eh_primo(int num);

/* inicializa um vetor de equipas */
Vetor inicializaVetor(int M);
/* insere uma equipa no vetor */
void insereVetor(Vetor a, Equipa eq);
/* liiberta espaco do vetor */
void freeVetor(Vetor a);

/* cria a hash */
int hash_eq(Chave_Equipas v, int M);
/* inicializa a hashtable */
Hashtable_eq inicializaHT_eq(int max);
/* insere uma equipa na hashtable */
void insereHT_eq(Hashtable_eq ht, Equipa equipa);
/* expande a hashtable */
void expandeHT_eq(Hashtable_eq ht);
/* procura na hashtable por um nome de uma equipa */
Equipa procuraHT_eq(Hashtable_eq ht, Chave_Equipas v);
/* liberta todo o espaco da hashtable */
void freeHT_eq(Hashtable_eq ht);

#endif