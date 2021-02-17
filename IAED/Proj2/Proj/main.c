/*
 * Ficheiro main.c
 * Aluna 95625, Mara Gomes Alves
 * Projeto 2 de IAED 2019/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jogos.h"

#define MAX_STR 1023 /* comprimento maximo dos nomes a ler */
#define HT_JOGOS 769 /* tamanho original da hashtable de jogos, deve ser um primo */
#define HT_EQUIPAS 389 /* tamanho inicial da hashtable de equipas, deve ser primo */


/* funcao para alocar apenas o espaco necessario para uma string
 * retorna ponteiro para a string */
char * copia_string (char * string);

/* Prototipos das funcoes de cada letra ------------------------------------ */
/* a */
void ad_novo_jogo(int linha, Hashtable_j ht_jogos, Lista nomes, 
                              Hashtable_eq ht_equipas);
/* l */
void imprime_jogos(int linha, Hashtable_j ht_jogos, Lista nomes);
/* p */
void procura_jogo(int linha, Hashtable_j ht_jogos);
/* r */
void apaga_jogo(int linha, Hashtable_j ht_jogos, Lista nomes);
/* s */
void altera_score(int linha, Hashtable_j ht_jogos);
/* A */
void ad_nova_equipa(int linha, Hashtable_eq ht_equipas, Vetor equipas);
/* P */
void procura_equipa(int linha, Hashtable_eq ht_equipas);
/* g */
void encontra_melhores(int linha, Hashtable_eq ht_equipas, Vetor equipas);

/* funcao de comparacao para o q sort
 * ordena por vitorias e por ordem lexicografica */
int comparador(const void * a, const void * b);


/* MAIN -------------------------------------------------------------------- */

int main(){
    char letra;
    int linha = 1; /* conta o numero de linhas do input */
    Lista nomes = criaLista(); /* guarda nomes dos jogos por ordem de introducao */
    Vetor equipas = inicializaVetor(HT_EQUIPAS); /* guarda as equipas num array */
    Hashtable_j ht_jogos = inicializaHT_j(HT_JOGOS);
    Hashtable_eq ht_equipas = inicializaHT_eq(HT_EQUIPAS);

    while(1){
        switch(letra=getchar()){
            case 'a':
                ad_novo_jogo(linha, ht_jogos, nomes, ht_equipas);
                linha++;
                break;
            case 'l':
                imprime_jogos(linha, ht_jogos, nomes);
                linha++;
                break;
            case 'p':
                procura_jogo(linha, ht_jogos);
                linha++;
                break;
            case 'r':
                apaga_jogo(linha, ht_jogos, nomes);
                linha++;
                break;
            case 's':
                altera_score(linha, ht_jogos);
                linha++;
                break;
            case 'A':
                ad_nova_equipa(linha, ht_equipas, equipas);
                linha++;
                break;
            case 'P':
                procura_equipa(linha, ht_equipas);
                linha++;
                break;
            case 'g':
                encontra_melhores(linha, ht_equipas, equipas);
                linha++;
                break;
            case 'x':
                freeLista(nomes);
                freeVetor(equipas);
                freeHT_eq(ht_equipas);
                freeHT_j(ht_jogos);
                return 0;
        }
    }
}
/* --------------------------------------------------------------------------------------- */

/* strdup
 * funcao para alocar apenas o espaco necessario para uma string
 * retorna ponteiro para a string */
char * copia_string (char * string){
    char * nova = malloc(sizeof(char)*strlen(string)+1); /* +1 para o \0 */
    strcpy(nova, string);
    return nova;
}

