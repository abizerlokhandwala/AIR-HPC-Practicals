import copy
import queue as q

class game:
    def __init__(self):
        self.size = 0
        self.init_gameboard = [[]]
        self.final_gameboard = [[]]
        self.final_mapping = {}
        self.visited = set()

    def user_input(self):
        print("Enter board size: ")
        self.size = int(input())

        self.init_gameboard = [[0 for x in range(self.size)] for y in range(self.size)]
        self.final_gameboard = [[0 for x in range(self.size)] for y in range(self.size)]

        print("Enter initial configuration: ")
        for i in range(0,self.size):
            self.init_gameboard[i] = list(map(int,input().strip().split(' ')))

        print("Enter final configuration: ")
        for i in range(0,self.size):
            self.final_gameboard[i] = list(map(int,input().strip().split(' ')))

        for i in range(self.size):
            for j in range(self.size):
                self.final_mapping[self.final_gameboard[i][j]] = (i,j)

    def findZero(self, board):
        for i in range(self.size):
            for j in range(self.size):
                if board[i][j]==0:
                    return (i,j)

    def issafe(self, x, y):
        if x>=0 and x<self.size and y>=0 and y<self.size:
            return True
        return False

    def getValidMoves(self, board, pos_zero):
        dx = [-1,0,1,0]
        dy = [0,-1,0,1]
        valids = []
        for i in range(4):
            row = pos_zero[0]+dx[i]
            col = pos_zero[1]+dy[i]
            if self.issafe(row,col)==True:
                valids.append([row,col])
        return valids

    def calculate_heuristic(self,board):
        score = 0
        for i in range(self.size):
            for j in range(self.size):
                x, y = self.final_mapping[board[i][j]]
                score += abs(i-x) + abs(j-y)
        return score

    def play(self):
        pos_zero = self.findZero(self.init_gameboard)
        moves = self.getValidMoves(self.init_gameboard,pos_zero)
        board = self.init_gameboard
        pq = q.PriorityQueue()
        pq.put((0,0,board))
        steps = 0
        parent = {}
        flag = 0
        while pq.qsize()>0:
            top = pq.get()
            g = int(top[1])
            board = top[2]
            if board == self.final_gameboard:
                steps = g
                flag = 1
                break;
            pos_zero = self.findZero(board)
            moves = self.getValidMoves(board, pos_zero)
            g = g+1
            for i in moves:
                board_temp = copy.deepcopy(board)
                board_temp[pos_zero[0]][pos_zero[1]] = board_temp[i[0]][i[1]]
                board_temp[i[0]][i[1]] = 0
                if str(board_temp) not in self.visited:
                    self.visited.add(str(board_temp))
                    h = self.calculate_heuristic(board_temp)
                    # h = 0
                    f = g + h
                    pq.put((f,g,board_temp))
                    parent[str(board_temp)] = board

        if flag == 0:
            print("Not Solvable")
            return 0
        board = copy.deepcopy(self.final_gameboard)
        board_moves = []
        while(board != self.init_gameboard):
            board_moves.append(board)
            board = parent[str(board)]

        board_moves.append(self.init_gameboard)
        board_moves.reverse()

        for i in range(len(board_moves)):
            print("\nMove",i)
            for row in range(self.size):
                for col in range(self.size):
                    print(board_moves[i][row][col], end = ('\n' if col == self.size-1 else ' '))

        print("\nSteps: ", steps)
obj = game()
obj.user_input()
obj.play()
