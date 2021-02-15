from math import sin
from math import pi

def aceleracao (ang_g, g):
    ang_r = (ang_g * pi)/180
    ac = sin (ang_r) * g
    return ac

def velocidade (vi, ac, t):
    v = vi + ac * t
    return v

def posicao (xi, v, t):
    x = xi + v * t
    return x    
    
def ciclo(xi,vi,ang_g,g,t):
    ac=aceleracao(ang_g,g)
    c=0
    while c<=t:
        v=velocidade(vi,ac,c)
        x=posicao(xi,v, c)
        print ('velocidade =', v, 'e posicao =', x, 'para tempo =', c)
        c+=1/4