/* adiciona um novo jogo ao sistema */
void ad_novo_jogo (int linha, Hashtable_j ht_jogos, Lista nomes, 
                              Hashtable_eq ht_equipas){
                                         
    char *n, nome[MAX_STR], equipa1[MAX_STR], equipa2[MAX_STR];
    int score1, score2;
    Equipa eq1, eq2;

    /* le input */
    scanf(" %[^:]:%[^:]:%[^:]:%d:%d", nome, equipa1, equipa2, &score1, &score2);
    
    /* procura na hashtable por um jogo com esse nome
     * caso ja exista da erro */
    if (procuraHT_j(ht_jogos, nome))
        printf("%d Jogo existente.\n", linha);

    /* erro se pelo menos uma equipa nao existir */
    else if (!(eq1=procuraHT_eq(ht_equipas, equipa1)) || !(eq2=procuraHT_eq(ht_equipas, equipa2)))
        printf("%d Equipa inexistente.\n", linha);

    /* nao ha erros */
    else {
        Jogo jogo = malloc(sizeof(struct jogo));

        /* nome do jogo */
        n = copia_string(nome);
        jogo -> nome = n;
        insereLista(nomes, n); /* adicionar nome a lista */

        /* equipas */
        jogo -> equipa1 = eq1;
        jogo -> equipa2 = eq2;

        /* scores */
        jogo -> score1 = score1;
        jogo -> score2 = score2;

        /* adiciona vitoria a equipa com mais golos, se existir */
        if (score1 > score2)
            vitoria(eq1);
        else if (score2 > score1)
            vitoria(eq2);

        /* adicionar jogo a hashtable */
        insereHT_j(ht_jogos, jogo);
    }
}

/* imprime todos os jogos, por ordem de introducao */
void imprime_jogos (int linha, Hashtable_j ht_jogos, Lista nomes){
    Node current = nomes -> primeiro;

    /*  percorre a lista de nomes
     * imprime jogo da hashtable com nome correspondente */
    while (current != NULL){
        printf("%d ", linha);
        imprime_jogo(procuraHT_j(ht_jogos, current->c));
        current = current -> prox;
    }
}

/* procura um jogo no sistema pelo nome */
void procura_jogo (int linha, Hashtable_j ht_jogos){
    char nome[MAX_STR];
    Jogo jogo;

    /* le input: nome do jogo a procurar*/
    scanf(" %[^\n]s", nome);

    /* se encontrou na hashtable um jogo com esse nome, imprime */
    if ((jogo=procuraHT_j(ht_jogos, nome))){
        printf("%d ", linha);
        imprime_jogo(jogo);
    }
    /* jogo nao existe */
    else
        printf("%d Jogo inexistente.\n", linha);
}

/* apaga um jogo do sistema */
void apaga_jogo(int linha, Hashtable_j ht_jogos, Lista nomes){
    char nome[MAX_STR];
    Jogo jogo;

    /* le input: nome do jogo */
    scanf(" %[^\n]s", nome);

    /* caso nao encontre na hashtable um jogo com esse nome */
    if(!(jogo=procuraHT_j(ht_jogos, nome)))
        printf("%d Jogo inexistente.\n", linha);
    
    /* se encontrar na hashtable */
    else{
        /* alterar as vitorias das equipas nesse jogo, se necessario */
        if (jogo->score1 > jogo->score2)
            perde_vitoria(jogo->equipa1);
        else if (jogo->score2 > jogo->score1)
            perde_vitoria(jogo->equipa2);

        /* apagar da lista de nomes */
        apagaLista(nomes, nome);
        /* apagar da hashtable */
        apagaHT_j(ht_jogos, nome);
    }
}

