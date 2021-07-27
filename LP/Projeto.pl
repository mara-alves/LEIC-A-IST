/* Aluna 95625 Mara Gomes Alves */

:- [codigo_comum].

%------------------------------------------------------------------------
% pertence(El, Lista), True se ha um elemento igual a El em Lista
%------------------------------------------------------------------------
pertence(P, [Q | _ ]) :- P == Q.
pertence(P, [_ | R]) :- pertence(P, R).

%------------------------------------------------------------------------
% substitui(Lista, Indice, Elemento, Nova) significa que Nova eh o
% resultado de substituir o conteudo de Lista no Indice por Elemento 
%------------------------------------------------------------------------
substitui([_|R], 1, Elemento, [Elemento|R]) :- !.

substitui([P|R1], Indice, Elemento, [P|R2]) :-
    Indice1 is Indice - 1,
    substitui(R1, Indice1, Elemento, R2).

%------------------------------------------------------------------------
% unificavel(Esp, Pal) retorna True se Esp eh unificavel com Pal, sem 
% modificar Esp
%------------------------------------------------------------------------
copia(Esp, Esp_C) :-
    maplist(copia_el, Esp, Esp_C).

copia_el(El, _) :-
    var(El), !.

copia_el(El, El).

unificavel(Esp, Pal) :-
    copia(Esp, Esp_C),
    Esp_C = Pal.

%------------------------------------------------------------------------
% Comp eh o numero de palavras possiveis para o Espaco
%------------------------------------------------------------------------
numero_palavras(Espaco, Comp-Espaco) :-
    nth1(2, Espaco, Palavras),
    length(Palavras, Comp).


/* 3.1.1 ----------------------------------------------------------------
                        obtem_letras_palavras/2
-----------------------------------------------------------------------*/
obtem_letras_palavras(Lst_Pals, Letras) :-
    sort(Lst_Pals, Pals_Ordenadas),
    maplist(atom_chars, Pals_Ordenadas, Letras).


/* 3.1.2 ----------------------------------------------------------------
                            espaco_fila/2
-----------------------------------------------------------------------*/
eh_prefixo([]).
eh_prefixo(Pref) :-
    last(Pref, Ultimo),
    Ultimo == '#'.

eh_sufixo([]).
eh_sufixo(Suf) :-
    nth0(0, Suf, Primeiro),
    Primeiro == '#'.

espaco_fila(Fila, Esp) :-
    append([Pref, Esp, Suf], Fila),
    \+ pertence('#', Esp),
    length(Esp, Comp),
    Comp >= 3,
    eh_prefixo(Pref),
    eh_sufixo(Suf).


/* 3.1.3 ----------------------------------------------------------------
                            espacos_fila/2
-----------------------------------------------------------------------*/
espacos_fila(Fila, Espacos) :-
    bagof(Esp, espaco_fila(Fila, Esp), Espacos), !.

espacos_fila(_, []).


/* 3.1.4 ----------------------------------------------------------------
                            espacos_puzzle/2
-----------------------------------------------------------------------*/
espacos_puzzle(Grelha, Espacos) :-
    espacos_parte_puzzle(Grelha, Espacos1),
    mat_transposta(Grelha, Transposta),
    espacos_parte_puzzle(Transposta, Espacos2),
    append([Espacos1, Espacos2], Espacos).

espacos_parte_puzzle([], []).

espacos_parte_puzzle([Fila|R], Espacos) :-
    espacos_fila(Fila, Espacos1),
    espacos_parte_puzzle(R, Espacos2),
    append([Espacos1, Espacos2], Espacos), !.


/* 3.1.5 ----------------------------------------------------------------
                    espacos_com_posicoes_comuns/3
-----------------------------------------------------------------------*/

% true se as duas listas tem pelo menos 1 variavel igual ----------------
em_comum([P|_], [P]).
em_comum([P|_], Q) :- pertence(P, Q), var(P).
em_comum([_|R], Q) :- em_comum(R, Q).
%------------------------------------------------------------------------

espacos_com_posicoes_comuns(Espacos, Esp, Esps_com) :-
    include(em_comum(Esp), Espacos, Esps_com_Esp),
    exclude(==(Esp), Esps_com_Esp, Esps_com). % remove o proprio espaco


/* 3.1.6 ----------------------------------------------------------------
                        palavra_possivel_esp/4
-----------------------------------------------------------------------*/

%------------------------------------------------------------------------
% letra_na_posicao(Pos, Letra, Espaco, Novo) significa que Novo eh o
% Espaco com a posicao Pos substituida pela Letra

letra_na_posicao(Pos, Letra, [P|R1], [P|R2]) :-
    Pos \== P, !,
    letra_na_posicao(Pos, Letra, R1, R2).

