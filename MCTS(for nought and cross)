#include<iostream>
#include<vector>
#include<cmath>
#include<cstring>
#include<utility>
#include<ctime>
#include<climits>
using namespace std;

#define c sqrt(2)

//Monte Carlo Tree Search - take "noughts and crosses" for example;

int random(int n){  //produce random number from 0 to n-1;
    srand(time(NULL));
    return rand()%n;
}

struct game{
    int board[3][3];    //record the state;
    vector<pair<int,int>> pm;   //possible move. if you want to get possible move, call gpm() first;

    void init(){
        memset(board,0,sizeof(board));
        pm.clear();
    }
    void gpm(){     //get possible move
        pm.clear();
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(board[i][j]==0) pm.push_back({i,j});
            }
        }
    }
    void Move(pair<int,int> pos,int current){board[pos.first][pos.second] = current;}
    int WL(){   //check winer
        int i;
        for(i=0;i<3;i++){
            if(board[i][0]==board[i][1] && board[i][1]==board[i][2] && board[i][0]) return board[i][0];
        }
        for(i=0;i<3;i++){
            if(board[0][i]==board[1][i] && board[1][i]==board[2][i] && board[0][i]) return board[0][i];
        }
        if(board[0][0]==board[1][1] && board[1][1]==board[2][2] && board[0][0]) return board[0][0];
        if(board[0][2]==board[1][1] && board[1][1]==board[2][0] && board[0][2]) return board[0][2];
        return 0;
    }
    void print(){
        for(int i=0;i<3;i++){
            cout << i << " ";
            for(int j=0;j<3;j++){
                if(board[i][j]==1) cout << "O";
                else if(board[i][j]==-1) cout << "X";
                else cout << "-";
                cout << " ";
            }
            cout << "\n";
        }
        cout << "# 0 1 2\n";
    }
    bool End(){
        if(WL()!=0) return 1;
        gpm();
        if(!pm.size()) return 1;
        return 0;
    }
    void operator=(game g){for(int i=0;i<3;i++) for(int j=0;j<3;j++) board[i][j] = g.board[i][j];}
};

struct TreeNode{
    TreeNode* parent=NULL;
    vector<TreeNode*> children;
    game state;
    int n=0;    //visited times
    int w=0;    //win times
    int cp;     //current player, player to make next move in this state;

    void init(){
        parent = NULL;
        children.clear();
        state.init();
        n = 0;w = 0;cp = 1;
    }

    float UCT(int N){               //according to "https://liaowc.github.io/blog/mcts-monte-carlo-tree-search/";
        if(n==0) return INT_MAX;    //N = total visits;
        else return (float)w/n + c*(sqrt(log(N)/n));
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
        state.gpm();

        for(pair<int,int> p : state.pm){
            TreeNode* tn = new TreeNode;
            tn->parent = this;
            tn->cp = cp*-1;
            tn->state = state;
            tn->state.Move(p,cp);
            children.push_back(tn);
        }
    }
    void update(int leaf_value){n++; if(leaf_value==cp*-1) w++; else w--;}  //cp*-1 == player who make this move;
    void update_recursive(int leaf_value){
        if(parent!=NULL) parent->update_recursive(leaf_value);
        update(leaf_value);
    }
    void rollout(){
        TreeNode* node = this;
        pair<int,int> m;
        while(1){
            //cout << "---------------\n";
            //node->state.print();
            //cout << "---------------";
            if(node->state.End()){
                //cout << "\n";
                update_recursive(node->state.WL());
                return;
            }
            node->state.gpm();
            //cout << "pm = " << node->state.pm.size() << "\n";
            m = node->state.pm[random(node->state.pm.size())];//randomly chose next move;
            TreeNode* tn = new TreeNode;
            tn->parent = node;
            tn->cp = node->cp*-1;
            tn->state = node->state;
            tn->state.Move(m,node->cp);
            children.push_back(tn);
            node = tn;
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
        //cout << "======================rollout " << i << " node.children = " << node_0->children.size() << " node.n = " << node_0->n << "\n";
        tn = node_0;

        //select
        while(!tn->is_leave()){/*cout << " select ";*/tn = tn->select(i);}

        //expand, rollout
        if(tn->n==0){
            tn->rollout();
            if(!tn->state.End()){
                Delete(tn->children[0]);
                tn->children.clear();
            }
        }
        else{
            tn->expand();
            if(tn->children.size()){
                tn = tn->children[0];
                tn->rollout();
                if(!tn->state.End()){
                    Delete(tn->children[0]);
                    tn->children.clear();
                }
            }
            else{   //game end;
                tn->update_recursive(tn->state.WL());
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

//human player = 1;
//computer player = -1;
int main(){
    int i,j,n;
    TreeNode* now = new TreeNode;
    TreeNode* tmp;
    now->init();

    //cout << "chose strength of computer(10~100) : ";
    //cin >> n;
    while(!now->state.End()){

        //clear
        if(now->parent!=NULL) delete now->parent;
        now->parent = NULL;

        for(TreeNode* tn : now->children) Delete(tn);
        now->children.clear();

        now->n = 0; now->w = 0; now->cp = -1;

        //computer's turn
        cout << "================\n";
        tmp = now;
        now = MCTS(now,50);
        now->state.print();
        for(TreeNode* tn : tmp->children){

            if(tn==now) continue;
            else Delete(tn);
        }

        if(now->state.End()) break;
        cout << "----------------\n";
        //human player's turn
        cout << "row column : ";
        cin >> i >> j;
        system("PAUSE");
        now->state.Move({i,j},1);
        now->state.print();
        cout << "================\n";
    }
    cout << "player " << now->state.WL() << " win";

    Delete(now);

    return 0;
}
