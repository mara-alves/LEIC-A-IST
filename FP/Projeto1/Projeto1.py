#numero 95625 Mara Gomes Alves


def eh_labirinto (maze):
    
    if not isinstance (maze, tuple): #verifica se e tuplo
        return False
    elif len(maze) < 3: #verifica minimo de 3 nas abcissas
        return False 
    
    #subtuplos
    else:
        for t in range(len(maze)):
            if not isinstance (maze[t], tuple): #dentro do tuplo principal so ha tuplos
                return False
            elif len(maze[t]) != len(maze[t-1]): #todos os tuplos tem o mesmo comprimento
                return False
            elif len(maze[t]) < 3: #verifica se cumpre o minimo 3 nas ordenadas
                return False
            
            #elementos dos subtuplos
            else:
                for e in range(len(maze[t])):
                    if type (maze[t][e]) != int: #corrige o problema de aceitar true e false como elementos
                        return False
                    elif maze[t][e] != 0 and maze[t][e] != 1: #elementos dos tuplos so podem ser 0 ou 1
                        return False                    
                    elif t == 0 and maze[t][e] != 1: #primeiro tuplo so paredes
                        return False
                    elif t == len(maze)-1 and maze[t][e] != 1: #ultimo tuplo so paredes
                        return False                    
                    elif (e == 0 and maze[t][e] != 1) or (e == len(maze[t])-1 and maze[t][e] != 1): #primeiro e ultimo elementos dos tuplos sao 1
                        return False                  
    return True




def eh_posicao (posicao):
    
    if not isinstance (posicao, tuple): #verifica se e um tuplo
        return False
    elif len(posicao) != 2: #verifica se tem apenas dois elementos
        return False

    else:
        for e in posicao:
            if not isinstance (e, int):
                return False
            if e < 0: #apenas aceita coordenadas positivas
                return False
    return True




def eh_conj_posicoes (unidades):
    
    if not isinstance (unidades, tuple):
        return False
    
    posicoes = ()
    for e in unidades:
        if not eh_posicao (e):
            return False
        else:
            if e in posicoes: #se for um elemento repetido, ou seja, se ja estiver na lista de posicoes, e falso
                return False
            else:
                posicoes = posicoes + (e,) #guardar posicoes num novo tuplo para o ciclo verificar se sao repetidas
    return True




def tamanho_labirinto (maze):
    
    if not eh_labirinto (maze):
        raise ValueError ('tamanho_labirinto: argumento invalido')
    
    return (len(maze), len(maze[0]))




def eh_mapa_valido (maze, unidades):
    
    if not eh_labirinto (maze) or not eh_conj_posicoes (unidades):
        raise ValueError ('eh_mapa_valido: algum dos argumentos e invalido')
    
    for t in range(len(unidades)):
        x = unidades[t][0]
        if x > len (maze): #ver se x das varias posicoes nao ultrapassa o comprimento do labirinto
            return False 
        y = unidades[t][1]
        if y > len (maze[0]): #ver se y das varias posicoes nao ultrapassa a altura do labirinto
            return False 
        if maze[x][y] == 1: #ver se posicao (x,y) nao calha numa posicao ocupada por parede
            return False
        
    return True




def eh_posicao_livre (maze, unidades, posicao):
    
    try: #verifica se as funcoes anteriores deram erro
        eh_mapa_valido (maze, unidades)
        eh_posicao(posicao)
    except ValueError: #se sim, esta funcao tambem da erro
        raise ValueError ('eh_posicao_livre: algum dos argumentos e invalido')
    
    if not eh_mapa_valido (maze, unidades) or not eh_posicao(posicao): #se as funcoes anteriores deram false esta da erro
        raise ValueError ('eh_posicao_livre: algum dos argumentos e invalido')
    for e in unidades:
        if e == posicao: #se a posicao e igual a alguma posicao ja ocupada pelas unidades entao e falso
            return False
        x = posicao[0]
        if x > len (maze): #ver se x da posicao nao ultrapassa o comprimento do labirinto
            return False 
        y = posicao[1]
        if y > len (maze[0]): #ver se y da posicao nao ultrapassa a altura do labirinto
            return False 
        if maze[x][y] == 1: #ver se posicao (x,y) nao calha numa posicao ocupada por parede
            return False
        
    return True




def posicoes_adjacentes (posicao):
    
    if not eh_posicao(posicao):
        raise ValueError('posicoes_adjacentes: argumento invalido')
    conj_posicoes = ()
    
    posicao1 = (posicao[0], posicao[1]-1) #calcular a primeira posicao e se for valida adiciona-la ao conjunto das posicoes
    if eh_posicao(posicao1):
        conj_posicoes = conj_posicoes + (posicao1,)
        
    posicao2 = (posicao[0]-1, posicao[1]) #mesmo processo para as restantes posicoes possiveis
    if eh_posicao(posicao2):
        conj_posicoes = conj_posicoes + (posicao2,)
        
    posicao3 = (posicao[0]+1, posicao[1])
    if eh_posicao(posicao3):
        conj_posicoes = conj_posicoes + (posicao3,)
        
    posicao4 = (posicao[0], posicao[1]+1)
    if eh_posicao(posicao4):
        conj_posicoes = conj_posicoes + (posicao4,)
        
    return conj_posicoes




