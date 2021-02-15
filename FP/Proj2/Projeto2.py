# Mara Gomes Alves, numero 95625

#############################
#        TAD posicao        #
#############################

# posicao -> (x, y)

''' Construtores '''

# devolve posicao correspondente a coordenadas
def cria_posicao (x, y):
    if type (x) is not int or type (y) is not int:
        raise ValueError ('cria_posicao: argumentos invalidos')
    if x < 0 or y < 0:
        raise ValueError ('cria_posicao: argumentos invalidos')
    else:
        return x, y


# copia posicao
def cria_copia_posicao (p):
    return cria_posicao (p[0], p[1])



''' Seletores '''

# obter x da posicao
def obter_pos_x (posicao):
    return posicao[0]


# obter y da posicao
def obter_pos_y (posicao):
    return posicao[1]



''' Reconhecedor '''

# ver se e uma posicao
def eh_posicao (arg):
    if not isinstance (arg, tuple) or len (arg) != 2:
        return False
    if type(obter_pos_x (arg)) is not int or obter_pos_x (arg) < 0:
        return False
    if type(obter_pos_y (arg)) is not int or obter_pos_y (arg) < 0:
        return False
    return True



''' Teste '''

# verificar se duas posicoes sao iguais
def posicoes_iguais (p1, p2):
    return p1 == p2



''' Transformador '''

# devolve posicao em string
def posicao_para_str (p):
    return str(p)



''' 
Funcao de alto nivel 
'''

# devolve tuplo com posicoes adjacentes a p
def obter_posicoes_adjacentes (p):
    conj_posicoes = ()
    
    # calcular primeira posicao adjacente se possivel
    if obter_pos_y (p)-1 >= 0:
        posicao1 = cria_posicao (obter_pos_x (p), obter_pos_y (p)-1)
        conj_posicoes += (posicao1,)
    # segunda se possivel
    if obter_pos_x (p)-1 >= 0:
        posicao2 = cria_posicao (obter_pos_x (p)-1, obter_pos_y (p))
        conj_posicoes += (posicao2,)
    # terceira sera sempre possivel
    posicao3 = cria_posicao (obter_pos_x (p)+1, obter_pos_y (p))
    conj_posicoes += (posicao3,)
    # quarta tambem
    posicao4 = cria_posicao (obter_pos_x (p), obter_pos_y (p)+1)
    conj_posicoes += (posicao4,)
        
    return conj_posicoes



#############################
#        TAD unidade        #
#############################

# unidade -> {p: , v: , f: , e: }

''' Construtores '''

def cria_unidade (p, v, f, e):
    if v <= 0 or f <= 0 or (type(v) is not int) or (type(f) is not (int)) \
       or not eh_posicao(p) \
       or not isinstance (e, str) or e == '':
        raise ValueError ('cria_unidade: argumentos invalidos')
    else:
        return {'p': p, 'v': v, 'f': f, 'e': e}
    
    
def cria_copia_unidade (u):
    return cria_unidade (u['p'], u['v'], u['f'], u['e'])



''' Seletores '''

def obter_posicao (u):
    return u['p']

def obter_exercito (u):
    return u['e']

def obter_forca (u):
    return u['f']

def obter_vida (u):
    return u['v']



''' Modificadores '''

# altera posicao de uma unidade
def muda_posicao (u, p):
    u['p'] = p
    return u



# tira vida a uma unidade
def remove_vida (u, v):
    u['v'] = obter_vida (u) - v
    # caso de negaivo, pomos vida a 0
    if obter_vida (u) < 0:
        u['v'] = 0
    return u



''' Reconhecedor '''

def eh_unidade (arg):
 
    if not isinstance (arg, dict) \
       or len (arg) != 4 \
       or 'p' not in arg \
       or 'v' not in arg \
       or 'f' not in arg \
       or 'e' not in arg:
        return False
    
    if obter_vida(arg) <= 0 or obter_forca(arg) <= 0 or \
       (type(obter_vida(arg)) is not int) or (type(obter_forca(arg)) is not (int)) \
       or not eh_posicao(obter_posicao(arg)) \
       or not isinstance (obter_exercito(arg), str) or obter_exercito(arg) == '':
        return False
    
    return True



''' Teste '''

def unidades_iguais (u1, u2):
    return u1 == u2



''' Transformadores '''

def unidade_para_char (u):  
    exercito = obter_exercito (u)  
    # primeira letra ja for maiuscula
    if 'A' <= exercito[0] <= 'Z':
        return exercito[0]
    # passar a maiuscula se nao
    else:
        return chr(ord(exercito[0])-32)



