;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;PROJETO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                ORIG    0
                MVI     R6, 8000h
                MVI     R5, 0070h
seno            STR     0000h,0001h,0002h,0003h,0004h,0006h,0007h,0008h,0009h,000Ah,000Bh,000Ch,000Dh,000Eh,000Fh,0011h,0012h,0013h,0014h,0015h,0016h,0017h,0018h,0019h,001Ah,001Bh,001Ch,001Dh,001Eh,001Fh,0020h,0021h,0022h,0023h,0024h,0025h,0026h,0027h,0027h,0028h,0029h,002Ah,002Bh,002Ch,002Ch,002Dh,002Eh,002Fh,0030h,0030h,0031h,0032h,0032h,0033h,0034h,0034h,0035h,0036h,0036h,0037h,0037h,0038h,0039h,0039h,003Ah,003Ah,003Ah,003Bh,003Bh,003Ch,72003Dh,003Dh,003Dh,003Eh,003Eh,003Eh,003Eh,003Fh,003Fh,003Fh,003Fh,003Fh,0040h,0040h,0040h,0040h,0040h,0040h,0040h,0040h
xi              WORD    0
vi              WORD    0
angulo          WORD    45  ; em graus
tempo           WORD    32  ; intervalo de tempo em segundos em formato Q.6
gravidade       WORD    640 ; aceleracao gravitica em Q.6
                JAL     aceleracao
                JAL     velocidade
                JAL     posicao
Fim:            BR      Fim

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;PRODUTO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Produto:        MVI     R3, 0
                CMP     R2, R0
                BR.Z    .Fim                
.Loop:          ADD     R3, R3, R4
                DEC     R2
                BR.NZ   .Loop
                SHR     R3    ; passar de Q.12 para Q.6 
                SHR     R3
                SHR     R3
                SHR     R3
                SHR     R3
                SHR     R3
.Fim:           JMP     R7

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;ACELERACAO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

aceleracao:     DEC     R6
                STOR    M[R6], R7
                MVI     R1, seno
                MVI     R2, angulo
                LOAD    R2, M[R2]
                MOV     R1, R2      ; buscar seno do angulo a memoria
                LOAD    R3, M[R1]
                MOV     R4, R3      ; mover seno para R4
                MVI     R2, gravidade
                LOAD    R2, M[R2]
                JAL     Produto     ; seno x gravidade
                STOR    M[R5], R3   ; guardar aceleracao em memoria
                DEC     R5
                LOAD    R7, M[R6]
                INC     R6
                JMP     R7
                
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;VELOCIDADE;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,

velocidade:     DEC     R6
                STOR    M[R6], R7
                MOV     R4, R3      ; mover aceleracao para R4
                MVI     R1, vi
                LOAD    R1, M[R1]
                MVI     R2, tempo
                LOAD    R2, M[R2] 
                JAL     Produto     ; ac * tempo
                ADD     R3, R3, R1  ; ac * tempo + vi
                STOR    M[R5], R3   ; guardar resultado em memoria
                DEC     R5
                LOAD    R7, M[R6]
                INC     R6
                JMP     R7
                
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;POSICAO;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                

posicao:        DEC     R6
                STOR    M[R6], R7
                MVI     R2, tempo
                LOAD    R2, M[R2]
                MOV     R4, R3     ; mover velocidade para R4
                JAL     Produto    ; tempo * vi
                MVI     R1, xi
                LOAD    R1, M[R1]
                ADD     R3, R3, R1 ; xi + tempo * vi
                STOR    M[R5], R3  ; guardar resultado em memoria
                DEC     R5
                LOAD    R7, M[R6]
                INC     R6
                JMP     R7