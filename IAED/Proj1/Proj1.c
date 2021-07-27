/*
 * Autor:  Mara Gomes Alves, numero 95625
 * Projeto 1 de IAED 2019/2020
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

/* tamanho maximo das strings */
#define MAX_STR 63
/* numero maximo de produtos */
#define PRODS 10000
/* numero maximo de encomendas */
#define ENC 500
/* peso maximo de uma encomenda */
#define PESO_MAX 200
/* criterios para a funcao de ordenacao */
#define ALFABETO 0
#define PRECOS 1

#define TRUE 1
#define FALSE 0

/* identificador do produto mais recente */
int ip=0;
/* identificador da encomenda mais recente */
int ie=0;

/* produtos */
typedef struct produto{
    char desc[MAX_STR]; /* descricao */
    int preco;
    int peso;
    int qtd; /* quantidade */
}produto;

/* stock */
produto stock[PRODS] = {{"",0,0,0}};
/* encomendas */
produto encomenda[ENC][PRODS] = {{{"",0,0,0}}};


/* Prototipos das funcoes */
void ad_produto ();
void ad_stock ();
void nova_encomenda ();
void ad_encomenda ();
int peso (int ide);
void remove_stock ();
void remove_encomenda ();
void custo ();
void alt_preco ();
void desc_quant ();
void encomenda_max_produto ();
void lista_stock();
void lista_encomenda();
void mergesort(int lista[PRODS], int esquerda, int direita, int criterio);
void merge(int lista[PRODS], int esquerda, int m, int direita, int criterio);
int condicao (int criterio, int a, int b);
void imprime_lista (int lista[PRODS], int i_max, produto vetor[PRODS]);


/************************************** MAIN *************************************/
int main(){
    char letra;

    while (1){
        switch (letra=getchar()){
            case 'a':
                ad_produto ();
                break;
            case 'q':
                ad_stock ();
                break;
            case 'N':
                nova_encomenda();
                break;
            case 'A':
                ad_encomenda();
                break;
            case 'r':
                remove_stock ();
                break;
            case 'R':
                remove_encomenda ();
                break;
            case 'C':
                custo ();
                break;
            case 'p':
                alt_preco ();
                break;
            case 'E':
                desc_quant ();
                break;
            case 'm':
                encomenda_max_produto ();
                break;
            case 'l':
                lista_stock();
                break;
            case 'L':
                lista_encomenda();
                break;
            case 'x':
                return 0;
        }
    }
}

/* adiciona um novo produto ao sistema */
void ad_produto (){
    scanf(" %[^:]:%d:%d:%d", stock[ip].desc, &stock[ip].preco, &stock[ip].peso, &stock[ip].qtd);
    printf ("Novo produto %d.\n", ip);
    ip++;
}


/* adiciona a quantidade qtd de produto indice idp ao sistema */
void ad_stock (){
    int idp, qtd;
    scanf ("%d:%d", &idp,&qtd);

    if (idp >= ip)
        printf("Impossivel adicionar produto %d ao stock. Produto inexistente.\n", idp);
    else
        stock[idp].qtd += qtd;
}


/* cria nova encomenda */
void nova_encomenda (){
    printf ("Nova encomenda %d.\n", ie);
    ie++;
}


/* calcula peso da encomenda indice ide */
int peso (int ide){
    int idp, peso_total=0;

    for (idp=0; idp<ip; idp++)
        peso_total += stock[idp].peso * encomenda[ide][idp].qtd;
    
    return peso_total;
}


