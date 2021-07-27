/*
 * Ficheiro equipas.c
 * Aluna 95625, Mara Gomes Alves
 * Implementacao de um vetor que aumenta de tamanho consoante seja necessario
 * e de uma hashtable dinamica com procura linear, que guarda equipas
 */

#include "equipas.h" 

/* ------------------------------------------------------------------------- */

/* imprime a equipa eq no formato "nome n_vitorias" */
void imprime_equipa(Equipa eq){
    printf("%s %d\n", eq->nome, eq->vitorias);
}

/* ------------------------------------------------------------------------- */

/* encontra o numero primo mais proximo a seguir a num */
int prox_primo(int num){
    int primo = num; 
    int encontrou = FALSE; 

    if (num <= 1) 
        return 2; 

    /* percorre numeros a seguir a num
     * ate encontrar um primo */
    while (!encontrou){ 
        primo++; 
  
        if (eh_primo(primo)) 
            encontrou = TRUE; 
    } 
    return primo; 
}

/* devolve 1 se num eh primo e 0 se nao eh */
int eh_primo(int num){
    int i;

    if (num <= 1)
        return FALSE;  
    else if (num <= 3)
        return TRUE;  

    if (num % 2 == 0 || num % 3 == 0) 
        return FALSE;  
    
    for (i = 5; i*i <= num; i = i + 6)  
        if (num % i == 0 || num % (i+2) == 0)  
           return FALSE;  
    
    return TRUE;  
}


/* Vetor de Equipas -------------------------------------------------------- */

/* inicializa o vetor */
Vetor inicializaVetor(int M) {
    Vetor a = malloc(sizeof(struct vetor));
    a->vetor = malloc(M * sizeof(struct equipa));
    a->N = 0; /* numero de equipas no vetor */
    a->M = M; /* tamanho do vetor */
    return a;
}

/* insere uma equipa eq no vetor a */
void insereVetor(Vetor a, Equipa eq) {
    /* caso o vetor esteja cheio, expandir vetor */
        if (a->N == a->M){
        int novo_tam = a->M + a->M; /* tamanho novo */
        a->M = novo_tam; /* atualizar o tamanho */
        a->vetor = realloc(a->vetor, novo_tam * sizeof(struct equipa));
    }
    /* adiciona equipa */
    a->vetor[a->N++] = eq;
}

/* liberta o espaco associado a um vetor
 * nao liberta o espaco de cada equipa pois isso eh feito na freeHT_eq */
void freeVetor(Vetor a) {
    free(a->vetor);
    a->vetor = NULL;
    free(a);
}


/* Funcoes para hashtable -------------------------------------------------- */

/* cria a hash atraves de uma string */
int hash_eq(Chave_Equipas k, int M) { 
    int h = 0, a = 127;
    for (; *k != '\0'; k++)
        h = (a*h + *k) % M;
    return h;
}

/* inicializa a hashtable */
Hashtable_eq inicializaHT_eq(int max){
    Hashtable_eq ht = malloc(sizeof(struct hashtable_eq));
    Equipa * tab = malloc(max*sizeof(Equipa));
    int i;

    ht -> N = 0; /* numero de equipas na hashtable, comeca com zero */
    ht -> M = max; /* tamanho da hashtable */

    /* colocar todos os espacos da tabela a NULL */
    for (i = 0; i < ht->M; i++)
        tab[i] = NULL;
    ht -> tabela = tab;

    return ht;
}

/* insere uma equipa na hashtable */
void insereHT_eq(Hashtable_eq ht, Equipa equipa) {
    Chave_Equipas k = equipa -> nome; 
    int i = hash_eq(k, ht->M);
    Equipa * tab = ht -> tabela;

    /* procura pela posicao: 
     * ou corresponde a hash, ou eh a primeira livre a seguir */
    while (tab[i] != NULL) 
        i = (i+1) % ht->M;

    /* coloca na posicao */
    tab[i] = equipa;

    /* expande a hashtable quando chega a metade da capacidade */
    if (ht->N++ > ht->M/2) 
        expandeHT_eq(ht);
} 

/* duplica o tamanho da hashtable */
void expandeHT_eq(Hashtable_eq ht) {
    int i;
    int tam_ant  = ht->M; /* guarda o tamanho da hashtable antes de expandida */
    Equipa * aux = ht -> tabela; /* guarda a tabela antiga */
    int novo_tam = prox_primo(ht->M + ht->M); /* tamanho da nova tabela */
    Equipa * tab = malloc(novo_tam*sizeof(Equipa)); /* nova tabela */

    /* tabela nova toda a NULL */
    for (i = 0; i < novo_tam; i++)
        tab[i] = NULL;

    ht -> tabela = tab; /* substitui antiga pela nova tabela */
    ht -> M = novo_tam; /* atualiza o tamanho */
    ht -> N = 0; /* reinicia a contagem de equipas na tabela */

    /* insere os elementos na nova tabela */
    for (i = 0; i < tam_ant; i++)
        if (aux[i] != NULL)
            insereHT_eq(ht, aux[i]);

    free(aux); /* liberta tabela antiga */
}

/* procura na hashtable por um jogo com chave k */
Equipa procuraHT_eq(Hashtable_eq ht, Chave_Equipas k) { 
    int i = hash_eq(k, ht->M);
    Equipa * tab = ht->tabela;

    /* procura na posicao da hash e nas seguintes ate encontrar ou ser NULL */
    while (tab[i] != NULL){
        if (iguais(k, tab[i]->nome))
            return tab[i]; /* devolve o jogo encontrado */
        else 
            i = (i+1) % ht->M;
    }
    return NULL; /* caso nao esteja na hashtable */
}

/* liberta todo o espaco da hashtable */
void freeHT_eq(Hashtable_eq ht) { 
    int i = 0;
    Equipa * tab = ht -> tabela;

    while (i < ht->M){
        if (tab[i]){
            free(tab[i]->nome); /* liberta nome da equipa */
            free(tab[i]); /* liberta a estrutura equipa */
        }
        i++;
    }
    free(tab); /* liberta a tabela */
    free(ht); /* liberta a hashtable */
}