/* muda as pontuacoes de um jogo no sistema pelas pontuacoes introduzidas */
void altera_score(int linha, Hashtable_j ht_jogos){
    char nome[MAX_STR];
    int antigo_score1, antigo_score2, score1, score2;
    Jogo jogo;
    Equipa eq1, eq2;

    /* le input: nome do jogo, pontuacoes da equipa 1 e da equipa 2 respetivamente */
    scanf(" %[^:]:%d:%d", nome, &score1, &score2);

    /* caso o jogo nao exista */
    if (!(jogo=procuraHT_j(ht_jogos, nome)))
        printf("%d Jogo inexistente.\n", linha);

    else {
        /* obtem equipas */
        eq1 = jogo -> equipa1;
        eq2 = jogo -> equipa2;
        /* obtem pontuacoes originais */
        antigo_score1 = jogo -> score1;
        antigo_score2 = jogo -> score2;
        /* atualiza as pontuacoes para as introduzidas */
        jogo -> score1 = score1;
        jogo -> score2 = score2;

        /* alterar numeros de vitorias se necessario */
        if (antigo_score1 > antigo_score2 && score1 <= score2){
            if (score1 < score2)
                vitoria(eq2);
            perde_vitoria(eq1);
        }
        else if (antigo_score1 < antigo_score2 && score1 >= score2){
            if (score1 > score2)
                vitoria(eq1);
            perde_vitoria(eq2);
        }
        else if (antigo_score1 == antigo_score2){
            if (score1 > score2)
                vitoria(eq1);
            else if (score1 < score2)
                vitoria(eq2);
        }
    }
}

/* adiciona uma nova equipa ao sistema, com o nome lido */
void ad_nova_equipa(int linha, Hashtable_eq ht_equipas, Vetor equipas){
    char nome[MAX_STR], *eq;

    /* le input: nome da equipa a adicionar */
    scanf(" %[^\n]s", nome);

    /* erro caso a equipa ja exista */
    if (procuraHT_eq(ht_equipas, nome))
        printf("%d Equipa existente.\n", linha);

    else {
        /* criar nova estrutura equipa com o nome introduzido */
        Equipa nova = malloc(sizeof(struct equipa));
        eq = copia_string(nome);
        nova -> nome = eq;
        nova -> vitorias = 0;
        /* introduzir na hashtable */
        insereHT_eq(ht_equipas, nova);
        /* introduzir no vetor */
        insereVetor(equipas, nova);
    }
}

/* procura uma equipa no sistema */
void procura_equipa (int linha, Hashtable_eq ht_equipas){
    char nome[MAX_STR];
    Equipa equipa;

    /* le input: nome da equipa a procurar */
    scanf(" %[^\n]s", nome);

    /* caso a equipa exista no sistema, imprimir */
    if ((equipa=procuraHT_eq(ht_equipas, nome))){
        printf("%d ", linha);
        imprime_equipa(equipa);
    }
    /* a equipa nao existe */
    else
        printf("%d Equipa inexistente.\n", linha);
}

/* procura equipas com maior numero de vitorias
 * lista-as por ordem lexicografica */
void encontra_melhores(int linha, Hashtable_eq ht_equipas, Vetor equipas){
    int N = ht_equipas -> N;

    /* so faz algo se existirem equipas */
    if (N>0){
        int max_vitorias, i=0;

        /* ordena todas as equipas por vitorias e nome */
        qsort(equipas->vetor, equipas->N, sizeof(Equipa), comparador);

        /* obtem o maior numero de vitorias
         * na lista ordenada sera as vitorias da primeira equipa */
        max_vitorias = (equipas->vetor[0])->vitorias;
    
        printf("%d Melhores %d\n", linha, max_vitorias);

        while(i<N && equipas->vetor[i]->vitorias == max_vitorias){
           printf("%d * %s\n", linha, equipas->vetor[i]->nome);
           i++;
        }
    }
}

/* funcao para o qsort, ordena por numero de vitorias e lexicograficamente */
int comparador(const void * a, const void * b){
    const Equipa * eq1 = (Equipa*)a;
    const Equipa * eq2 = (Equipa*)b;
    int diferenca = (*eq2)->vitorias - (*eq1)->vitorias;
    
    /* o primeiro criterio eh o numero de vitorias */
    if (diferenca != 0)
        return diferenca;
    /* em caso de mesmo numero de vitorias, ordena lexicograficamente */
    else
        return strcmp((*eq1)->nome,(*eq2)->nome);
}