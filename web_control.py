from selenium import webdriver
import os
import time
import datetime
import math
#<path>===============================================
data_path = "ExperimentRecord\\data.txt"
driver_path = "D:\\chromedriver_win32_chrome111\\chromedriver.exe"
solver_path = "MCTS-AMAF-SaveBridge.exe"
controler_txt_path = "dataForControl.txt"
#</path>===================================================
#<get to the site>====================================
driver = webdriver.Chrome(driver_path)
url = "http://www.lutanho.net/play/hex.html"
driver.get(url)
driver.set_window_position(0,0)
#</get to the site>===================================================
#<set up>=========================================================================
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
message = driver.find_element_by_css_selector("input[name='Msg']") #message text box
get_movelist = driver.find_element_by_css_selector("input[onclick='javascript:GetMoveList()']") #"get" botton
move_list = driver.find_element_by_css_selector("textarea[name='MoveList']") #move list text box
set_blue_level = driver.find_elements_by_css_selector("input[name='BlueLevel']")
set_red_level = driver.find_elements_by_css_selector("input[name='RedLevel']")
new_game = driver.find_element_by_css_selector("input[onclick='javascript:Init()']")
red_player = driver.find_element_by_css_selector("input[value='Red: Player']")
red_computer = driver.find_element_by_css_selector("input[value='Red: Computer']")
blue_player = driver.find_element_by_css_selector("input[value='Blue: Player']")
blue_computer = driver.find_element_by_css_selector("input[value='Blue: Computer']")
#</set up>===============================================================================================

#Start
class game: #dataset and resault for mcts hex solver experiment
    def __init__(self,level,local,explore_weight,simulate_times):
        self.level = level #online computer's level
        self.local = local #local player's color ("Red" or "Blue")
        self.explore_weight = explore_weight #explore_weight in UCT formula
        self.simulate_times = simulate_times #simulate_times in MCTS
        self.total_time = 0
        self.total_move = 0
        self.win = 0 #winner = 1(win) or 0(lose)
        self.moves = ""
    def start(self): self.total_time = time.time()
    def end(self): self.total_time = time.time() - self.total_time

def play(experiment):    #c is the explore weight in UCT formula;
                               #simulate_times is how many times the MCTS(solver) simulate;
    #<setup>--------------------------
    level = experiment.level
    local = experiment.local
    _c = experiment.explore_weight
    simulate_times = experiment.simulate_times

    local_player = local
    game_over = 0

    if local_player == "Red":
        set_blue_level[level-1].click()
        red_player.click()
        blue_computer.click()
    else:
        set_red_level[level-1].click()
        red_computer.click()
        blue_player.click()
    #</setup>-------------------------------
    #<play>---------------------------
    while 1: #wait untill the board is refresh successfully
        new_game.click()
        txt1 = message.get_attribute("value").split()
        if txt1[-1] != "!": break #the board is refreshed
        else: pass
    experiment.start()
    while game_over==0:
        while 1:
            txt1 = message.get_attribute("value").split()
            if (local_player in txt1) and (txt1[-1]=="move."): break #our turn
            elif txt1[-1]=="!":                                  #game over
                game_over = 1
                break

        if game_over==1: break

        get_movelist.click()    #refresh the move list
        txt = move_list.get_attribute("value")  #movelist (str)

        f = open(controler_txt_path,'w+')#write the board state and setting to .txt
        f.write("<set> "), f.write(str(_c)), f.write(" "), f.write(str(simulate_times)), f.write(" </set> \n")
        f.write("<board> "), f.write(txt), f.write(" </board> \n")
        f.close()   #close file to save it

        os.system(solver_path) #call the solver to solve this board
        #<read the solution>----------------------------------------------------
        f = open(controler_txt_path,'r')
        while 1:
            txt = f.readline().split()
            try:
                if txt[-1]=="</solve>": break
            except: pass
        f.close()
        #</read the solution>----------------------------------------------------------------------
        #<make move>----------------
        move = txt[len(txt)-2]
        r = int(move[1:])-1
        c = ord(move[0])-ord("A")
        board[r][c].click()
        #</make move>-------------------------
    #</play>-------------------------------
    experiment.end() #end time mark
    f = open(controler_txt_path,'w+')
    f.write("<board> end </board>")
    f.close()   #close file to save it
    if txt1[0]==local: experiment.win = 1
    else: experiment.win = 0
    get_movelist.click()    #refresh the move list
    experiment.moves = move_list.get_attribute("value")
    experiment.total_move = len(experiment.moves.split())

def write_set(f,data):
    #<record>
    #<set>
    f.write("<set> "), f.write("expolre_weight="), f.write(str(data.explore_weight))
    f.write(" simulate_times="), f.write(str(data.simulate_times))
    f.write(" go_first=")
    if data.local == "Red": f.write("local")
    else:                   f.write("online")
    f.write(" level="), f.write(str(data.level))
    f.write(" </set>\n")
    #</set>

def write_result(f,data):
    #<move>
    f.write("<move> "), f.write(data.moves), f.write(" </move>\n")
    #</move>
    #<resault>
    f.write("<resault> "), f.write("total_move="), f.write(str(data.total_move))
    f.write(" total_time="), f.write(str(data.total_time))
    f.write(" win="), f.write(str(data.win)), f.write(" </resault>\n")
    #</resault>

#start

N = 1
for i in range(1):
    
    record = open(data_path,'a')
    record.write("<experiment N0=0 "), record.write(str(datetime.date.today())), record.write(">\n")

    test = game(3,"Red",1,60000)
    write_set(record,test)
    record.close()

    for j in range(N):
        play(test)

        print(message.get_attribute("value"))
        print(test.total_time, test.total_move, test.win)
        
        record = open(data_path,'a')
        write_result(record,test)
        record.close()

    record = open(data_path,'a')
    record.write("</experiment>\n")
    record.close()

driver.close()
