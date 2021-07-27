
:- use_module(library(clpfd)).
%-------------------------------------------------------------------------------
% mat_transposta(Matriz, Transp) significa que Transp e' a transposta de Matriz
%-------------------------------------------------------------------------------
mat_transposta(Matriz, Transp) :-
    transpose(Matriz, Transp).

%-------------------------------------------------------------------------------
%                escreve_Grelha(Grelha)
%-------------------------------------------------------------------------------
escreve_Grelha(Grelha) :-
    maplist(escreve_Linha, Grelha).

escreve_Linha([]) :- nl, !.

escreve_Linha([P | R]) :-
    (var(P) -> write('- ')
            ;
     write(P), write(' ')),
     escreve_Linha(R).




