#include<iostream>
#include<vector>
#include<cmath>
#include<cstring>
#include<utility>
#include<ctime>
#include<climits>
#include<string>
#include<sstream>
#include<fstream>
using namespace std;

double C=sqrt(2);
#define Size 11 //this is eleven!!!!!!!!!!
stringstream ss;
//Monte Carlo Tree Search - Hex solver;

int random(int n){  //produce random number from 0 to n-1;
    return rand()%n;
}
string cr2str(pair<int,int> p){ //(char column, int row) to (int column, int row);
    ss.clear();
    string str = "",s;
    str += p.first-1+'A';
    ss << p.second; ss >> s;
    str += s;
    return str;
}
pair<int,int> str2cr(string s){
    int c = s[0]+1-'A';
    int r;
    ss.clear();
    ss << s.substr(1); ss >> r;
    return {c,r};
}
struct game{    //a hex game;
    int board[Size+2][Size+2];      //record the state;
    int search_mask[Size+2][Size+2];//for dfs() to use; 1 == ok to go, 0 = searched;
    vector<pair<int,int>> pm;       //possible move. if you want to get possible move, call gpm() first;
    int total_move = 0;             //check if swap rule is still available;

    void init(){
        memset(board,0,sizeof(board));
        memset(search_mask,1,sizeof(search_mask));
        for(int i=0;i<Size+2;i++) board[0][i] = 1, board[Size+1][i] = 1, board[i][0] = -1, board[i][Size+1] = -1;
        pm.clear();
    }
    void gpm(){     //get possible move
        pm.clear();
        for(int i=1;i<=Size;i++){
            for(int j=1;j<=Size;j++){
                if(total_move==1) pm.push_back({j,i});
                else{
                    if(board[i][j]==0) pm.push_back({j,i});//{column,row}
                }
            }
        }
    }
    void Move(pair<int,int> pos,int current){
        if(board[pos.second][pos.first]!=0){
            board[pos.second][pos.first] = 0;
            board[pos.first][pos.second] = current;
        }
        else board[pos.second][pos.first] = current;
        total_move++;
    }
    int dfs(pair<int,int> pos,int player){
        //cout << "r = " << pos.second << " c = " << pos.first << "\n";
        if(player==1 && pos.second==Size) return 1;  //player 1 need to connect to the floor to win
        else if(player==-1 && pos.first==Size) return 1;  //player -1 need to connect to the right to win
        else{
            int r = pos.second;
            int c = pos.first;
            search_mask[r][c] = 0;
            if(r-1>0 && board[r-1][c]==player && search_mask[r-1][c])                  if(dfs({c,r-1},player))   return 1;//UP
            if(r-1>0 && c+1<=Size && board[r-1][c+1]==player && search_mask[r-1][c+1]) if(dfs({c+1,r-1},player)) return 1;//UP RIGHT
            if(c-1>0 && board[r][c-1]==player && search_mask[r][c-1])                  if(dfs({c-1,r},player))   return 1;//LEFT
            if(c+1<=Size && board[r][c+1]==player && search_mask[r][c+1])              if(dfs({c+1,r},player))   return 1;//RIGHT
            if(r+1<=Size && board[r+1][c]==player && search_mask[r+1][c])              if(dfs({c,r+1},player))   return 1;//DOWN
            if(r+1<=Size && c-1>0 && board[r+1][c-1]==player && search_mask[r+1][c-1]) if(dfs({c-1,r+1},player)) return 1;//DOWN LEFT
            return 0;
        }
    }
    int WL(){   //check winer
        for(int i=1;i<=Size;i++){
            memset(search_mask,1,sizeof(search_mask));
            if(dfs({i,0},1)) return 1;
            memset(search_mask,1,sizeof(search_mask));
            if(dfs({0,i},-1)) return -1;
        }
        return 0;
    }
    void print(){
        for(int i=0;i<Size+2;i++){
            if(i==0 || i==Size+1) cout << "#";
            else cout << i;
            if(i==Size+1 && i>9) cout << " ";

            if(i<=9) cout << " ";

            for(int j=0;j<=i;j++) cout << " ";

            for(int j=0;j<Size+2;j++){
                if(board[i][j]==1) cout << "O";
                else if(board[i][j]==-1) cout << "X";
                else cout << "-";
                cout << " ";
            }
            cout << "\n";
        }
        for(int i=0;i<Size+5;i++) cout << " ";
        for(int i=0;i<Size+2;i++){
            if(i==0) cout << "# ";
            else if(i==Size+1) cout << "#";
            else cout << (char)(i-1+'A') << " ";
        }
        cout << "\n";
    }
    bool End(){
        if(WL()!=0) return 1;
        gpm();
        if(!pm.size()) return 1;
        return 0;
    }
    void operator=(game g){
        for(int i=0;i<=Size+1;i++) for(int j=0;j<=Size+1;j++) board[i][j] = g.board[i][j];
        total_move = g.total_move;
    }
};