def unidade_para_str (u):
    return unidade_para_char(u) + str([obter_vida (u), obter_forca (u)]) + '@' + str(obter_posicao (u))



'''
Funcoes de alto nivel
'''

# retira a vida de u2 a forca de u1
def unidade_ataca (u1, u2):
    remove_vida (u2, obter_forca (u1))
    # morreu
    if obter_vida (u2) <= 0:
        return True
    # sobreviveu
    else:
        return False


# bubble sort
def ordenar_unidades (t):
    
    t = list(t)
    for j in range(len(t)):
        
        for i in range(len(t)-1-j):
            
            # se y do anterior for maior, troca
            if obter_pos_y (obter_posicao (t[i])) > obter_pos_y (obter_posicao(t[i+1])):
                t[i], t[i+1] = t[i+1], t[i]
                
            # se y forem iguais comparar os x
            elif obter_pos_y (obter_posicao(t[i])) == obter_pos_y (obter_posicao(t[i+1])):
                if obter_pos_x (obter_posicao(t[i])) > obter_pos_x (obter_posicao(t[i+1])):
                    t[i], t[i+1] = t[i+1], t[i]
                    
    return tuple(t)



#############################
#          TAD mapa         #
#############################


''' Construtores '''

# mapa -> {d: , w: , e1: , e2: }

def cria_mapa (d, w, e1, e2):
    
    if not isinstance (d, tuple) or len (d) != 2:
        raise ValueError ('cria_mapa: argumentos invalidos')
    # verificar tamanho do labirinto
    if d[0] < 3 or d[1] < 3:
        raise ValueError ('cria_mapa: argumentos invalidos')
    
    # w: tuplo de posicoes
    if not isinstance (w, tuple):
        raise ValueError ('cria_mapa: argumentos invalidos')
    for p in w:
        if not eh_posicao (p) \
           or obter_pos_x (p) >= d[0] or obter_pos_x (p) == 0\
           or obter_pos_y (p) >= d[1] or obter_pos_y (p) == 0:
            raise ValueError ('cria_mapa: argumentos invalidos')
    
    # e1, e2: tuplos de uma ou mais unidades
    if len (e1) < 1 or len (e2) < 1 or not isinstance (e1, tuple) or not isinstance (e2, tuple):
        raise ValueError ('cria_mapa: argumentos invalidos')
    for u1 in e1:
        if not eh_unidade (u1):
            raise ValueError ('cria_mapa: argumentos invalidos')
    for u2 in e2:
        if not eh_unidade (u2):
            raise ValueError ('cria_mapa: argumentos invalidos')        
    
    return {'d': d, 'w': w, 'e1': list(e1), 'e2': list(e2)}
    
    
    
def cria_copia_mapa (m):
    m_novo = {}
    m_novo['d'] = m['d']
    m_novo['w'] = m['w']
    m_novo['e1'] = list(dict(u1) for u1 in m['e1'])
    m_novo['e2'] = list(dict(u2) for u2 in m['e2'])    
    return m_novo



''' Seletores '''

def obter_tamanho (m):
    return m['d']



# tuplo por ordem alfabetica
def obter_nome_exercitos (m):
    
    # caso algum exercito nao exista
    if not m['e1'] and not m['e2']:
        return ()
    elif not m['e1']:
        return obter_exercito (m['e2'][0])
    elif not m['e2']:
        return obter_exercito (m['e1'][0]) 
        
    else:
        nome1 = obter_exercito (m['e1'][0])
        nome2 = obter_exercito (m['e2'][0])
        if nome1 > nome2:
            return nome2, nome1
        else:
            return nome1, nome2



def obter_unidades_exercito (m, e):
    res = ()
    for u in obter_todas_unidades (m):
        if obter_exercito (u) == e:
            res += (u,)
    return ordenar_unidades (res)

    
    
def obter_todas_unidades (m):
    return ordenar_unidades (m['e1'] + m['e2'])



# devolve unidade que esta em p
def obter_unidade (m, p):
    for u in obter_todas_unidades (m):
        if obter_posicao (u) == p:
            return u



''' Modificadores '''

def eliminar_unidade (m, u):
    # apagar se estiver no primeiro exercito
    m['e1'] = list (filter (lambda x: x != u, m['e1']))
    # apagar se estiver no segundo exercito
    m['e2'] = list (filter (lambda x: x != u, m['e2']))
    return m


# mover u para p
def mover_unidade (m, u, p):
    u['p'] = p
    return m