letra_na_posicao(Pos, Letra, [Pos|R], [Letra|R]).

% caso Pos nao esteja no Espaco nao ha substituicao a fazer
letra_na_posicao(_, _, Espaco, Espaco). 

%------------------------------------------------------------------------
% poe_letras(Espaco, Palavra, Lista, Nova) significa que Nova eh a Lista 
% em que as Posicoes de Espaco foram substituidas pelas Letras de Palavra

poe_letras([], [], Lista, Lista).

poe_letras([Pos|R1], [Letra|R2], Lista, Nova) :-
    maplist(letra_na_posicao(Pos, Letra), Lista, Lista1),
    poe_letras(R1, R2, Lista1, Nova), !.

%------------------------------------------------------------------------

palavra_possivel_esp(Pal, Esp, Espacos, Letras) :-
    member(Pal, Letras),
    pertence(Esp, Espacos),
    unificavel(Esp, Pal),
    espacos_com_posicoes_comuns(Espacos, Esp, Esps_com),
    poe_letras(Esp, Pal, Esps_com, Esps_com_Letras),
    forall(member(E, Esps_com_Letras), (member(L, Letras), unificavel(L,E))).


/* 3.1.7 ----------------------------------------------------------------
                         palavras_possiveis_esp/4
-----------------------------------------------------------------------*/
palavras_possiveis_esp(Letras, Espacos, Esp, Pals_Possiveis) :-
    bagof(Pal, palavra_possivel_esp(Pal, Esp, Espacos, Letras), Pals_Possiveis), !.

palavras_possiveis_esp(_, _, _, []).


/* 3.1.8 ----------------------------------------------------------------
                        palavras_possiveis/3
-----------------------------------------------------------------------*/
palavras_possiveis(Letras, Espacos, Pals_Possiveis) :-
    bagof([Esp, Pals], (member(Esp, Espacos), palavras_possiveis_esp(Letras, Espacos, Esp, Pals)), Pals_Possiveis).


/* 3.1.9 ----------------------------------------------------------------
                             letras_comuns/2
-----------------------------------------------------------------------*/
letras_comuns(Lst_Pals, Letras_comuns) :-
    letras_comuns(Lst_Pals, Letras_comuns, 1).

% fim da palavra
letras_comuns(Lst_Pals, [], Pos) :-
    nth1(1, Lst_Pals, Palavra),
    length(Palavra, Comp),
    Pos1 is Pos - 1,
    Comp == Pos1, !.

% letra sempre na mesma posicao
letras_comuns(Lst_Pals, Letras_comuns, Pos) :-
    % descobrir qual a letra a comparar
    nth1(1, Lst_Pals, Palavra),
    nth1(Pos, Palavra, Letra),
    % comparar com todas as palavras
    forall(member(Pal, Lst_Pals), (nth1(Pos, Pal, Letra))), !,
    % adicionar a lista
    append([(Pos, Letra)], Letras_comuns_1, Letras_comuns),
    % passar a proxima posicao
    Pos1 is Pos + 1,
    letras_comuns(Lst_Pals, Letras_comuns_1, Pos1).

% letra nao comum, passar a proxima posicao
letras_comuns(Lst_Pals, Letras_comuns, Pos) :-
    Pos1 is Pos + 1,
    letras_comuns(Lst_Pals, Letras_comuns, Pos1).


/* 3.1.10 ---------------------------------------------------------------
                            atribui_comuns/1
-----------------------------------------------------------------------*/
atribui_comuns(Pals_Possiveis) :-
    maplist(substitui_comuns, Pals_Possiveis).

substitui_comuns([Esp, Pals]) :-
    letras_comuns(Pals, Letras_comuns),
    maplist(substitui_comuns_aux(Esp), Letras_comuns).

% substitui em Esp a posicao Pos pela Letra
substitui_comuns_aux(Esp, (Pos, Letra)) :-
    substitui(Esp, Pos, Letra, Esp).


/* 3.1.11 ---------------------------------------------------------------
                        retira_impossiveis/2
-----------------------------------------------------------------------*/
retira_impossiveis([], []).

retira_impossiveis([[Espaco, Palavras] | R1], [[Espaco, Novas_Palavras] | R2]) :-
    include(unificavel(Espaco), Palavras, Novas_Palavras),
    retira_impossiveis(R1,R2).


/* 3.1.12 ---------------------------------------------------------------
                            obtem_unicas/2
-----------------------------------------------------------------------*/
obtem_unicas(Pals_Possiveis, Unicas) :-
    findall(Palavras, (member(Esp_Pals, Pals_Possiveis), nth1(2, Esp_Pals, [Palavras]), length([Palavras], Comp), Comp == 1), Unicas).

