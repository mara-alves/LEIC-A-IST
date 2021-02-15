TERM_WRITE      EQU     FFFEh
TERM_CURSOR     EQU     FFFCh
INT_MASK        EQU     FFFAh
TIMER_CONTROL   EQU     FFF7h
TIMER_VALUE     EQU     FFF6h
ACELEROMETRO    EQU     FFEBh

STACK_VALUE     EQU     8000h
ASTERISCOS      EQU     '*'
ESPACOS         EQU     ' '
BOLA            EQU     'o'
tempo           EQU     5 


                ORIG    0
                
flag_timer      WORD    0
xi              WORD    1
vi              WORD    0
x_atual         TAB     1
v_atual         TAB     1
x_final         TAB     1
v_final         TAB     1
aceleracao_x    TAB     1
posicao         TAB     1


                ORIG    7FF0h ; rotina de tratamento da interrupção
                
TIMER_INT:      DEC     R6
                STOR    M[R6], R7
                
                JAL     INT_TIMER
                
                LOAD    R7, M[R6]
                INC     R6
                
                RTI



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;                                MAIN                                ;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


                ORIG    0
                
MAIN:           MVI     R6, STACK_VALUE

                MVI     R1, INT_MASK
                MVI     R2, 8000h
                STOR    M[R1], R2
                
                ; colocar valores iniciais nas variaveis atuais
                MVI     R1, xi
                MVI     R2, x_atual
                LOAD    R1, M[R1]
                STOR    M[R2], R1
                MVI     R1, vi
                MVI     R2, v_atual
                LOAD    R1, M[R1]
                STOR    M[R2], R1
                
                ; montar paredes no terminal
                JAL     PAREDES
                JAL     INIC_FIM
                JAL     ESPACO
                JAL     INIC_FIM
                JAL     PAREDES
                
                ENI
                
                ; por o temporizador a contar
                MVI     R1, TIMER_VALUE
                MVI     R2, 5
                STOR    M[R1], R2
                MVI     R1, TIMER_CONTROL
                MVI     R2, 1
                STOR    M[R1], R2
                
.CICLO:         MVI     R1, flag_timer
                LOAD    R1, M[R1]
                MVI     R2, 0
                CMP     R1, R2
                
                JAL.NZ  TIMER_BOOM
                
                BR      .CICLO


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;ACELEROMETRO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ACELERACAO:     DEC     R6
                STOR    M[R6], R7
                
                MVI     R1, ACELEROMETRO
                LOAD    R1, M[R1]
                MVI     R2, 255
                JAL     DIVISAO ; a / 255
                
                MOV     R1, R3
                MVI     R2, 640 ; g em Q6
                JAL     PRODUTO ; a / 255 * g
                
                MVI     R1, aceleracao_x ; guarda em variavel em memoria
                STOR    M[R1], R3
                
                LOAD    R7, M[R6]
                INC     R6
                JMP     R7
                
                

; R1 - ACELERACAO LIDA DO ACELEROMETRO
; R2 - 255

DIVISAO:        DEC     R6
                STOR    M[R6], R4
                DEC     R6
                STOR    M[R6], R5
                
                MVI     R3, 0   
                
.LOOP:          SUB     R1, R1, R2
                BR.N    .Fim
                INC     R3
                BR      .LOOP
                
.Fim:           LOAD    R5, M[R6]
                INC     R6
                LOAD    R4, M[R6]
                INC     R6
                
                JMP     R7
                


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;PRODUTO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; R1 e R2 - NUMEROS A MULTIPLICAR

PRODUTO:        DEC     R6
                STOR    M[R6], R4
                DEC     R6
                STOR    M[R6], R5
                
                MVI     R3, 0
                CMP     R1, R0
                BR.Z    .Fim
                
.LOOP:          ADD     R3, R3, R2
                DEC     R1
                BR.NZ   .LOOP
                SHRA    R3
                SHRA    R3
                SHRA    R3
                SHRA    R3
                SHRA    R3
                SHRA    R3
                
                LOAD    R5, M[R6]
                INC     R6
                LOAD    R4, M[R6]
                INC     R6
                
.Fim:           JMP     R7

                
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;VELOCIDADE;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; R1 - VELOCIDADE ATUAL
; R2 - ACELERACAO

VELOCIDADE:     DEC     R6
                STOR    M[R6], R4
                DEC     R6
                STOR    M[R6], R5
                
                MOV     R4, R1 ; guardar a velocidade atual em R4
                MVI     R1, tempo
                
                DEC     R6
                STOR    M[R6], R7
                JAL     PRODUTO     ; ac * tempo
                LOAD    R7, M[R6]
                INC     R6
                
                MOV     R1, R4 ; ir buscar a velocidade atual
                ADD     R3, R3, R1  ; ac * tempo + vi
                
                MVI     R1, v_final 
                STOR    M[R1], R3 ; guardar resultado em variavel
                
                LOAD    R5, M[R6]
                INC     R6
                LOAD    R4, M[R6]
                INC     R6
                
                JMP     R7
                
                
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;POSICAO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                

