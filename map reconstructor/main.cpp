#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main(){
    ifstream in;
    in.open("input.txt");

    vector <string> S;
    string s;
    while(in>>s)
        S.push_back(s);
    vector <vector<int> > a(S.size());
    for(int i=0;i<S.size();i++){
        a[i].resize(S[i].size());
        for(int j=0;j<S[i].size();j++){
            if(S[i][j] == '1')a[i][j] = 15;
            else a[i][j] = 11;
        }
    }
    for(int i=1;i<a.size()-1;i++){
        for(int j=1;j<a[i].size()-1;j++){
            if(S[i][j]!='1')continue;
            if(S[i-1][j]!='1'){
                a[i][j] = 3;
                if(S[i][j-1]!='1')a[i][j]=8;
                if(S[i][j+1]!='1')a[i][j]=9;
                continue;
            }
            if(S[i+1][j]!='1'){
                a[i][j] = 6;
                if(S[i][j-1]!='1')a[i][j]=12;
                if(S[i][j+1]!='1')a[i][j]=13;
                continue;
            }
            if(S[i][j-1]!='1'){
                a[i][j]=2;
                continue;
            }
            if(S[i][j+1]!='1'){
                a[i][j]=7;
                continue;
            }
            for(int u=0;u<4;u++){
                if(S[i-(u%2)*2+1][j-(u/2)*2+1]!='1'){
                    a[i][j] = u/2+4*(u%2);
                }
            }
        }
    }
    ofstream out;
    out.open("sfml show/bin/debug/input.txt");
    for(int i=0;i<a.size();i++){
        for(int j=0;j<a[i].size();j++)
            out<<char(a[i][j]+'a');
        out<<'\n';
    }
    out.close();
    return 0;
}