struct TreeNode{
    TreeNode* parent=NULL;
    vector<TreeNode*> children;
    game* state = new game;
    pair<int,int> this_move;
    int n=0;    //visited times
    int w=0;    //win times
    int cp;     //current player, player to make next move in this state;

    void init(){
        parent = NULL;
        children.clear();
        state->init();
        n = 0;w = 0;cp = 1;
    }

    double UCT(int N){               //according to "https://liaowc.github.io/blog/mcts-monte-carlo-tree-search/";
        if(n==0) return INT_MAX;    //N = total visits;
        else {
            return (double)w/n + C*(sqrt(log(N)/n));
        }
    }
    TreeNode* select(int N){
        //cout << "select\n";
        TreeNode* tn=children[0];
        for(int i=0;i<children.size();i++){
            //cout << "child" << i << ":w = " << children[i]->w << " n = " << children[i]->n << " UCT = " << children[i]->UCT(N) << "\n";
            if(children[i]->UCT(N) > tn->UCT(N)) tn = children[i];
        }
        return tn;
    }
    void expand(){
        //cout << " expand ";
        state->gpm();

        for(pair<int,int> p : state->pm){   // p for pair
            //system("PAUSE");
            //cout << "this move" << p.first << " " << p.second << "\n";
            TreeNode* tn = new TreeNode;
            tn->parent = this;
            tn->cp = cp*-1;
            *(tn->state) = *state;
            tn->state->Move(p,cp);
            tn->this_move = p;

            //tn->state->print();

            children.push_back(tn);
        }
        if(is_leave()) return;
        else{
            delete state;
            state = NULL;
        }
        return;
    }
    void update(int leaf_value){n++; if(leaf_value==cp*-1) w++; else w--;}  //cp*-1 == player who make this move;

    void update_recursive(int leaf_value){
        if(parent!=NULL) parent->update_recursive(leaf_value);
        update(leaf_value);
    }
    void rollout(){
        game cur;
        pair<int,int> m;
        cur = *(this->state);
        int p = cp; //player
        while(1){
            //cout << "---------------\n";
            //cur.print();
            //cout << "---------------";
            if(cur.End()){
                //cout << "\n";
                update_recursive(cur.WL());
                return;
            }
            cur.gpm();
            //cout << "pm = " << cur.pm.size() << " total_move = " << cur.total_move << "\n";
            m = cur.pm[random(cur.pm.size())];//randomly chose next move;
            p *= -1;
            cur.Move(m,p);
        }
    }
    bool is_leave(){return !children.size();}
    bool is_root(){return parent==NULL;}
};

void Delete(TreeNode* node){
    for(TreeNode* tn : node->children) Delete(tn);
    delete node;
}

