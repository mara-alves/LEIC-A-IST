# numbrix.py: Template para implementação do projeto de Inteligência Artificial 2021/2022.
# Devem alterar as classes e funções neste ficheiro de acordo com as instruções do enunciado.
# Além das funções e classes já definidas, podem acrescentar outras que considerem pertinentes.

# Grupo 38:
# 95625 Mara Alves
# 95627 Margarida Rodrigues
import sys
import copy
import time

from search import Problem, Node, astar_search, breadth_first_tree_search, depth_first_tree_search, greedy_search, \
    recursive_best_first_search
from utils import manhattan_distance


class NumbrixState:
    state_id = 0

    def __init__(self, board):
        self.board = board
        self.id = NumbrixState.state_id
        NumbrixState.state_id += 1

    def __lt__(self, other):
        return self.id < other.id

    # TODO: outros metodos da classe


class Board:
    """ Representação interna de um tabuleiro de Numbrix. """

    def __init__(self, n, positioned):
        """ Construtor da classe Board. """
        self.n = n
        self.positioned = positioned

    def get_number(self, row: int, col: int) -> int:
        """ Devolve o valor na respetiva posição do tabuleiro. """
        if row >= self.n or row < 0 or col >= self.n or col < 0:
            return None
        key_list = list(self.positioned.keys())
        val_list = list(self.positioned.values())
        if (row, col) in val_list:
            return key_list[val_list.index((row, col))]
        return 0

    def adjacent_vertical_numbers(self, row: int, col: int) -> (int, int):
        """ Devolve os valores imediatamente abaixo e acima,
        respectivamente. """
        bottom = self.get_number(row + 1, col)
        top = self.get_number(row - 1, col)
        return bottom, top

    def adjacent_horizontal_numbers(self, row: int, col: int) -> (int, int):
        """ Devolve os valores imediatamente à esquerda e à direita,
        respectivamente. """
        left = self.get_number(row, col - 1)
        right = self.get_number(row, col + 1)
        return left, right

    def closest_antecessor_sucessor(self, num: int):
        """ Devolve o antecessor e o sucessor mais proximo presentes
        no tabuleiro. """
        options = list(self.positioned.keys())
        options.append(num)
        options.sort()
        idx = options.index(num)
        if idx - 1 < 0:
            ant = None
        else:
            ant = options[idx - 1]
        if idx + 1 > len(self.positioned):
            suc = None
        else:
            suc = options[idx + 1]
        return ant, suc

    @staticmethod
    def parse_instance(filename: str):
        """ Lê o ficheiro cujo caminho é passado como argumento e retorna
        uma instância da classe Board. """
        f = open(filename, "r")
        n = int(f.readline())
        positioned = {}
        for i in range(n):
            line = f.readline()
            for j in range(n):
                num = int(line.split('\t')[j])
                if num != 0:
                    positioned[num] = (i, j)
        return Board(n, positioned)

    def to_string(self):
        """ Retorna a matriz numa string. """
        string = ""
        for i in range(self.n):
            for j in range(self.n):
                string += str(self.get_number(i, j)) + '\t'
            string = string[:-1]
            string += '\n'
        return string[:-1]

    # TODO: outros metodos da classe


class Numbrix(Problem):
    def __init__(self, board: Board):
        """ O construtor especifica o estado inicial. """
        self.initial = NumbrixState(board)

    def actions(self, state: NumbrixState):
        """ Retorna uma lista de ações que podem ser executadas a
        partir do estado passado como argumento. """
        board = state.board
        n = board.n
        positioned = state.board.positioned
        actions = []

        # lista dos numeros ainda nao posicionados
        possible = list(filter(lambda x: x not in positioned.keys(), range(1, n ** 2 + 1)))

        for x in range(n):
            for y in range(n):
                # primeira posicao vazia encontrada
                if (x, y) not in positioned.values():

                    # adjacentes desta posicao
                    adjacent = board.adjacent_horizontal_numbers(x, y)
                    adjacent += board.adjacent_vertical_numbers(x, y)

                    for poss in possible:
                        # verificar se os numeros antes e depois estao adjacentes
                        # ou se pelo menos ha espaço para eles
                        missing = 0
                        if poss - 1 > 0 and poss - 1 not in adjacent:
                            if poss - 1 in positioned:
                                continue
                            else:
                                missing += 1
                        if poss + 1 <= n ** 2 and poss + 1 not in adjacent:
                            if poss + 1 in positioned:
                                continue
                            else:
                                missing += 1

                        if missing <= adjacent.count(0):
                            # fazer cortes consoante as distancias
                            (suc, ant) = board.closest_antecessor_sucessor(poss)

                            if (suc is not None and manhattan_distance(positioned[suc], (x, y)) <= abs(suc - poss) or suc is None) and \
                                    (ant is not None and manhattan_distance(positioned[ant], (x, y)) <= abs(ant - poss) or ant is None):

                                # ver se numero nao bloqueia numeros posicionados adjacentemente
                                blocked = False
                                for a in list(filter(None, adjacent)):
                                    (i, j) = positioned[a]
                                    adjacent_2 = board.adjacent_horizontal_numbers(i, j)
                                    adjacent_2 += board.adjacent_vertical_numbers(i, j)
                                    missing = 0
                                    if a - 1 > 0 and a - 1 not in adjacent_2 and a - 1 != poss:
                                        missing += 1
                                    if a + 1 <= n ** 2 and a + 1 not in adjacent_2 and a + 1 != poss:
                                        missing += 1
                                    if adjacent_2.count(0) - 1 < missing:
                                        blocked = True
                                        break

                                if blocked is False:
                                    actions.append((x, y, poss))
                    return actions

        # nao ha posicao vazia -> []
        return actions

    def result(self, state: NumbrixState, action):
        """ Retorna o estado resultante de executar a 'action' sobre
        'state' passado como argumento. A ação a executar deve ser uma
        das presentes na lista obtida pela execução de
        self.actions(state). """
        new_state = copy.deepcopy(state)
        new_state.board.positioned[action[2]] = (action[0], action[1])
        return new_state

    def goal_test(self, state: NumbrixState):
        """ Retorna True se e só se o estado passado como argumento é
        um estado objetivo. Deve verificar se todas as posições do tabuleiro
        estão preenchidas com uma sequência de números adjacentes. """
        board = state.board
        n = board.n
        positioned = board.positioned
        current_num = 1
        if current_num not in positioned:
            return False
        (x, y) = positioned[current_num]

        while current_num < n ** 2:
            current_num += 1
            if current_num not in positioned:
                return False
            (x_new, y_new) = positioned[current_num]
            if x_new == x and (y_new == y - 1 or y_new == y + 1):
                y = y_new
            elif y_new == y and (x_new == x - 1 or x_new == x + 1):
                x = x_new
            else:
                return False
        return True

    def h(self, node: Node):
        """ Função heuristica utilizada para a procura A*. """
        return 0


if __name__ == "__main__":
    # Ler o ficheiro de input de sys.argv[1],
    board = Board.parse_instance(sys.argv[1])
    # Usar uma técnica de procura para resolver a instância,
    # Retirar a solução a partir do nó resultante,
    problem = Numbrix(board)
    goal_node = depth_first_tree_search(problem)
    # Imprimir para o standard output no formato indicado.
    print(goal_node.state.board.to_string())