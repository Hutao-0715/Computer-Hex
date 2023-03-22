#include<iostream>
#include<vector>
#include<queue>
#include<cmath>
#include<cstring>
#include<utility>
#include<ctime>
#include<climits>
#include<string>
#include<sstream>
#include<fstream>
#include<ctime>
using namespace std;

double C=1;
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
pair<int,int> p_plused(pair<int,int> a, pair<int,int> b){    //plused for pair<int,int>
    return {a.first+b.first, a.second+b.second};
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
    pair<int,int> save_bridge(pair<int,int> cur,int next_player){

        pair<int,int> dir[6] = {{-1,0},{0,-1},{1,-1},{1,0},{0,1},{-1,1}};
        vector<pair<int,int>> save;    //the place that have to put a chess to save bridge;
        int player = next_player;

        //recognize the threatened bridge(s);
        for(int i=0;i<6;i++){
            pair<int,int> pos1 = p_plused(cur,dir[i]);
            pair<int,int> pos2 = p_plused(cur,dir[(i+2)%6]);
            pair<int,int> mid = p_plused(cur,dir[(i+1)%6]); //middle of the bridge
            if(board[pos1.second][pos1.first]==player && board[pos2.second][pos2.first]==player && board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
        }

        //chose a bridge to save
        if(save.size()) return save[random(save.size())];
        else return {0,0};
    }
    bool InRange(int x,int y){
        if(0<=x && x<=Size+1 && 0 <= y && y <= Size+1){
            return 1;
        }
        return 0;
    }
    int dfs(int player){
        memset( search_mask,0,sizeof(search_mask) );
        deque<pair<int,int>> dq;
        int dx[6]={-1,-1,0,0,1,1};
        int dy[6]={0,1,-1,1,-1,0};

        dq.push_back({0,0});

        while( !dq.empty() ){
            pair<int,int> now=dq.back();
            dq.pop_back();

            if(player==1 and now.first==Size) return 1;
            else if(player==-1 and now.second==Size) return 1;

            for(int i=0;i<6;i++){
                int nxt_x=now.first+dx[i];
                int nxt_y=now.second+dy[i];
                if( InRange(nxt_x,nxt_y) ){

                    if( board[nxt_x][nxt_y]==player and !search_mask[nxt_x][nxt_y] ){
                        dq.push_back({nxt_x,nxt_y});
                        search_mask[nxt_x][nxt_y]=1;
                    }
                }
            }
        }
        return 0;
    }
    int WL(){   //check winer
        for(int i=1;i<=Size;i++){
            if(dfs(1)) return 1;
            if(dfs(-1)) return -1;
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
    TreeNode* AMAF_parent=NULL;
    vector<TreeNode*> children;
    game* state = new game;
    pair<int,int> this_move;
    int n=0;    //visited times
    int w=0;    //win times
    int cp;     //current player, player to make next move in this state;

    bool is_leave(){return !children.size();}
    bool is_root(){return parent==NULL;}

    void init(){
        parent = NULL;
        children.clear();
        state->init();
        n = 0;w = 0;cp = 1;
    }

    pair<int,int> save_bridge(pair<int,int> cur,int next_player){
        if(cur.first==0 && cur.second==0) return {0,0}; //skip

        pair<int,int> dir[6] = {{-1,0},{0,-1},{1,-1},{1,0},{0,1},{-1,1}};
        vector<pair<int,int>> save;    //the place that have to put a chess to save bridge;
        int player = next_player;

        //recognize the threatened bridge(s);
        for(int i=0;i<6;i++){
            pair<int,int> pos1 = p_plused(cur,dir[i]);
            pair<int,int> pos2 = p_plused(cur,dir[(i+2)%6]);
            pair<int,int> mid = p_plused(cur,dir[(i+1)%6]); //middle of the bridge
            if(state->board[pos1.second][pos1.first]==player && state->board[pos2.second][pos2.first]==player && state->board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
        }

        //chose a bridge to save
        if(save.size()) return save[random(save.size())];
        else return {0,0};
    }

    double UCT(int N){         //according to "https://liaowc.github.io/blog/mcts-monte-carlo-tree-search/";
        if(n==0) return INT_MAX;    //N = total visits;
        else {
            int c = this_move.first;
            int r = this_move.second;

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

        //save bridge
        pair<int,int> save = save_bridge(this_move, cp);
        if(save.first != 0 && save.second != 0){
            TreeNode* tn = new TreeNode;
            tn->parent = this;
            tn->cp = cp*-1;
            *(tn->state) = *state;
            tn->state->Move(save,cp);
            tn->this_move = save;

            children.push_back(tn);
        }
        else{
            //expand legal moves
            state->gpm();
            for(pair<int,int> p : state->pm){   // p for pair
                TreeNode* tn = new TreeNode;
                tn->parent = this;
                tn->cp = cp*-1;
                *(tn->state) = *state;
                tn->state->Move(p,cp);
                tn->this_move = p;

                children.push_back(tn);
            }
        }

        //delete state
        if(is_leave()) return;
        else{
            delete state;
            state = NULL;
        }
        return;
    }

    void update(int leaf_value){n++; if(leaf_value==cp*-1) w++;}

    void AMAF_update(bool win){
        if(is_root()) return;
        if(AMAF_parent == NULL){    //haven't found AMAF_parent
            //look for AMAF_parent
            TreeNode* cur = this->parent;
            while(cur->parent != NULL && AMAF_parent == NULL){

                cur = cur->parent;
                for(TreeNode* tn : cur->children){
                    if(tn->this_move == this_move){
                        AMAF_parent = tn;
                        break;
                    }
                }
            }

            if(AMAF_parent != NULL){
                AMAF_parent->AMAF_update(win);
                return;
            }
        }
        else{   //have found AMAF_parent
            AMAF_parent->AMAF_update(win);
            return;
        }
    }

    void update_recursive(int leaf_value){
        if(parent!=NULL) parent->update_recursive(leaf_value);
        update(leaf_value);
        AMAF_update(leaf_value == cp*-1);
    }

    void rollout(){
        game cur;
        pair<int,int> m,save;
        cur = *(this->state);
        int p = cp; //player
        cur.gpm();

        while(1){
            //choose next move
            if(!cur.pm.size()) break; //board is filled

            //board is not filled
            //save bridge
            save = cur.save_bridge(m, p*-1);
            if(save.first != 0 && save.second != 0){
                m = save;
                for(int i=0;i<cur.pm.size();i++){   //delete m from possible move list
                    if(cur.pm[i] == m){
                        cur.pm.erase(cur.pm.begin()+i);
                        break;
                    }
                }
            }
            else{
                int idx = random(cur.pm.size());
                m = cur.pm[idx];//randomly chose next move;
                cur.pm.erase(cur.pm.begin()+idx);   //delete m from possible move list
            }

            p *= -1;
            cur.Move(m,p);
        }
        //cout << "\n";
        update_recursive(cur.WL());

        return;
    }
};

void Delete(TreeNode* node){
    for(TreeNode* tn : node->children) Delete(tn);
    delete node;
}

TreeNode* MCTS(TreeNode* node_0,int simulation_times){  //select -> expand -> rollout -> backpropagation
    TreeNode* tn;
    for(int i=0;i<simulation_times;i++){
        //system("PAUSE");
        //if(i>30000) cout << "======================rollout " << i << " node.children = " << node_0->children.size() << " node.n = " << node_0->n << "\n";
        cout << "\r" << i;

        tn = node_0;

        //select
        while(!tn->is_leave()) tn = tn->select(i);

        //expand, rollout
        if(tn->n==0){
            //cout << "rollout\n";
            tn->rollout();
        }
        else{
            //cout << "expand\n";
            tn->expand();
            if(tn->children.size()){
                tn = tn->children[0];
                tn->rollout();
            }
            else{   //game end;
                tn->update_recursive(tn->state->WL());
            }
        }
    }

    tn = node_0->children[0];
    for(int i=0;i<node_0->children.size();i++){
        if(tn->n < node_0->children[i]->n) tn = node_0->children[i];
    }
    return tn;
}
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

string hex_solver(vector<string> in_moves,int simulation_times){ //input the previous moves, this function will give you the best next move

    int i,j,player=1;
    TreeNode* now = new TreeNode;
    string res;

    //set up
    now->init();
    now->state->total_move = in_moves.size();
    for(i=0;i<in_moves.size();i++){
        now->state->Move(str2cr(in_moves[i]),player);
        player*=-1;
    }
    now->cp = player;
    if(in_moves.size()) now->this_move = str2cr(in_moves[in_moves.size()-1]);
    else now->this_move = {0,0};
    now->n = 1;

    //save bridge
    pair<int,int> save_b = now->save_bridge(now->this_move, player);
    if(save_b.first != 0 && save_b.second != 0){
        cout << cr2str(save_b) << "\n";
        return cr2str(save_b);
    }

    //now->state.print();
    //solve
    if(now->state->End()) return "End";

    res = cr2str(MCTS(now,simulation_times)->this_move);

    //output
    cout << res << "\n";

    Delete(now);

    return res;
}

