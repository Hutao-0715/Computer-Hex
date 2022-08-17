def xy2str(row,column):
        ans = ""
        ans += chr(row-1+ord("A"))
        ans += str(column)
        return ans

def creat_smask(size): #crear search mask
    s = [[0 for i in range(size+2)] for j in range(size+2)]
    for i in range(1,size+1):
        for j in range(1,size+1):
            s[i][j]=1
    return s

def dfs(self,search_mask,now,player): 
    #string last,now. int player=1 or -1. int[][] search_mask (used to mark searched position)
    #print(now)
    #search_mask 0=searched
    ans = []
    x = ord(now[0])-ord("A")+1             
    y = int(now[1:])
    if player==1 and x==self.size: return 1
    elif player==-1 and y==self.size: return 1
    search_mask[x][y] = 0
    if self.board[x-1][y]==player and search_mask[x-1][y]!=0:
        tmp = xy2str(x-1,y)
        ans.append(dfs(self,search_mask,tmp,player))
    if self.board[x-1][y+1]==player and search_mask[x-1][y+1]!=0:
        tmp = xy2str(x-1,y+1)
        ans.append(dfs(self,search_mask,tmp,player))
    if self.board[x+1][y]==player and search_mask[x+1][y]!=0:
        tmp = xy2str(x+1,y)
        ans.append(dfs(self,search_mask,tmp,player))
    if self.board[x+1][y-1]==player and search_mask[x+1][y-1]!=0:
        tmp = xy2str(x+1,y-1)
        ans.append(dfs(self,search_mask,tmp,player))
    if self.board[x][y-1]==player and search_mask[x][y-1]!=0:
        tmp = xy2str(x,y-1)
        ans.append(dfs(self,search_mask,tmp,player))
    if self.board[x][y+1]==player and search_mask[x][y+1]!=0:
        tmp = xy2str(x,y+1)
        ans.append(dfs(self,search_mask,tmp,player))
    if 1 in ans: return 1
    else: return 0

def connect(self,pos,player):
    #print("in connect")
    #hex_board self, string pos, int player
    #check if this move is connect with other chess
    x = ord(pos[0])-ord("A")+1
    y = int(pos[1:])
    if self.board[x-1][y]==player: return 1
    if self.board[x-1][y+1]==player: return 1
    if self.board[x+1][y]==player: return 1
    if self.board[x+1][y-1]==player: return 1
    if self.board[x][y-1]==player: return 1
    if self.board[x][y+1]==player: return 1
    else: return 0

class hex_board:
    def __init__(self,size): #int size
         
        self.size = size #int size
        self.board = [[0 for i in range(size+2)] for j in range(size+2)] #board is a 2D int_array
        self.mask = [[0 for i in range(size+2)] for j in range(size+2)]  #0 and 1 matrix. 1 mean availble for move
        self.pre_move = []                                               #pre_move for previous move
        self.chess_on_edge = [[],[],[]]                 #second subarray for player 1,third is for player -1
        self.winner = 0
    
    def build(self):                    
        #for build or initialization.
        #After using this, the 4 sides of board will be marked with 1 and -1, refer to two player's camps
        self.board = [[0 for i in range(self.size+2)] for j in range(self.size+2)]
        self.mask = [[0 for i in range(self.size+2)] for j in range(self.size+2)]
        self.pre_move = []
        self.chess_on_edge = [[],[],[]]
        self.winner = 0
        for i in range(self.size+2):
            if i==0 or i==self.size+1:
                for j in range(self.size+2):
                    self.board[i][j]=1
            else:
                for j in range(self.size+2):
                    if j==0 or j==self.size+1: self.board[i][j]=-1
                    else:
                        self.board[i][j]=0
                        self.mask[i][j]=1
            #print(self.board)

    def b_print(self):
        for i in range(self.size+2):
            #print coordinate (for row)
            if i!=0 and i!= self.size+1:
                print(chr((i-1)+ord("A")),end="")
            else: print("#",end="")
            #print value in self.board
            for j in range(i): print("  ",end="")
            for j in range(self.size+2):
                if (self.board[i][j]==0 or self.board[i][j]==1): print(" ",end="")
                print(" ",self.board[i][j],end="")
            print("")
        #print coordinate (for column)
        print("  "*(self.size+3),end="")
        for i in range(self.size+2):
            if i==0 or i==self.size+1: print("  #",end="")
            elif i<10: print("  ",i,end="")
            else: print(" ",i,end="")
        print("")

    def win(self,player): #check if this player wins
        s = creat_smask(self.size)
        if player==1:
            for i in range(1,self.size+1):
                now = xy2str(1,i)
                if dfs(self,s,now,1)==1:
                    self.winner = 1
                    return 1
            return 0
        else:
            for i in range(1,self.size+1):
                now = xy2str(i,1)
                if dfs(self,s,now,-1)==1:
                    self.winner = -1
                    return -1
            return 0
    def move(self,player,m): 
        #int player = 1 or -1. string array m. Ex: ["A5","E2"].
        # #player is the person who make the first move in m.
        # #first(letter) for row, second(number) for column. Left up coner is "A1".
        i=0                  
        while i<len(m):      
            x = ord(m[i][0])-ord("A")+1   #for row
            y = int(m[i][1:])             #for column
            if len(self.pre_move)==0: self.board[x][y] = player
            elif len(self.pre_move)==1:
                if m[i]==self.pre_move[0]:  #swap
                    self.board[x][y] = 0
                    self.board[y][x] = player
                    self.mask[y][x] = 0
                else:
                    self.board[x][y] = player
                    self.mask[x][y] = 0
                    x = ord(self.pre_move[0][0])-ord("A")+1
                    y = int(self.pre_move[0][1:])
                    self.mask[x][y] = 0
            else:
                self.board[x][y] = player
                self.mask[x][y] = 0
            self.pre_move.append(m[i])
            self.win(player)
            i += 1
            player *= -1

'''
#example usage
m=["A3","A2"]    #The moves players make
test = hex_board(5)   #Declare a hex_board and its size
test.build()          #Build up the board. This MUST be done before any further operation
test.move(1,m)        #Make moves. Detail of argument is at definition area
test.b_print()        #Show present board
print(test.mask)
print(test.winner)
print(test.win(1))
#print(s)'''
#path = 'C:\Users\bendo\OneDrive\桌面\程式設計\Python\專題\test_data.txt'
f = open(r"C:\Users\bendo\OneDrive\桌面\程式設計\Python\專題\test_data.txt",'r',encoding = "UTF-8")
i=1
test = hex_board(11)

for s in f.readlines():
    test.build()
    s = s.split()
    m = s[:-1]
    ans = int(s[-1])
    test.move(-1,m)
    #test.b_print()
    print("line",i,end="")
    if test.winner==ans: print(" pass")
    else: print(" WA:",test.winner)
    i += 1
    