TreeNode* MCTS(TreeNode* node_0,int simulation_times){  //select -> expand -> rollout -> backpropagation
    TreeNode* tn;
    for(int i=0;i<simulation_times;i++){
        //cout << "\r" << i;
        //system("PAUSE");
        //if(i>30000) cout << "======================rollout " << i << " node.children = " << node_0->children.size() << " node.n = " << node_0->n << "\n";
        cout << "\r" << i;

        tn = node_0;

        //select
        while(!tn->is_leave()){/*cout << " select "*/;tn = tn->select(i);}

        //expand, rollout
        if(tn->n==0){
            tn->rollout();
        }
        else{
            //cout << "expand ";
            tn->expand();
            if(tn->children.size()){
                //cout << "case:1\n";
                tn = tn->children[0];
                tn->rollout();
            }
            else{   //game end;
                //cout << "case:2\n";
                tn->update_recursive(tn->state->WL());
            }
        }
    }
    tn = node_0->children[0];
    for(int i=0;i<node_0->children.size();i++){
        //cout << "chose" << i << "n = " << node_0->children[i]->n << " w = " << node_0->children[i]->w << "\n";
        if(tn->n < node_0->children[i]->n) tn = node_0->children[i];
    }

    return tn;
}
void self_play();
void human_and_computer();
string hex_solver(vector<string> in_moves,int simulation_times);

//================================================main
//player 1(red) go first                            //
int main(){                                         //
    srand(time(NULL));
    ios::sync_with_stdio(0);
    cout.tie(0);

    string controler_txt_path = "D:\\desktop\\experiment_package\\dataForControl.txt";

    ifstream f_in(controler_txt_path);
    ofstream f_out(controler_txt_path, ios::app);
    vector<string> v;
    string str;
    int s; //s for simulation times
    //read file

    f_in >> str >> C >> s >> str;
    cout << "C = " << C << " s = " << s << "\n";
    f_in >> str;  //data start
    while(f_in>>str){
        if(str=="end") return 0;    //game has end
        if(str=="</board>") break;  //data end
        else v.push_back(str);
    }
    //write the answer
    f_out << "<solve> " << hex_solver(v,s) << " </solve> ";

    //close file
    f_out.close();
    f_in.close();

                                                    //
    return 0;                                       //
}                                                   //
//====================================================

void self_play(){
    int i,j,n;
    TreeNode* now = new TreeNode;
    TreeNode* tmp;
    now->init();
    while(!now->state->End()){

        //clear
        if(now->parent!=NULL) delete now->parent;
        now->parent = NULL;

        for(TreeNode* tn : now->children) Delete(tn);
        now->children.clear();

        now->n = 0; now->w = 0;

        //computer's turn
        cout << "================\n";
        tmp = now;
        now = MCTS(now,1000);
        cout << "this move " << now->this_move.first << " " << now->this_move.second << "\n";
        now->state->print();
        for(TreeNode* tn : tmp->children){

            if(tn==now) continue;
            else Delete(tn);
        }
        cout << "================\n";
    }
    cout << "player " << now->state->WL() << " win";

    Delete(now);
    system("PAUSE");
}
void human_and_computer(){
    int i,j,n;
    TreeNode* now = new TreeNode;
    TreeNode* tmp;
    now->init();
    while(!now->state->End()){

        //clear
        if(now->parent!=NULL) delete now->parent;
        now->parent = NULL;

        for(TreeNode* tn : now->children) Delete(tn);
        now->children.clear();

        now->n = 0; now->w = 0; now->cp = -1;

        //computer's turn
        cout << "================\n";
        tmp = now;
        now = MCTS(now,1000);
        now->state->print();
        for(TreeNode* tn : tmp->children){

            if(tn==now) continue;
            else Delete(tn);
        }

        if(now->state->End()) break;
        cout << "----------------\n";
        //human player's turn
        cout << "row column : ";
        cin >> i >> j;

        now->state->Move({i,j},1);
        now->state->print();
        cout << "================\n";
    }
    cout << "player " << now->state->WL() << " win";

    Delete(now);
}

string hex_solver(vector<string> in_moves,int simulation_times){ //input the previous moves, this function will give you the best next move

    int i,j,player=1;
    TreeNode* now = new TreeNode;
    string res;
    //set up

    now->init();
    if(in_moves.size()) now->state->total_move = 2;
    for(i=0;i<in_moves.size();i++){
        now->state->Move(str2cr(in_moves[i]),player);
        player*=-1;
    }
    now->cp = player;
    //now->state.print();
    //solve
    if(now->state->End()) return "End";

    res = cr2str(MCTS(now,simulation_times)->this_move);

    //output
    cout << res << "\n";

    Delete(now);

    return res;
}
