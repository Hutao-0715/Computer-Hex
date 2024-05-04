#include<iostream>
#include<fstream>
#include<vector>
#include<utility>
#include<sstream>
#include<vector>
#include<cstring>
using std::pair;
using std::string;
using std::vector;
using std::cout;
using std::cin;

std::stringstream ss;
const int Size = 11;
int board[Size+2][Size+2];
pair<int,int> dir[6] = {{-1,0},{0,-1},{1,-1},{1,0},{0,1},{-1,1}};   //{c,r}
pair<int,int> bridge_dir[6] = {{-1,-1},{1,-2},{2,-1},{1,1},{-1,2},{-2,1}};
int cnt[6] = {0}; //{(bridge || extend), bridge, extend, (stick || bridge_block), stick, bridge_block}
int cnt_l=6;

//<tools>
string cr2str(pair<int,int> p);
pair<int,int> str2cr(string s);
pair<int,int> p_plused(pair<int,int> a, pair<int,int> b);
void board_init();
void board_setup(vector<string> movelist);
void Move(pair<int,int> pos,int current);
void print();
//</tools>

//behavior recognition
//<bridge> reference : bridge.cpp;
bool is_bridge(pair<int,int> pos1, pair<int,int> pos2);
bool bridge(pair<int,int> p);
bool unnecessary_savebridge(pair<int,int> p);
//</bridge>

//<stick>
bool stick(pair<int,int> p);
//</stick>

//<extend>
bool extend(pair<int,int> p);
//</extend>

//<cut>
bool type1_cut(pair<int,int> p);
bool type2_cut(pair<int,int> p);
bool cut(pair<int,int> p);
//</cut>

//<block>
bool player1_block(pair<int,int> p);
bool player2_block(pair<int,int> p);
bool block(pair<int,int> p);
bool bridge_block(pair<int,int> p);
//</block>

void MoveType(pair<int,int> p){
    bool br=0, ex=0, st=0, brbl=0; // bridge, extend, stick, bridge block
    if(bridge(p)) br = 1;
    if(extend(p)) ex = 1;
    if(stick(p)) st = 1;
    if(bridge_block(p)) brbl = 1;

    if(br || ex) cnt[0]++;
    if(br) cnt[1]++;
    if(ex) cnt[2]++;
    if(st || brbl) cnt[3]++;
    if(st) cnt[4]++;
    if(brbl) cnt[5]++;
    //else cout << "\nothers\n";
}

void MoveCnt(vector<string> movelist, int target_player, int steps){
    int player = 1;
    pair<int,int> tmp;
    //init
    board_init();
    for(int i=0;i<cnt_l;i++) cnt[i] = 0;
    //count
    for(int i=0;i<steps;i++){
        tmp = str2cr(movelist[i]);
        Move(tmp , player);


//        print();
        if(player == target_player) MoveType(tmp);
//        for(int i=0;i<cnt_l;i++) cout << cnt[i] << " ";
//        cout << "\n";
//        system("PAUSE");
        player *= -1;
    }
}

void get_movelist(string s, vector<string> &res){
    res.clear();
    int idx = 6;
    for(int i=7;i<s.length()-7;i++){
        if(s[i] == ' '){
            res.push_back(s.substr(idx+1,i-idx-1));
            idx = i;
        }
    }
    return;
}

int main(){
    int steps;
    string s;
    vector<string> movelist;
    string data_path = "C:\\Users\\bendo\\Downloads\\2023_02_22_n50_d10_sv_data.txt";
    string statistics_path = "D:\\desktop\\test_outfile.txt";
    std::ifstream infile;
    std::ofstream outfile;
    string cnt_structure = "<cnt_structure> (bridge || extend), bridge, extend, (stick || bridge_block), stick, bridge_block </cnt_structure>\n";

    infile.open(data_path);
    outfile.open(statistics_path, std::ios::app);

    outfile << cnt_structure;

    int _cnt=0;
    while(1){
        cout << "\r" << _cnt++;
        //system("PAUSE");
        getline(infile, s);
        if(s.length()==0) break;
        //cout << s << "\n";

        if(s.substr(0,5) == "<set>") outfile << s << "\n";
        else if(s.substr(0,6) == "<move>"){
            outfile << s << "\n";

            get_movelist(s, movelist);
            steps = movelist.size();    //steps setting
            MoveCnt(movelist, 1, steps);

            outfile << "<move_cnt>";
            for(int i=0;i<cnt_l;i++) outfile << " " << cnt[i];
            outfile << " </move_cnt>\n";
        }
    }

    infile.close();
    outfile.close();

    return 0;
}

