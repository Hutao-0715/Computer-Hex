import os
import pygame
from pygame.locals import QUIT
import sys
from math import sqrt

controler_txt_path = "dataForControl.txt"
first_hand_solver = "solver\\MCTS-AMAF-SaveBridge.exe"
second_hand_solver = "solver\\MCTS-AMAF-SaveBridge.exe"

set1 = [0.7, 200] #for first_hand_solver [c,s]   c = explore weight
set2 = [0.3, 200] #for second_hand_solver [c,s]  s = simulation times

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


buttons = dict()
board_size = 11
board = [[] for i in range(board_size)]
length = 25
color = [(0,0,0),(255,0,0),(0,0,255)] #[non,red,blue]
cur_player = 1

movelist = []
_now = 0

def draw_cell(surface,c,len,color_cell,color_line):   # c for center
    point = []
    point.append((c[0], c[1]-len))
    point.append((c[0]+(len/2)*sqrt(3), c[1]-(len/2)))
    point.append((c[0]+(len/2)*sqrt(3), c[1]+(len/2)))
    point.append((c[0], c[1]+len))
    point.append((c[0]-(len/2)*sqrt(3), c[1]+(len/2)))
    point.append((c[0]-(len/2)*sqrt(3), c[1]-(len/2)))
    pygame.draw.polygon(surface,color_cell,point)
    for i in range(1,6):
        pygame.draw.line(surface,color_line,point[i-1],point[i],2)

def creat_button(surface,area,button_color,text,text_color,text_size): #area is a tuple (left,top,width,height)
    #button icon
    button_surface = pygame.Surface((area[2],area[3]))
    button_surface.fill(button_color)
    surface.blit(button_surface,(area[0],area[1]))

    #button text
    head_font = pygame.font.SysFont(None,text_size)
    txt_surface = head_font.render(text,True,text_color)
    surface.blit(txt_surface,(area[0],area[1]))

    #add to buttons
    global buttons
    buttons[text] = pygame.Rect(area[0],area[1],area[2],area[3])

def build_board(pos,len):   #set up an 2D array which record all the cell's center position
                            #pos is the center position of the top left cell
    board[0].append(pos)
    for i in range(1,board_size):
        board[0].append((board[0][i-1][0]+len*sqrt(3),board[0][i-1][1]))
    for i in range(board_size): #column
        for j in range(1,board_size): #row
            board[j].append((board[j-1][i][0]+len*(sqrt(3)/2), board[j-1][i][1]+len*(3/2)))

def draw_edge(surface,len):
    #red
    for i in range(board_size):
        c = board[0][i]
        pos1 = (c[0]-(len/2)*sqrt(3), c[1]-(len/2))
        pos2 = (c[0], c[1]-len)
        pos3 = (c[0]+(len/2)*sqrt(3), c[1]-(len/2))
        pygame.draw.line(surface,(255,0,0),pos1,pos2,4)
        pygame.draw.line(surface,(255,0,0),pos2,pos3,4)
    for i in range(board_size):
        c = board[board_size-1][i]
        pos1 = (c[0]+(len/2)*sqrt(3), c[1]+(len/2))
        pos2 = (c[0], c[1]+len)
        pos3 = (c[0]-(len/2)*sqrt(3), c[1]+(len/2))
        pygame.draw.line(surface,(255,0,0),pos1,pos2,4)
        pygame.draw.line(surface,(255,0,0),pos2,pos3,4)
    #blue
    for i in range(board_size):
        c = board[i][0]
        pos1 = (c[0], c[1]+len)
        pos2 = (c[0]-(len/2)*sqrt(3), c[1]+(len/2))
        pos3 = (c[0]-(len/2)*sqrt(3), c[1]-(len/2))
        pygame.draw.line(surface,(0,0,255),pos1,pos2,4)
        pygame.draw.line(surface,(0,0,255),pos2,pos3,4)
    for i in range(board_size):
        c = board[i][board_size-1]
        pos1 = (c[0], c[1]-len)
        pos2 = (c[0]+(len/2)*sqrt(3), c[1]-(len/2))
        pos3 = (c[0]+(len/2)*sqrt(3), c[1]+(len/2))
        pygame.draw.line(surface,(0,0,255),pos1,pos2,4)
        pygame.draw.line(surface,(0,0,255),pos2,pos3,4)

def make_move(surface,m,len,color): #m for move (is a string); len is cell len;
    c = ord(m[0])-ord("A")   #for column
    r = int(m[1:])-1   #for row
    pos = board[r][c]
    #pygame.draw.circle(surface,color,pos,len*(sqrt(3)/2)*(8/10))
    draw_cell(surface,pos,len,color,(0,0,0))
    if c==0 or c==board_size-1 or r==0 or r==board_size-1:
        draw_edge(surface,len)