; R1 - X ATUAL
; R2 - V ATUAL

POSICAO:        DEC     R6
                STOR    M[R6], R4
                DEC     R6
                STOR    M[R6], R5
                
                MOV     R4, R1 ; por x atual em R4
                MVI     R1, tempo
                
                DEC     R6
                STOR    M[R6], R7
                JAL     PRODUTO    ; tempo * vi
                LOAD    R7, M[R6]
                INC     R6
                
                MOV     R1, R4
                ADD     R3, R3, R1 ; xi + tempo * vi
                
                MVI     R1, 78
                CMP     R1, R3
                BR.NP   RESSALTO
                
                MVI     R1, 1
                CMP     R1,R3
                BR.P    RESSALTO
                
                ; guardar resultado na variavel x_final
                MVI     R1, x_final
                STOR    M[R1], R3
                
                LOAD    R5, M[R6]
                INC     R6
                LOAD    R4, M[R6]
                INC     R6
                
                JMP     R7

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; R1 - Posicao junto a parede

RESSALTO:       MOV     R3, R1
                MVI     R1, v_final
                LOAD    R2, M[R1]
                NEG     R2
                STOR    M[R1], R2
                
                
                ; guardar resultado na variavel x_final
                MVI     R1, x_final
                STOR    M[R1], R3
                
                LOAD    R5, M[R6]
                INC     R6
                LOAD    R4, M[R6]
                INC     R6
                
                JMP     R7



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;APARENCIA DO TERMINAL;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                

PAREDES:        
                MVI     R3, 80
.CICLO:         CMP     R3, R0
                JMP.Z   R7
                MVI     R1, TERM_WRITE
                MVI     R2, ASTERISCOS
                STOR    M[R1], R2
                DEC     R3
                BR      .CICLO
                
INIC_FIM:       MVI     R1, TERM_WRITE
                MVI     R2, ASTERISCOS
                STOR    M[R1], R2
                JMP     R7
              
ESPACO:         
                MVI     R3, 78
.CICLO:         CMP     R3, R0
                JMP.Z   R7
                MVI     R1, TERM_WRITE
                MVI     R2, ESPACOS
                STOR    M[R1], R2
                DEC     R3
                BR      .CICLO
                
                
                
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TIMER_BOOM:     ; posicionar cursor na antiga posicao
                MVI     R1, TERM_CURSOR
                MVI     R4, 0100h
                MVI     R5, x_atual
                LOAD    R5, M[R5]
                ADD     R4, R4, R5
                STOR    M[R1], R4
                
                ;apagar antiga posicao da bola
                MVI     R2, TERM_WRITE
                MVI     R5, ESPACOS
                STOR    M[R2], R5
                
                DEC     R6
                STOR    M[R6], R7
                DEC     R6
                STOR    M[R6], R4
                
                MVI     R1, flag_timer
                STOR    M[R1], R0
                
                JAL     ACELERACAO
                
                MVI     R1, v_atual
                LOAD    R1, M[R1]
                MVI     R2, aceleracao_x
                LOAD    R2, M[R2]
                JAL     VELOCIDADE
                
                MVI     R1, x_atual
                LOAD    R1, M[R1]
                MVI     R2, v_atual
                LOAD    R2, M[R2]
                JAL     POSICAO
                
                
                ; atualizar x e v atuais para o calculo seguinte
                MVI     R1, x_atual
                MVI     R4, x_final
                LOAD    R4, M[R4]
                STOR    M[R1], R4
                
                MVI     R2, v_atual
                MVI     R4, v_final
                LOAD    R4, M[R4]
                STOR    M[R2], R4
                
                
                ;posicionar cursor na nova posicao
                MVI     R1, TERM_CURSOR
                MVI     R4, 0100h
                MVI     R5, x_final
                LOAD    R5, M[R5]
                ADD     R4, R4, R5
                STOR    M[R1], R4
                
                
                ;escrever nova posicao da bola
                MVI     R2, TERM_WRITE
                MVI     R5, BOLA
                STOR    M[R2], R5
                
                LOAD    R4, M[R6]
                INC     R6
                LOAD    R7, M[R6]
                INC     R6
                
                JMP     R7
                
                
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INT_TIMER:      DEC     R6
                STOR    M[R6], R1
                DEC     R6
                STOR    M[R6], R2
                
                MVI     R1, flag_timer
                MVI     R2, 1
                STOR    M[R1], R2
                
                MVI     R1, TIMER_CONTROL
                STOR    M[R1], R2
                
                LOAD    R2, M[R6]
                INC     R6
                LOAD    R1, M[R6]
                INC     R6
                
                JMP     R7