/*
 * Ficheiro jogos.h
 * Aluna 95625, Mara Gomes Alves
 * Ficheiro para os jogos de futebol, com uma lista para os nomes
 * e uma hashtable
 */

#ifndef JOGOS_H

#include "equipas.h" /* necessario para a estrutura equipa
                      * e para as funcoes de numeros primos */


/* Estrutura Jogo ---------------------------------------------------------- */

typedef struct jogo{
    char *nome;
    Equipa equipa1;
    Equipa equipa2;
    int score1;
    int score2;
} * Jogo;


/* Lista para guardar os nomes dos jogos por ordem de insercao ------------- */

/* Node de uma Lista */
typedef struct node {
    struct node * prox; /* ponteiro para a proxima node */
    char * c; /* conteudo na node, string com nome de um jogo */
} * Node;

/* Lista */
typedef struct lista {
    struct node * primeiro, * ult; /* ponteiros para primeira 
                                    * e ultima nodes da lista */
} * Lista;


/* Hashtable --------------------------------------------------------------- */

typedef struct hashtable_j{
    Jogo * tabela;
    int N; /* numero de elementos */
    int M; /* tamanho da hashtable */
} * Hashtable_j;


/* chave eh o nome do jogo */
typedef char* Chave_Jogos; 
#define iguais(a,b) (strcmp(a,b)==0)


/* Prototipos -------------------------------------------------------------- */

/* imprime um jogo */
void imprime_jogo(Jogo jogo);

/* cria nova lista vazia */
Lista criaLista(); 
/* adiciona c como o ultimo elemento da lista */
void insereLista(Lista l, char * c); 
/* apaga uma node de uma lista */
void apagaLista(Lista l, char * c);
/* liberta toda a memoria associada a lista */
void freeLista(Lista l); 

/* cria a hash */
int hash_j(Chave_Jogos v, int M);
/* inicializa a hashtable */
Hashtable_j inicializaHT_j(int max);
/* insere um jogo na hashtable */
void insereHT_j(Hashtable_j ht, Jogo jogo);
/* expande a hashtable */
void expandeHT_j(Hashtable_j ht);
/* procura na hashtable por um nome de um jogo */
Jogo procuraHT_j(Hashtable_j ht, Chave_Jogos v);
/* apaga um jogo da hashtable */
void apagaHT_j(Hashtable_j ht, Chave_Jogos v);
/* liberta o espaco da hashtable */
void freeHT_j(Hashtable_j ht);

#endif