def board_setup():
    #window setting
    pygame.init()
    window_surface = pygame.display.set_mode((800,480))
    pygame.display.set_caption("Visual Hex")
    window_surface.fill((127,127,127))

    #board setting
    build_board((30,40),length)
    for i in range(board_size):
        for j in range(board_size):
            draw_cell(window_surface,board[i][j],length,(255,255,255),(0,0,0))

    draw_edge(window_surface,length)

    #button setting
    next_area = (600,10,150,70) #tuple (left,top,width,height)
    creat_button(window_surface,next_area,(0,0,255),"next",(255,255,255),100)
    last_area = (600,100,150,70)
    creat_button(window_surface,last_area,(255,0,0),"last",(255,255,255),100)
    pause_play_area = (600,190,180,38)
    creat_button(window_surface,pause_play_area,(255,255,255),"pause/play",(0,0,0),50)

    pygame.display.update()
    return window_surface

def user_command_process_after_pause():
    global _now
    global cur_player
    global buttons
    while 1:
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                mx, my = event.pos
                if pygame.Rect.collidepoint(buttons["next"],(mx,my)):
                    #next move
                    if _now > len(movelist)-1: pass
                    else:
                        cur_player = cur_player*-1
                        make_move(window_surface, movelist[_now], length,color[cur_player])
                        _now += 1
                    
                elif pygame.Rect.collidepoint(buttons["last"],(mx,my)):
                    if _now <= 0: pass
                    else:
                        _now -= 1
                        cur_player = cur_player*-1
                        c = ord(movelist[_now][0])-ord("A")   #for column
                        r = int(movelist[_now][1:])-1   #for row
                        draw_cell(window_surface,board[r][c],length,(255,255,255),(0,0,0))
                        if c==0 or c==board_size-1 or r==0 or r==board_size-1:
                            draw_edge(window_surface,length)

                elif pygame.Rect.collidepoint(buttons["pause/play"],(mx,my)):
                    #paused, waiting to play
                    return

            pygame.display.update()

def user_command_process():
    global _now
    global cur_player
    global buttons

    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()
        if event.type == pygame.MOUSEBUTTONDOWN:
            mx, my = event.pos
            if pygame.Rect.collidepoint(buttons["next"],(mx,my)):
                #next move
                if _now > len(movelist)-1: pass
                else:
                    cur_player = cur_player*-1
                    make_move(window_surface, movelist[_now], length,color[cur_player])
                    _now += 1
                
            elif pygame.Rect.collidepoint(buttons["last"],(mx,my)):
                if _now <= 0: pass
                else:
                    _now -= 1
                    cur_player = cur_player*-1
                    c = ord(movelist[_now][0])-ord("A")   #for column
                    r = int(movelist[_now][1:])-1   #for row
                    draw_cell(window_surface,board[r][c],length,(255,255,255),(0,0,0))
                    if c==0 or c==board_size-1 or r==0 or r==board_size-1:
                            draw_edge(window_surface,length)

            elif pygame.Rect.collidepoint(buttons["pause/play"],(mx,my)):
                #pause, waiting to play
                user_command_process_after_pause()

        pygame.display.update()

def write_board(f, movelist): #write to controler_txt
    print(movelist)
    f.write("<board> ")
    for m in movelist:
        f.write(m)
        f.write(" ")
    f.write("</board>\n")

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
        user_command_process()
        txt = f.readline().split()
        try:
            if txt[-1]=="</solve>": break
        except: pass
    return txt[len(txt)-2]

def play(window_surface):
    global _now
    global cur_player

    cur_player = 1
    #play
    while 1:
        f = open(controler_txt_path, 'w+')
        write_set(f, cur_player)
        write_board(f, movelist)
        f.close()

        if cur_player == 1:
            #os.system("start "+first_hand_solver)
            os.system(first_hand_solver)
        else:
            #os.system("start "+second_hand_solver)
            os.system(second_hand_solver)

        f = open(controler_txt_path, 'r')
        move = get_solve(f)
        f.close()

        if move == "End":
            _now = _now*-1
            break
        else:
            movelist.append(move)
            make_move(window_surface,move,length,color[cur_player])
            _now = _now+1
            cur_player = cur_player*(-1)
            pygame.display.update()

        

#start
window_surface = board_setup()
play(window_surface)
while 1:
    user_command_process()