''' Reconhecedores '''

# ve se em p esta uma unidade
def eh_posicao_unidade (m, p):
    for u in obter_todas_unidades (m):
        if obter_posicao (u) == p:
            return True     
    return False



# ver se posicao nao e uma parede
def eh_posicao_corredor (m, p):
    if p in m['w'] \
       or obter_pos_x (p) == 0 \
       or obter_pos_x (p) == obter_pos_x (obter_tamanho (m)) -1 \
       or obter_pos_y (p) == 0 \
       or obter_pos_y (p) == obter_pos_y (obter_tamanho (m)) -1:
        return False
    return True



# oposto da funcao anterior
def eh_posicao_parede (m, p):
    return not eh_posicao_corredor (m, p)



''' Teste '''

def mapas_iguais (m1, m2):
    return m1 == m2



''' Transformador '''

def mapa_para_str (m):
    
    coluna = ''
    res = ''
    
    for y in range(obter_pos_y (obter_tamanho (m))):
        for x in range(obter_pos_x (obter_tamanho (m))):
    
            if eh_posicao_parede (m, cria_posicao(x,y)):
                coluna += '#'
                
            elif eh_posicao_corredor (m, cria_posicao(x,y)):
                # pode ser posicao livre ou ter unidade
                if eh_posicao_unidade (m, cria_posicao(x, y)):
                    coluna += unidade_para_char (obter_unidade (m, cria_posicao(x,y)))
                else:
                    coluna += '.'
            
        # linha acaba
        res += coluna
        coluna = ''
        
        # se ainda nao for a ultima linha
        if y < obter_pos_y (m['d'])-1:
            res += '\n'
        
    return res



'''
Funcoes de alto nivel
'''

def obter_inimigos_adjacentes (m, u):
    
    exercito_u = obter_unidades_exercito (m, obter_exercito (u))
    adjacentes = obter_posicoes_adjacentes (obter_posicao (u))
    
    def aux (unidades, inimigos):
        if unidades == ():
            return inimigos
        elif unidades[0] not in exercito_u and obter_posicao(unidades[0]) in adjacentes:
            return aux (unidades[1:], inimigos + (unidades[0],))
        else:
            return aux (unidades[1:], inimigos)

    return aux (obter_todas_unidades (m), ())



def obter_movimento(mapa, unit):
    '''
    A funcao obter_movimento devolve a posicao seguinte da unidade argumento
    de acordo com as regras de movimento das unidades no labirinto.

    obter_movimento: mapa x unidade -> posicao
    '''

    ######################
    # Funcoes auxiliares #
    ######################
    def pos_to_tuple(pos):
        return obter_pos_x(pos), obter_pos_y(pos)

    def tuple_to_pos(tup):
        return cria_posicao(tup[0], tup[1])

    def tira_repetidos(tup_posicoes):
        conj_tuplos = set(tuple(map(pos_to_tuple, tup_posicoes)))
        return tuple(map(tuple_to_pos, conj_tuplos))

    def obter_objetivos(source):
        enemy_side = tuple(filter(lambda u: u != obter_exercito(source), obter_nome_exercitos(mapa)))[0]
        target_units = obter_unidades_exercito(mapa, enemy_side)
        tup_com_repetidos = \
            tuple(adj
                  for other_unit in target_units
                  for adj in obter_posicoes_adjacentes(obter_posicao(other_unit))
                  if eh_posicao_corredor(mapa, adj) and not eh_posicao_unidade(mapa, adj))
        return tira_repetidos(tup_com_repetidos)

    def backtrack(target):
        result = ()
        while target is not None:
            result = (target,) + result
            target, _ = visited[target]
        return result

    ####################
    # Funcao principal #
    ####################
    # Nao mexer se ja esta' adjacente a inimigo
    if obter_inimigos_adjacentes(mapa, unit):
        return obter_posicao(unit)

    visited = {}
    # posicao a explorar, posicao anterior e distancia
    to_explore = [(pos_to_tuple(obter_posicao(unit)), None, 0)]
    # registro do numero de passos minimo ate primeira posicao objetivo
    min_dist = None
    # estrutura que guarda todas as posicoes objetivo a igual minima distancia
    min_dist_targets = []

    targets = tuple(pos_to_tuple(obj) for obj in obter_objetivos(unit))

    while to_explore:  # enquanto nao esteja vazio
        pos, previous, dist = to_explore.pop(0)

        if pos not in visited:  # posicao foi ja explorada?
            visited[pos] = (previous, dist)  # registro no conjunto de exploracao
            if pos in targets:  # se a posicao atual eh uma dos objetivos
                # se eh primeiro objetivo  ou se esta a  distancia minima
                if min_dist is None or dist == min_dist:
                    # acrescentor 'a lista de posicoes minimas
                    min_dist = dist
                    min_dist_targets.append(pos)
            else:  # nao 'e objetivo, acrescento adjacentes
                for adj in obter_posicoes_adjacentes(tuple_to_pos(pos)):
                    if eh_posicao_corredor(mapa, adj) and not eh_posicao_unidade(mapa, adj):
                        to_explore.append((pos_to_tuple(adj), pos, dist + 1))

        # Parar se estou a visitar posicoes mais distantes que o minimo,
        # ou se ja encontrei todos os objetivos
        if (min_dist is not None and dist > min_dist) or len(min_dist_targets) == len(targets):
            break

    # se encontrei pelo menos uma posicao objetivo, 
    # escolhe a de ordem de leitura menor e devolve o primeiro movimento
    if len(min_dist_targets) > 0:
        # primeiro dos objetivos em ordem de leitura
        tar = sorted(min_dist_targets, key=lambda x: (x[1], x[0]))[0]
        path = backtrack(tar)
        return tuple_to_pos(path[1])

    # Caso nenhuma posicao seja alcancavel
    return obter_posicao(unit)



