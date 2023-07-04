import pygame
from pygame.locals import QUIT
import sys
from math import sqrt

board_size = 11
board = [[] for i in range(board_size)]

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

def move(surface,m,len,color): #m for move (is a string); len is cell len;
    c = ord(m[0])-ord("A")   #for column
    r = int(m[1:])-1   #for row
    pos = board[r][c]
    #pygame.draw.circle(surface,color,pos,len*(sqrt(3)/2)*(8/10))
    draw_cell(surface,pos,len,color,(0,0,0))
    if c==0 or c==board_size-1 or r==0 or r==board_size-1:
        draw_edge(surface,len)

def creat_button(surface,area,color1,text,color2,size): #area is a tuple (left,top,width,height)
    #button icon
    button_surface = pygame.Surface((area[2],area[3]))
    button_surface.fill(color1)
    surface.blit(button_surface,(area[0],area[1]))
    #button text
    head_font = pygame.font.SysFont(None,size)
    txt_surface = head_font.render(text,True,color2)
    surface.blit(txt_surface,(area[0],area[1]))

    return pygame.Rect(area[0],area[1],area[2],area[3])

def main():
    length = 25
    color = [(0,0,0),(255,0,0),(0,0,255)] #[non,red,blue]
    now_color = -1

    print("MOVES : ", end="")
    moves = input().split()
    _now = 0
    #window setting
    pygame.init()
    window_surface = pygame.display.set_mode((800,480))
    pygame.display.set_caption("Visual Hex")
    window_surface.fill((127,127,127))

    next_area = (600,10,150,70) #tuple (left,top,width,height)
    next_button = creat_button(window_surface,next_area,(0,0,255),"next",(255,255,255),100)
    last_area = (600,100,150,70)
    last_button = creat_button(window_surface,last_area,(255,0,0),"last",(255,255,255),100)

    #board setting
    build_board((30,40),length)
    for i in range(board_size):
        for j in range(board_size):
            draw_cell(window_surface,board[i][j],length,(255,255,255),(0,0,0))
    
    draw_edge(window_surface,length)

    pygame.display.update()

    #start
    while True:
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                mx, my = event.pos
                if pygame.Rect.collidepoint(next_button,(mx,my)):
                    #next move
                    if _now > len(moves)-1: pass
                    else:
                        now_color = now_color*-1
                        move(window_surface, moves[_now], length,color[now_color])

                        _now += 1

                elif pygame.Rect.collidepoint(last_button,(mx,my)):
                    if _now == 0: pass
                    else:
                        _now -= 1
                        now_color = now_color*-1
                        c = ord(moves[_now][0])-ord("A")   #for column
                        r = int(moves[_now][1:])-1   #for row
                        draw_cell(window_surface,board[r][c],length,(255,255,255),(0,0,0))
                        if c==0 or c==board_size-1 or r==0 or r==board_size-1:
                            draw_edge(window_surface,length)

        pygame.display.update()

main()