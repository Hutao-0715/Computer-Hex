#include<bits/stdc++.h>
using namespace std;

const int Size = 11;
int board[Size+2][Size+2];
// 0 1 1 1 1 1 0
// -1 0 0 0 0 0 -1
//  -1 0 0 0 0 0 -1
//   -1 0 0 0 0 0 -1
//    -1 0 0 0 0 0 -1
//     -1 0 0 0 0 0 -1
//       0 1 1 1 1 1  0

stringstream ss;
pair<int,int> dir[6] = {{-1,0},{0,-1},{1,-1},{1,0},{0,1},{-1,1}}; //six grid that connect to a single grid
//  1 2
// 0 O 3
//  5 4
int random(int range){
    return rand() % range;
}

pair<int,int> p_plused(pair<int,int> a, pair<int,int> b){    //plused for pair<int,int>
    return {a.first+b.first, a.second+b.second};
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

pair<int,int> save_bridge(vector<string> m){
    int Board[Size+2][Size+2]={0};
    pair<int,int> cur = str2cr(m[m.size()-1]);
    vector<pair<int,int>> save;    //the place that have to put a chess to save bridge;
    int player = 1;

    //set up board
    for(string str : m){
        pair<int,int> p = str2cr(str);
        Board[p.second][p.first] = player;
        player *= -1;
    }

    //recognize the threatened bridge(s);
    //case1 not on the side
    for(int i=0;i<6;i++){
        pair<int,int> pos1 = p_plused(cur,dir[i]);
        pair<int,int> pos2 = p_plused(cur,dir[(i+2)%6]);
        pair<int,int> mid = p_plused(cur,dir[(i+1)%6]); //middle of the bridge
        if(Board[pos1.second][pos1.first]==player && Board[pos2.second][pos2.first]==player && Board[mid.second][mid.first]==0){
            save.push_back(mid);
        }
    }

    //case2 on the side
    if(player == 1){
        if(cur.second == 1){
            pair<int,int> pos1 = p_plused(cur,dir[5]);
            pair<int,int> mid = p_plused(cur,dir[0]);
            if(Board[pos1.second][pos1.first]==1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
            pos1 = p_plused(cur,dir[4]);
            mid = p_plused(cur,dir[3]);
            if(Board[pos1.second][pos1.first]==1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
        }
        else if(cur.second == Size){
            pair<int,int> pos1 = p_plused(cur,dir[1]);
            pair<int,int> mid = p_plused(cur,dir[0]);
            if(Board[pos1.second][pos1.first]==1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
            pos1 = p_plused(cur,dir[2]);
            mid = p_plused(cur,dir[3]);
            if(Board[pos1.second][pos1.first]==1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
        }
    }
    else{
        if(cur.first == 1){
            pair<int,int> pos1 = p_plused(cur,dir[2]);
            pair<int,int> mid = p_plused(cur,dir[1]);
            if(Board[pos1.second][pos1.first]==-1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
            pos1 = p_plused(cur,dir[3]);
            mid = p_plused(cur,dir[4]);
            if(Board[pos1.second][pos1.first]==-1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
        }
        else if(cur.first == Size){
            pair<int,int> pos1 = p_plused(cur,dir[0]);
            pair<int,int> mid = p_plused(cur,dir[1]);
            if(Board[pos1.second][pos1.first]==-1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
            pos1 = p_plused(cur,dir[5]);
            mid = p_plused(cur,dir[4]);
            if(Board[pos1.second][pos1.first]==-1 && Board[mid.second][mid.first]==0){
                save.push_back(mid);
            }
        }
    }

    //chose a bridge to save
    if(save.size()==1) return save[0];
    else if(save.size()) return save[random(save.size())];

    return {0,0};
}

int main(){
    srand(time(NULL));

    string s;
    vector<string> M;
    int n;

    cin >> n;
    for(int i=0;i<n;i++){
        cin >> s;
        M.push_back(s);
    }

    auto a = save_bridge(M);
    cout << cr2str(a);
    return 0;
}