/* adiciona a quantidade qtd de produto idp a encomenda ide*/
void ad_encomenda (){
    int ide, idp, qtd;
    scanf("%d:%d:%d", &ide, &idp, &qtd);

    /* erros */
    if (ide >= ie)
        printf ("Impossivel adicionar produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
    else if (idp >= ip)
        printf ("Impossivel adicionar produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
    else if (stock[idp].qtd < qtd)
        printf ("Impossivel adicionar produto %d a encomenda %d. Quantidade em stock insuficiente.\n", idp, ide);
    else if (peso(ide)+qtd*stock[idp].peso > PESO_MAX)
        printf ("Impossivel adicionar produto %d a encomenda %d. Peso da encomenda excede o maximo de %d.\n", idp, ide, PESO_MAX);
        
    /* nao houve erros */
    else{
        encomenda[ide][idp].qtd += qtd;
        stock[idp].qtd -= qtd; /* retirar do stock */
    }
}


/* remove do sistema a quantidade qtd de produto idp */
void remove_stock (){
    int idp, qtd;
    scanf("%d:%d", &idp, &qtd);

    if (idp >= ip)
        printf ("Impossivel remover stock do produto %d. Produto inexistente.\n", idp);
    else if (stock[idp].qtd < qtd)
        printf ("Impossivel remover %d unidades do produto %d do stock. Quantidade insuficiente.\n", qtd, idp);
    else
        stock[idp].qtd -= qtd;
}


/* remove da encomenda ide o produto idp*/
void remove_encomenda (){ 
    int ide, idp;
    scanf("%d:%d", &ide, &idp);

    if (ide >= ie)
        printf ("Impossivel remover produto %d a encomenda %d. Encomenda inexistente.\n", idp, ide);
    else if (idp >= ip)
        printf ("Impossivel remover produto %d a encomenda %d. Produto inexistente.\n", idp, ide);
    else{
        stock[idp].qtd += encomenda[ide][idp].qtd; /* repoe no stock */
        encomenda[ide][idp].qtd = 0;
    }
}


/* calcula o custo da encomenda ide */
void custo (){
    int ide, idp, custo=0;
    scanf("%d", &ide);

    if (ide >= ie)
        printf("Impossivel calcular custo da encomenda %d. Encomenda inexistente.\n", ide);
    else{
        for (idp=0; idp<ip; idp++)
            custo += stock[idp].preco * encomenda[ide][idp].qtd;
    
        printf ("Custo da encomenda %d %d.\n", ide, custo);
    }
}


/* altera o custo do produto idp para novo_preco */
void alt_preco (){
    int idp, novo_preco;
    scanf("%d:%d", &idp, &novo_preco);

    if (idp >= ip)
        printf ("Impossivel alterar preco do produto %d. Produto inexistente.\n", idp);
    else
        stock[idp].preco = novo_preco;
}


/* imprime a descricao e quantidade de produto idp na encomenda ide */
void desc_quant (){
    int ide, idp;
    scanf("%d:%d", &ide, &idp);

    if (ide >= ie)
        printf ("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
    else if (idp >= ip)
        printf ("Impossivel listar produto %d. Produto inexistente.\n", idp);
    else
        printf ("%s %d.\n", stock[idp].desc, encomenda[ide][idp].qtd);
}


/* diz em qual encomenda o produto idp ocorre mais vezes */
void encomenda_max_produto (){
    int ide=0, idp, i_max=0, vazia=TRUE;
    scanf("%d", &idp);

    if (idp >= ip)
        printf ("Impossivel listar maximo do produto %d. Produto inexistente.\n",idp);
    else{
        while (ide<ie){
            if (encomenda[ide][idp].qtd > 0){
                if (encomenda[ide][idp].qtd > encomenda[i_max][idp].qtd)
                    i_max = ide;
                vazia = FALSE; /* produto presente em pelo menos 1 encomenda */
            }
            ide++;
        }
        /* se existe pelo menos 1 encomenda e em pelo menos uma delas existe o produto */
        if (ide > 0 && vazia == FALSE)
            printf ("Maximo produto %d %d %d.\n", idp, i_max, encomenda[i_max][idp].qtd);
    }
}


/* lista todos os produtos do sistema do mais barato ao mais caro */
void lista_stock(){
    int idp, lista[PRODS];

    printf ("Produtos\n");

    /* cria uma lista com os indices */
    for (idp=0; idp<ip; idp++)
        lista[idp] = idp;

    mergesort (lista, 0, idp-1, PRECOS);
    imprime_lista (lista, idp, stock);
}


/* lista todos os produtos na encomenda ide por ordem alfabetica */
void lista_encomenda(){
    int idp, ide, k=0, lista[PRODS];
    scanf("%d", &ide);

    if (ide >= ie)
        printf ("Impossivel listar encomenda %d. Encomenda inexistente.\n", ide);
    else{
        printf ("Encomenda %d\n", ide);
        /* cria lista com indices dos produtos que pertencem a encomenda */
        for (idp=0; idp<ip; idp++)
            if (encomenda[ide][idp].qtd > 0){
                lista[k] = idp;
                k++;
            }
        mergesort (lista, 0, k-1, ALFABETO);
        imprime_lista(lista, k, encomenda[ide]);
    }
}


/* merge sort */
void mergesort(int lista[PRODS], int esquerda, int direita, int criterio){
    int m = (direita+esquerda)/2;
    if (direita<=esquerda)
        return;
    mergesort(lista, esquerda, m, criterio);
    mergesort(lista, m+1, direita, criterio);
    merge(lista, esquerda, m, direita, criterio);
}


/* merge de duas listas */
void merge(int lista[PRODS], int esquerda, int m, int direita, int criterio){
    int i, j, k, aux[PRODS];
    for (i=m+1; i>esquerda; i--)
        aux[i-1] = lista[i-1];
    for (j=m; j<direita; j++)
        aux[direita+m-j] = lista[j+1];
    /* com i=esquerda; j=direita */
    for (k=esquerda; k<=direita; k++)
        if (condicao(criterio, aux[i], aux[j])==TRUE)
            lista[k] = aux[j--];
        else
            lista[k] = aux[i++];   
}


/* define a condicao adequada de acordo com o criterio */
int condicao (int criterio, int a, int b){
    /* para a funcao lista_stock
     * ordem CRESCENTE de precos e indices */
    if (criterio == PRECOS){
        if (stock[a].preco == stock[b].preco)
            return (a>b);
        return (stock[a].preco > stock[b].preco);
    }
    /* para a funcao lista_encomenda
     * alfabeticamente */
    else if (criterio == ALFABETO)
        return (strcmp(stock[a].desc, stock[b].desc) > 0);
    else
        return 0;
}


/* imprime uma lista */
void imprime_lista (int lista[PRODS], int i_max, produto vetor[PRODS]){
    int i; 
    for (i=0; i<i_max; i++)
            printf ("* %s %d %d\n", stock[lista[i]].desc, stock[lista[i]].preco, vetor[lista[i]].qtd);
}