/* 3.1.13 ---------------------------------------------------------------
                            retira_unicas/2
-----------------------------------------------------------------------*/
retira_unicas(Pals_Possiveis, Novas_Pals_Possiveis) :-
    obtem_unicas(Pals_Possiveis, Unicas),
    retira_unicas(Pals_Possiveis, Novas_Pals_Possiveis, Unicas).

retira_unicas([], [], _).

retira_unicas([[Espaco, Palavras] | R1], [[Espaco, Novas_Palavras] | R2], Unicas) :-
    exclude(removivel(Palavras, Unicas), Palavras, Novas_Palavras),
    retira_unicas(R1,R2, Unicas).

removivel(Palavras, Unicas, Pal) :-
    Palavras \== [Pal],
    pertence(Pal, Unicas).


/* 3.1.14 ---------------------------------------------------------------
                            simplifica/2
-----------------------------------------------------------------------*/

% false se todos os espacos teem palavras unicas diferentes
ha_iguais(Pals_Possiveis) :-
    member([Esp1, Pals1], Pals_Possiveis),
    length(Pals1, Comp),
    Comp == 1,
    member([Esp2, Pals2], Pals_Possiveis),
    Esp1 \== Esp2,
    Pals1 == Pals2, !.

simplifica(Pals_Possiveis, Novas_Pals_Possiveis) :-
    atribui_comuns(Pals_Possiveis),
    retira_impossiveis(Pals_Possiveis, Pals_Possiveis_2),
    retira_unicas(Pals_Possiveis_2, Pals_Possiveis_3),
    Pals_Possiveis \== Pals_Possiveis_3, !,
    % se algum espaco ficou sem palavras, falha
    forall(member(Esp_Pals, Pals_Possiveis_3), (numero_palavras(Esp_Pals, Comp-_), Comp > 0)),
    simplifica(Pals_Possiveis_3, Novas_Pals_Possiveis).

simplifica(Novas_Pals_Possiveis, Novas_Pals_Possiveis) :-
% passo anterior nao deteta se houve alteracao ao atribuir comuns
    \+ ha_iguais(Novas_Pals_Possiveis),
    atribui_comuns(Novas_Pals_Possiveis).


/* 3.1.15 ---------------------------------------------------------------
                            inicializa/2
-----------------------------------------------------------------------*/
inicializa(Puz, Pals_Possiveis) :-
    nth1(1, Puz, Lst_Pals),
    obtem_letras_palavras(Lst_Pals, Letras),
    nth1(2, Puz, Grelha),
    espacos_puzzle(Grelha, Espacos),
    palavras_possiveis(Letras, Espacos, Pals_Possiveis_1),
    simplifica(Pals_Possiveis_1, Pals_Possiveis).


/* 3.2.1 ---------------------------------------------------------------
                    escolhe_menos_alternativas/2
-----------------------------------------------------------------------*/
escolhe_menos_alternativas(Pals_Possiveis, Escolha) :-
    maplist(numero_palavras, Pals_Possiveis, Pals_Comp),
    keysort(Pals_Comp, Pals_Comp_Ord),
    member(L-Escolha, Pals_Comp_Ord),
    L > 1, !.


/* 3.2.2 ---------------------------------------------------------------
                            experimenta_pal/3
-----------------------------------------------------------------------*/
experimenta_pal(Escolha, Pals_Possiveis, Novas_Pals_Possiveis) :-
    nth1(1, Escolha, Esp),
    nth1(2, Escolha, Lst_Pals),
    nth1(Indice, Pals_Possiveis, [Esp, Lst_Pals]),
    member(Pal, Lst_Pals),
    Esp = Pal,
    substitui(Pals_Possiveis, Indice, [Esp, [Pal]], Novas_Pals_Possiveis).


/* 3.2.3 ---------------------------------------------------------------
                            resolve_aux/2
-----------------------------------------------------------------------*/
resolve_aux(Novas_Pals_Possiveis, Novas_Pals_Possiveis) :-
    forall(member(Espaco, Novas_Pals_Possiveis), (numero_palavras(Espaco, Comp-Espaco), Comp == 1)), !.

resolve_aux(Pals_Possiveis, Novas_Pals_Possiveis) :-
    escolhe_menos_alternativas(Pals_Possiveis, Escolha),
    experimenta_pal(Escolha, Pals_Possiveis, Pals_Possiveis_Exp),
    simplifica(Pals_Possiveis_Exp, Pals_Possiveis_1),
    resolve_aux(Pals_Possiveis_1, Novas_Pals_Possiveis).


/* 3.3.1 ---------------------------------------------------------------
                            resolve/1
-----------------------------------------------------------------------*/
resolve(Puz) :-
    inicializa(Puz, Pals_Possiveis),
    resolve_aux(Pals_Possiveis, _).