string cr2str(pair<int,int> p){ //(char column, int row) to (int column, int row);
    ss.clear();                 //need stringstream
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

bool is_bridge(pair<int,int> pos1, pair<int,int> pos2){
    int i;
    pair<int,int> mid1, mid2;  //middle of the bridge
    for(i=0;i<6;i++){
        if(p_plused(pos1,bridge_dir[i]) == pos2) break;
    }
    //cout << "i = " << i << "\n";
    if(i==6) return 0;

    mid1 = p_plused(pos1,dir[i]);
    mid2 = p_plused(pos1,dir[(i+1)%6]);
    //cout << cr2str(mid1) << ":" << board[mid1.second][mid1.first] << ";" << cr2str(mid2) << ":" << board[mid2.second][mid2.first] << "\n";
    if(!(board[mid1.second][mid1.first] || board[mid2.second][mid2.first])) return 1;
    else return 0;
}

bool bridge(pair<int,int> p){
    pair<int,int> pos;

    for(int i=0;i<6;i++){
        pos = p_plused(p,bridge_dir[i]);
        if(0 <= pos.first && pos.first <= Size+1 && 0 <= pos.second && pos.second <= Size+1){
            if(is_bridge(p,pos) && (board[p.second][p.first] == board[pos.second][pos.first]) ) return 1;
        }
    }

    return 0;
}
bool unnecessary_savebridge(pair<int,int> p){
    int r = p.second;
    int c = p.first;
    for(int i=0;i<6;i++){
        int r1 = r+dir[i].second;
        int c1 = c+dir[i].first;

        int r2 = r+dir[(i+2)%6].second;
        int c2 = c+dir[(i+2)%6].first;

        int rm = r+dir[(i+1)%6].second;   //middle of the bridge
        int cm = c+dir[(i+1)%6].first;

        if(board[r1][c1] == board[r2][c2] && board[rm][cm] == 0 && board[r][c] == board[r1][c1]){
            return 1;
        }
    }
    return 0;
}
bool stick(pair<int,int> p){
    int r = p.second;
    int c = p.first;

    for(int i=0;i<6;i++){
        int tmp_r = r+dir[i].second;
        int tmp_c = c+dir[i].first;
        if(0 < tmp_r && tmp_r < Size+1 && 0 < tmp_c && tmp_c < Size+1){
            if( (board[r][c] + board[tmp_r][tmp_c]) == 0 ) return 1;
        }
    }
    return 0;
}
bool extend(pair<int,int> p){
    int r = p.second;
    int c = p.first;

    for(int i=0;i<6;i++){
        int tmp_r = r+dir[i].second;
        int tmp_c = c+dir[i].first;
        if( board[r][c] == board[tmp_r][tmp_c] ) return 1;
    }
    return 0;
}
void board_init(){
    for(int i=0;i<Size+2;i++){
        for(int j=0;j<Size+2;j++){
            if(i==0 || i==Size+1) board[i][j] = 1;
            else if(j==0 || j==Size+1) board[i][j] = -1;
            else board[i][j] = 0;
        }
    }
}
bool type1_cut(pair<int,int> p){
    int r = p.second;
    int c = p.first;
    for(int i=0;i<3;i++){
        int r1 = r+dir[i].second;
        int c1 = c+dir[i].first;
        int r2 = r+dir[(i+3)%6].second;
        int c2 = c+dir[(i+3)%6].first;
        if(board[r][c]*-1 == board[r1][c1] && board[r][c]*-1 == board[r2][c2]) return 1;
    }
    return 0;
}
bool type2_cut(pair<int,int> p){    //cut the bridge
    int r = p.second;
    int c = p.first;
    for(int i=0;i<6;i++){
        int r1 = r+dir[i].second;
        int c1 = c+dir[i].first;

        int r2 = r+dir[(i+2)%6].second;
        int c2 = c+dir[(i+2)%6].first;

        int rm = r+dir[(i+1)%6].second;   //middle of the bridge
        int cm = c+dir[(i+1)%6].first;

        if(board[r1][c1] == board[r2][c2] && board[rm][cm] == board[r][c] && board[r][c]*-1 == board[r1][c1]){
            return 1;
        }
    }
    return 0;
}
bool cut(pair<int,int> p){
    if(type1_cut(p) || type2_cut(p)) return 1;
    else return 0;
}
bool player1_block(pair<int,int> p){    //player 1 blocks player -1
    pair<int,int> d[6] = {dir[0],dir[2],dir[3],dir[5],bridge_dir[5],bridge_dir[2]};
    pair<int,int> pos;

    for(int i=0;i<6;i++){
        pos = p_plused(p,d[i]);
        if(i<4){    //dir
            if(board[pos.second][pos.first] == board[p.second][p.first]*-1) return 1;
        }
        else{       //bridge dir
            if(is_bridge(p,pos) && board[pos.second][pos.first] == board[p.second][p.first]*-1) return 1;
        }
    }
    return 0;
}
bool player2_block(pair<int,int> p){    //player -1 blocks player 1
    pair<int,int> d[6] = {dir[1],dir[2],dir[4],dir[5],bridge_dir[1],bridge_dir[4]};
    pair<int,int> pos;

    for(int i=0;i<6;i++){
        pos = p_plused(p,d[i]);
        if(i<4){    //dir
            if(board[pos.second][pos.first] == board[p.second][p.first]*-1) return 1;
        }
        else{       //bridge dir
            if(is_bridge(p,pos) && board[pos.second][pos.first] == board[p.second][p.first]*-1) return 1;
        }
    }
    return 0;
}
bool block(pair<int,int> p){
    if(board[p.second][p.first] == 1){
        return player1_block(p);
    }
    else return player2_block(p);
}
bool bridge_block(pair<int,int> p){
    int r = p.second, tmp_r;
    int c = p.first , tmp_c;
    for(int i=0;i<6;i++){
        tmp_r = r + bridge_dir[i].second;
        tmp_c = c + bridge_dir[i].first;
        if(0<tmp_r && tmp_r<Size+1 && 0<tmp_c && tmp_c<Size+1 && board[r][c]*-1 == board[tmp_r][tmp_c] && is_bridge(p,{tmp_c,tmp_r})){
            return 1;
        }
    }
    return 0;
}
void board_setup(vector<string> movelist){
    int player = 1;
    pair<int,int> tmp;
    for(string s : movelist){
        tmp = str2cr(s);
        board[tmp.second][tmp.first] = player;
        player *= -1;
    }
    return;
}
void Move(pair<int,int> pos,int current){
    if(board[pos.second][pos.first]!=0){
        board[pos.second][pos.first] = 0;
        board[pos.first][pos.second] = current;
    }
    else board[pos.second][pos.first] = current;
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
