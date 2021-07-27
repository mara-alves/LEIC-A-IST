/*
 * Ficheiro jogos.c
 * Aluna 95625, Mara Gomes Alves
 * Implementacao de uma lista simplesmente ligada e de uma
 * hashtable dinamica com procura linear, que guarda jogos de futebol
 */

#include "jogos.h" 

/* ------------------------------------------------------------------------- */

/* imprime jogo no formato:
 * "nome_jogo nome_equipa1 nome_equipa2 pontos_equipa1 pontos_equipa2 */
void imprime_jogo(Jogo jogo){
    if (jogo != NULL)
        printf("%s %s %s %d %d\n", jogo->nome, 
                                   jogo->equipa1->nome, jogo->equipa2->nome,
                                   jogo->score1, jogo->score2);
}

/* Funcoes para lista ------------------------------------------------------ */

/* cria uma nova lista vazia */
Lista criaLista(){
    Lista new = malloc(sizeof(struct lista));
    new -> primeiro = NULL;
    new -> ult = NULL;
    return new;
}

/* adiciona no final da lista l um novo nome c */
void insereLista(Lista l, char * c){
    Node new = malloc(sizeof(struct node));
    new -> c = c;
    new -> prox = NULL;

    /* caso a lista esteja vazia
     * novo elemento eh o primeiro e o ultimo */
    if (l->primeiro == NULL){
        l->primeiro = new;
        l->ult = new;
    }
    /* se nao, eh so o ultimo */
    else{
        l->ult->prox = new;
        l->ult = new;
    }
}

/* apaga a node de conteudo c na lista l */
void apagaLista(Lista l, char * c){
    Node atual = l->primeiro;
    Node aux;

    if (iguais(atual->c, c)){
        /* caso so haja um elemento na lista e for esse a remover */
        if (atual->prox == NULL){
            l->ult = NULL;
            l->primeiro = NULL;
            free(atual);
            return;
        }
        /* caso o primeiro elemento seja o a remover */
        else{
            l->primeiro = l->primeiro->prox;
            free(atual);
            return;
        }
    }
    /* se nenhuma das anteriores, procurar pela posicao */
    while (atual->prox != NULL){
        if (iguais(atual->prox->c, c)){
            aux = atual->prox; /* node a remover */
            atual->prox = atual->prox->prox; /* mudar ponteiro prox da node antes */

            /* caso seja o ultimo na lista
             * passar o ponteiro ultimo para o anterior */
            if (atual->prox == NULL)
                l->ult = atual;

            free(aux);
            return;
        }
        atual = atual->prox;
    }
}

/* libertar memoria da lista de nomes */
void freeLista(Lista l){
    Node aux = l -> primeiro;

    while(aux != NULL){
        l->primeiro = l->primeiro->prox;
        free(aux->c); /* liberta string */
        free(aux); /* liberta node */
        aux = l->primeiro;
    }
    free(l);
}


/* Funcoes para hashtable -------------------------------------------------- */

/* cria a hash atraves de uma string */
int hash_j(Chave_Jogos k, int M) { 
    int h = 0, a = 127;
    for (; *k != '\0'; k++)
        h = (a*h + *k) % M;
    return h;
}

/* inicializa a hashtable */
Hashtable_j inicializaHT_j(int max){
    Hashtable_j ht = malloc(sizeof(struct hashtable_j));
    Jogo * tab = malloc(max*sizeof(Jogo));
    int i;

    ht -> N = 0; /* numero de jogos na hashtable, comeca com zero */
    ht -> M = max; /* tamanho da hashtable */

    /* colocar todos os espacos da tabela a NULL */
    for (i = 0; i < ht->M; i++)
        tab[i] = NULL;
    ht -> tabela = tab;
    
    return ht;
}

/* insere um jogo na hashtable */
void insereHT_j(Hashtable_j ht, Jogo jogo) {
    Chave_Jogos k = jogo->nome; 
    int i = hash_j(k, ht->M);
    Jogo * tab = ht -> tabela;

    /* procura pela posicao: 
     * ou corresponde a hash, ou eh a primeira livre a seguir */
    while (tab[i] != NULL) 
        i = (i+1) % ht->M; 

    /* coloca na posicao */
    tab[i] = jogo;

    /* expande a hashtable quando chega a metade da capacidade */
    if (ht->N++ > ht->M/2) 
        expandeHT_j(ht);
} 

/* aumenta o tamanho da hashtable para o primo a seguir ao dobro do original */
void expandeHT_j(Hashtable_j ht) {
    int i;
    int tam_ant  = ht -> M; /* guarda o tamanho da hashtable antes de expandida */
    Jogo * aux = ht -> tabela; /* guarda a tabela antiga */
    int novo_tam = prox_primo(ht->M + ht->M); /* tamanho da nova tabela */
    Jogo * tab = malloc(novo_tam*sizeof(Jogo)); /* nova tabela */

    /* tabela nova toda a NULL */
    for (i = 0; i < novo_tam; i++)
        tab[i] = NULL;

    ht -> tabela = tab; /* substitui antiga pela nova tabela */
    ht -> M = novo_tam; /* atualiza tamanho da hashtable */
    ht -> N = 0; /* reinicia a contagem de jogos na tabela */

    /* insere os elementos na nova tabela */
    for (i = 0; i < tam_ant; i++)
        if (aux[i] != NULL)
            insereHT_j(ht, aux[i]);

    free(aux); /* liberta tabela antiga */
}

/* procura na hashtable por um jogo com chave v */
Jogo procuraHT_j(Hashtable_j ht, Chave_Jogos k) { 
    int i = hash_j(k, ht->M);
    Jogo * tab = ht->tabela;

    /* procura na posicao da hash e nas seguintes ate encontrar ou ser NULL */
    while (tab[i] != NULL){
        if (iguais(k, tab[i]->nome))
            return tab[i]; /* devolve o jogo encontrado */
        else 
            i = (i+1) % ht->M; /* passar a proxima posicao */
    }
    return NULL; /* caso nao esteja na hashtable */
}

/* apaga um jogo da hashtable */
void apagaHT_j(Hashtable_j ht, Chave_Jogos k) { 
    int j, i = hash_j(k, ht->M);
    Jogo * tab = ht -> tabela;
    Jogo aux;

    /* procura na posicao da hash e nas seguintes ate encontrar ou chegar a NULL */
    while (tab[i] != NULL) 
        if (iguais(k, tab[i]->nome))
            break; 
        else 
            i = (i+1) % ht->M;
    /* percorreu a tabela toda, jogo nao existe */
    if (tab[i] == NULL) 
        return;
    /* encontrou */
    ht->N--;
    free(tab[i]->nome); /* libertar a string do nome */
    free(tab[i]); /* libertar o jogo */
    tab[i] = NULL;

    /* voltar a inserir os jogos das posicoes a frente */
    for (j = (i+1) % ht->M; tab[j] != NULL; j = (j+1) % ht->M) {
        aux = tab[j];
        tab[j] = NULL;
        insereHT_j(ht, aux);
        ht->N--;
    }
}

/* liberta o espaco da hashtable */
void freeHT_j(Hashtable_j ht) { 
    int i = 0;
    Jogo * tab = ht -> tabela;

    while (i < ht->M){
        /* nao liberta a string com o nome do jogo, isso eh feito em freeLista */
        free(tab[i]); /* liberta a estrutura jogo */
        i++;
    }
    free(tab); /* liberta a tabela de ponteiros */
    free(ht); /* liberta a estrutura hashtable */
}