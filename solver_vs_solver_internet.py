from selenium import webdriver
import os

#set up
set1 = [0.7, 20000] #for first_hand_solver [c,s]   c = explore weight
set2 = [0.3, 20000] #for second_hand_solver [c,s]  s = simulation times

controler_txt_path = "dataForControl.txt"
first_hand_solver = "solver\\MCTS-AMAF-SaveBridge.exe"
second_hand_solver = "solver\\MCTS-UCT-NoMinus.exe"
driver_path = "chromedriver.exe"


#list avaliable solvers
os.chdir("solver")
text = os.popen("dir")
os.chdir("../")

cnt = 0
solvers = []
print("#=====Avaliable Solvers=====#\n")
for line in text.readlines():
    s = line.split()
    if len(s) and s[-1].endswith("exe"):
            print(cnt,s[-1])
            solvers.append(s[-1])
            cnt += 1
print("\n#===========================#\n")

#solver setting
print("first hand solver : ",end="")
first_hand_solver = "solver\\" + solvers[int(input())]
print("c : ",end="")
set1[0] = float(input())
print("s : ",end = "")
set1[1] = int(input())

print("second hand solver : ",end="")
second_hand_solver = "solver\\" + solvers[int(input())]
print("c : ",end="")
set2[0] = float(input())
print("s : ",end = "")
set2[1] = int(input())

#open the website
driver = webdriver.Chrome(driver_path)
url = "http://www.lutanho.net/play/hex.html"
driver.get(url)
driver.set_window_position(0,0)
board = [[] for i in range(11)]                #every position on the board
cells = driver.find_elements_by_tag_name("img")
cnt = 0

for i in range(11):
    for j in range(i,-1,-1):
        board[j].append(cells[cnt])
        cnt += 1
for i in range(10):
    for j in range(10,i,-1):
        board[j].append(cells[cnt])
        cnt += 1
blue_player = driver.find_element_by_css_selector("input[value='Blue: Player']")

def write_board(f, movelist): #write to controler_txt
    #print(movelist)
    f.write("<board> ")
    f.write(movelist)
    f.write(" </board>\n")

def write_set(f,player):    #write to controler_txt
    f.write("<set> ")

    if player == 1:
        f.write(str(set1[0]))
        f.write(" ")
        f.write(str(set1[1]))
    else:
        f.write(str(set2[0]))
        f.write(" ")
        f.write(str(set2[1]))

    f.write(" </set>\n")

def get_solve(f):
    while 1:
        txt = f.readline().split()
        try:
            if txt[-1]=="</solve>": break
        except: pass
    return txt[len(txt)-2]

def play():
    cur_player = 1
    movelist = ""
    while 1:
        f = open(controler_txt_path, 'w+')
        write_set(f, cur_player)
        write_board(f, movelist)
        f.close()

        if cur_player == 1:
            os.system(first_hand_solver)
        else:
            os.system(second_hand_solver)

        f = open(controler_txt_path, 'r')
        move = get_solve(f)
        f.close()

        if move == "End": break
        else:
            movelist = movelist + " " + move
            r = int(move[1:])-1
            c = ord(move[0])-ord("A")
            board[r][c].click()

        cur_player = cur_player*(-1)


#start
blue_player.click()

play()
os.system("PAUSE")
driver.close()