def mapa_str (maze, unidades):
    
    #verificar se eh_mapa_valido da erro
    try: 
        eh_mapa_valido (maze, unidades)
    except ValueError: #se eh_mapa_valido da erro, mapa_str tambem
        raise ValueError ('mapa_str: algum dos argumentos e invalido')
    if not eh_mapa_valido (maze, unidades): #se eh_mapa_valido da false, mapa_str da erro
        raise ValueError ('mapa_str: algum dos argumentos e invalido')
    
    res = ('')
    linha = 0
    coluna = 0
    
    while coluna < len(maze) and linha < len(maze[coluna]):
        if (coluna, linha) in unidades: #se coordenadas forem iguais as de alguma posicao, assinalar com O
            res = res + 'O'
        elif maze[coluna][linha] == 1: #paredes
            res = res + '#'
        elif maze[coluna][linha] == 0: #espacos vazios
            res = res + '.'
        coluna = coluna + 1
        if coluna == len(maze) and linha + 1 < len(maze[coluna-1]): #quando acabar de percorrer uma linha e a proxima ainda existir
            res = res + '\n'
            coluna = 0 #recomecar colunas
            linha = linha + 1 #passar a proxima linha
            
    return res




def obter_objetivos (maze, unidades, posicao):
    
    #verificar erros
    try:
        eh_mapa_valido (maze, unidades)
        eh_posicao (posicao)
    except ValueError:
        raise ValueError ('obter_objetivos: algum dos argumentos e invalido')
    if not eh_mapa_valido or not eh_posicao (posicao) or (posicao not in unidades):
        raise ValueError ('obter_objetivos: algum dos argumentos e invalido')
    
    unidades_exceto_posicao = ()
    objetivos = ()
    
    #criar nova lista de unidades sem a posicao
    for u in unidades: 
        if u != posicao:
            unidades_exceto_posicao = unidades_exceto_posicao + (u,)
            
    for u in unidades_exceto_posicao: 
        #verficar se ha erros erros
        try: 
            posicoes_adjacentes (u)
        except ValueError:
            raise ValueError ('obter_objetivos: algum dos argumentos e invalido')
        
        for a in posicoes_adjacentes(u):
            #verficar se ha erros erros
            try: 
                eh_posicao_livre (maze, unidades, a)
            except ValueError:
                raise ValueError ('obter_objetivos: algum dos argumentos e invalido')
            
            if eh_posicao_livre (maze, unidades, a) and a not in objetivos: #se uma posicao adjacente a uma das unidade estiver livre e ainda nao estiver em objetivos, adiciona-se a objetivos
                objetivos = objetivos + (a,) 
                
    return objetivos




def obter_caminho (maze, unidades, posicao):
    
    #verificar erros
    try:
        eh_mapa_valido(maze, unidades) 
        eh_posicao (posicao)
    except ValueError:
        raise ValueError ('obter_caminho: algum dos argumentos e invalido')    
    if not eh_mapa_valido(maze,unidades) or (posicao not in unidades): #verificar validade dos argumentos
        raise ValueError ('obter_caminho: algum dos argumentos e invalido')
    
    #algoritmo
    lista_exploracao = [(posicao), ()]
    posicoes_visitadas = ()
    
    if obter_objetivos (maze, unidades, posicao) == (): #se a unidade ja estiver numa das posicoes de objetivo o caminho e ()
        return ()
    
    while lista_exploracao:
        posicao_atual = lista_exploracao[0]
        caminho_atual = lista_exploracao[1]
        
        if posicao_atual not in posicoes_visitadas: #marcar posicao como visitada se ainda nao o for
            posicoes_visitadas += (posicao_atual,)
            caminho_atual += (posicao_atual,)
            
            if posicao_atual in obter_objetivos (maze, unidades, posicao): #se chegou a um dos objetivos acaba
                return caminho_atual
            else:
                for posicao_adjacente in posicoes_adjacentes (posicao_atual):
                    if eh_posicao_livre(maze, unidades, posicao_adjacente): #se posicao adjacente da atual for valida adicionar a lista de exploracao essa posicao e o percurso ate la
                        lista_exploracao += (posicao_adjacente,) + (caminho_atual,)
                        
        del lista_exploracao[1] #apagar o que ja foi percorrido da lista para passar a proxima hipotese
        del lista_exploracao[0]       
        
    return ()
        



def mover_unidade (maze, unidades, posicao):
    
    #verificar erros
    try:
        eh_mapa_valido(maze, unidades)
        eh_posicao (posicao)
    except ValueError:
        raise ValueError ('mover_unidade: algum dos argumentos e invalido')
    if not eh_mapa_valido (maze, unidades) or (posicao not in unidades):
        raise ValueError ('mover_unidade: algum dos argumentos e invalido')
    
    novas_unidades = ()
    
    if obter_caminho(maze, unidades, posicao) == (): #se nao houver caminho nao fazer alteracao nenhuma a unidades
        return unidades
    
    for u in unidades: 
        if u in posicoes_adjacentes (posicao): #se a unidade ja estiver num dos objetivos nao sofre alteracao nenhuma
            return unidades
        elif u != posicao:
            novas_unidades += (u,) #se a posicao nao corresponder entao adiciona se a novas unidades
        elif u == posicao:
            novas_unidades += (obter_caminho(maze, unidades, posicao)[1],) #se a unidade for a posicao entao substitui se pelo passo seguinte do caminho
            
    return novas_unidades