###########################################
#                                         #
#            FUNCOES ADICIONAIS           #
#                                         #
###########################################


# pontos = soma vida das unidades de e
def calcula_pontos (m, e):

    def aux (pontos, unidades):
        if unidades == ():
            return pontos
        else:
            return aux (pontos + obter_vida (unidades[0]), unidades[1:])
    
    return aux (0, obter_unidades_exercito (m,e))



def simula_turno (m):
    
    for u in obter_todas_unidades (m):
        
        mover_unidade (m, u, obter_movimento(m, u))
        inimigos = obter_inimigos_adjacentes (m, u)
        
        if inimigos != () and obter_vida (u) > 0:
            if unidade_ataca (u, inimigos[0]):
                eliminar_unidade (m, inimigos[0])
    return m



def simula_batalha (ficheiro, standard):
    
    # ler ficheiro e guardar valores
    f = open(ficheiro, 'r')
    d = eval(f.readline())
    e1 = eval(f.readline())
    e2 = eval(f.readline())
    w = eval(f.readline())
    u1 = eval(f.readline())
    u2 = eval(f.readline())
    
    ''' 
    e1[0] - nome do exercito
    e1[1] - vida das unidades
    e1[2] - forca das unidades
    '''    
    
    # criar exercitos
    exercito1 = tuple(cria_unidade (p1, e1[1], e1[2], e1[0]) for p1 in u1)
    exercito2 = tuple(cria_unidade (p2, e2[1], e2[2], e2[0]) for p2 in u2)
    
    # mapa inicial
    m = cria_mapa (d, w, exercito1, exercito2)
    
    # obter nomes por ordem
    nomes = obter_nome_exercitos (m)   
    if nomes[0] == e1[0]:
        nome1 = e1[0]
        nome2 = e2[0]
    else:
        nome1 = e2[0]
        nome2 = e1[0]
        
    # pontuacao inicial
    pontos1 = calcula_pontos (m, nome1)
    pontos2 = calcula_pontos (m, nome2)


############################# Funcoes auxiliares ###############################
        
    def prints ():
        print (mapa_para_str(m))
        print ('[ ' + nome1 +':' + str(calcula_pontos (m, nome1)) + ' ' + nome2 + ':' + str(calcula_pontos (m, nome2)) + ' ]')
        
    def aux (m, pontos1, pontos2, standard):
        # casos terminais
        if pontos1 == 0:
            prints()            
            return nome2
        elif pontos2 == 0:
            prints()            
            return nome1
        elif m == simula_turno(cria_copia_mapa (m)):
            prints()            
            return 'EMPATE'
        # passar a proximo turno
        else:
            # prints em todos os turnos
            if standard == True:
                prints()
                return aux (simula_turno(m), calcula_pontos (m, nome1), calcula_pontos (m, nome2), standard)
            # em modo quiet so faz mais o print final
            else:
                return aux (simula_turno(m), calcula_pontos (m, nome1), calcula_pontos (m, nome2), standard)


################################################################################

    # em modo quiet fazer print inicial
    if standard == False:
        prints ()
        
    f.close()
                
    return aux(m, pontos1, pontos2, standard)