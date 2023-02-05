#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <cmath>
#include <windows.h>

#define GTK(key) Keyboard::isKeyPressed(Keyboard :: key)

using namespace std;
using namespace sf;

string BEGIN;

void push_in_map(vector <vector<int>> &mp, int x, int y, int val){
    if(x < 0 || y < 0)return;
    if(mp.size() <= y){
        int i=mp.size(), n=0;
        if(i>0)n=mp[0].size();
        mp.resize(y+1);
        for(;i<mp.size();i++){
            mp[i] = vector<int>(n, 0);
        }
    }
    if(mp[0].size() <= x){
        for(int i=0;i<mp.size();i++){
            int j=mp[i].size();
            mp[i].resize(x+1);
            for(;j<x+1;j++)mp[i][j] = 0;
        }
    }
    mp[y][x] = val;


}

void draw_line(vector <vector<int> >&mp, int x, int y, int X, int Y, int val){
    /*if(abs(x-X) < 2 && abs(y-Y) < 2){
        push_in_map(mp, x, y, val);
        push_in_map(mp, X, Y, val);
        return;
    }
    int xx=(x+X)/2, yy=(y+Y)/2;
    draw_line(mp, x, y, xx, yy, val);
    draw_line(mp, X, Y, xx, yy, val);
    */
    for(int i=min(x, X);i<=max(x, X);i++){
        push_in_map(mp, i, y, val);
        push_in_map(mp, i, Y, val);
    }
    for(int i=min(y, Y);i<=max(y, Y);i++){
        push_in_map(mp, x, i, val);
        push_in_map(mp, X, i, val);
    }
    return;
}

void zalivka(vector<vector <int> > &mp, int x, int y, int val, int val2){
    if(y < 0 || y >= mp.size() || x < 0)return;
    if(mp[y].size() <= x)return;
    if(mp[y][x] != val2)return;
    mp[y][x] = val;
    zalivka(mp, x+1, y, val, val2);
    zalivka(mp, x-1, y, val, val2);
    zalivka(mp, x, y+1, val, val2);
    zalivka(mp, x, y-1, val, val2);
    return;
}

void check_mp_size(vector <vector<int> > &mp){
    while(mp.size()){
        bool f = 1;
        for(int i=0;i<mp[mp.size()-1].size();i++){
            if(mp[mp.size()-1][i] != 0){
                f = 0;
                break;
            }
        }
        if(f){
            mp.pop_back();
        }else{
            break;
        }
    }
    if(mp.empty())return;
    while(1){
        bool f=1;
        for(int i=0;i<mp.size();i ++){
            if(mp[i].back() != 0){
                f = 0;
                break;
            }
        }
        if(f){
            for(int i=0;i<mp.size();i++)mp[i].pop_back();
        }else{
            break;
        }
    }
}

struct{
    bool is_enter = 0;
    string s="";
}for_dyn;

void load(vector <vector<int> > &mp,
    vector <tuple <int, int, string> > &dynamic,
    vector <tuple<IntRect, string, string, bool> > &gor_por,
    vector <tuple<Vector2f, string, string> > &deep_por){

    mp.clear();
    dynamic.clear();
    gor_por.clear();
    deep_por.clear();

    ifstream in;
        in.open("file.txt");
        getline(in, BEGIN);
        {
            int n, x, y;
            in>>n;
            string s;
            while(n -- > 0){
                in>>s>>x>>y;
                dynamic.push_back(make_tuple(x, y, s));
            }
        }
        {
            int n, x, y, w, h;
            in>>n;
            string s, s1;
            bool f;
            while(n -- > 0){
                in>>x>>y>>w>>h>>s>>s1>>f;
                gor_por.push_back(make_tuple(IntRect(x, y, w, h), s, s1, f));
            }
            in>>n;
            while(n -- > 0){
                in>>x>>y>>s>>s1;
                deep_por.push_back(make_tuple(Vector2f(x, y), s, s1));
            }
        }
        string s;
        while(in>>s){
            mp.push_back(vector<int> (s.size()));
            for(int i=0;i<s.size();i++){
                mp.back()[i] = (s[i] == '_');
            }
        }

}

void save(vector <vector<int> > &mp,
    vector <tuple <int, int, string> > &dynamic,
    vector <tuple<IntRect, string, string, bool> > &gor_por,
    vector <tuple<Vector2f, string, string> > &deep_por){


    ofstream out;
    out.open("file.txt");
    out<<BEGIN<<'\n';
    out<<dynamic.size()<<'\n';
    for(auto i: dynamic){
        out<<get<2>(i)<<' '<<get<0>(i)<<' '<<get<1>(i)<<'\n';
    }
    out<<gor_por.size()<<'\n';
    for(auto i: gor_por){
        auto j = get<0>(i);
        out<<j.left<<' '<<j.top<<' '<<j.width<<' '<<j.height<<' '
        <<get<1>(i)<<' '<<get<2>(i)<<' '<<get<3>(i)<<'\n';
    }
    out<<deep_por.size()<<'\n';
    for(auto i: deep_por){
        out<<get<0>(i).x<<' '<<get<0>(i).y<<' '<<get<1>(i)<<' '<<get<2>(i)<<'\n';

    }
    for(int i=0;i<mp.size();i++){
        for(int j=0;j<mp[i].size();j++){
            out<<(mp[i][j]? '_' : '.');
        }
        out<<'\n';
    }
    out.close();

}

int main(){
    //RenderWindow win(VideoMode::getDesktopMode(), "sfml", Style::Fullscreen);
    RenderWindow win(VideoMode(800, 600), "sfml");
    vector <vector<int>> mp;
    vector <tuple <int, int, string> > dynamic;
    vector <tuple<IntRect, string, string, bool> > gor_por;
    vector <tuple<Vector2f, string, string> > deep_por;
    Vector2f point1, point_for_space, point_for_gate[2];
    enum {walls, dyns, gate} mod=walls;
    int gatestage = 0;
    string gatestring[2] = {"", ""};
    bool gatedir=0;
    win.setFramerateLimit(60);
    View view(FloatRect(0, 0, 800, 600));
    bool is_space=0;

    {
        load(mp, dynamic, gor_por, deep_por);
    }



    while(win.isOpen()){
        Event event;
        if(mod == dyns && GTK(Enter))for_dyn.is_enter=0;

        if(mod == gate && GTK(Enter)){
            if(gatestage == 0)gatestage = 1;
            gatestage++;
            if(gatestage == 4){
                gatestage = 0;
                point_for_gate[1] -= point_for_gate[0];
                int x=point_for_gate[0].x, y=point_for_gate[0].y, w=point_for_gate[1].x, h=point_for_gate[1].y;
                if(w < 0){
                    x += w;
                    w *= -1;
                }
                if(h < 0){
                    y += h;
                    y *= -1;
                }
                if(w == h && w == 0){
                    deep_por.push_back(make_tuple(
                                             Vector2f(x, y),
                                             gatestring[0],
                                             gatestring[1]
                                             ));
                }else
                    gor_por.push_back(make_tuple(
                                             IntRect(x, y, w, h),
                                             gatestring[0],
                                             gatestring[1],
                                             (x < int(mp[0].size() * 8))
                                             ));
                point_for_gate[0] = point_for_gate[1] = Vector2f(0, 0);
            }
            while(GTK(Enter)){}
        }
        while(win.pollEvent(event)){
            if(event.type == Event::Closed || GTK(Escape))
                win.close();
            if(event.type == Event::KeyPressed){
                if(event.key.code == Keyboard::S && GTK(LControl)){
                    save(mp, dynamic, gor_por, deep_por);
                    load(mp, dynamic, gor_por, deep_por);
                    save(mp, dynamic, gor_por, deep_por);


                }
            }
            if (event.type == sf::Event::TextEntered && for_dyn.is_enter){
                char abc = static_cast<char>(event.text.unicode);
                int ABC = int(abc);
                if(ABC == 8){
                    string &s = for_dyn.s;
                    if(s.size() > 0)
                        s.resize(s.size()-1);
                }else{
                    if(ABC != 32)
                        for_dyn.s += abc;
                }
            }
            if (event.type == sf::Event::TextEntered && (gatestage == 2 || gatestage == 3)){
                char abc = static_cast<char>(event.text.unicode);
                ///cout<<gatestring[0]<<' '<<abc<<'\n';
                int ABC = int(abc);
                if(ABC == 8){
                    string &s = gatestring[gatestage-2];
                    if(s.size() > 0)
                        s.resize(s.size()-1);
                }else{
                    if(ABC != 32)
                        gatestring[gatestage-2] += abc;
                }
            }
        }


        if((Mouse::isButtonPressed(Mouse::Left) || Mouse::isButtonPressed(Mouse::Right)) && !GTK(Space)){
            if(mod == walls){
                int val = (Mouse::isButtonPressed(Mouse::Left));
                Vector2f pos = win.mapPixelToCoords(Mouse::getPosition(win));
                int x = (pos.x) / 16, y = (pos.y) / 16;
                if(GTK(G)){
                    zalivka(mp, x, y, val, 1-val);

                }

                if(GTK(LShift)){
                    draw_line(mp, point1.x, point1.y, x, y, val);
                    point1 = Vector2f(x, y);
                }else{
                    push_in_map(mp, x, y, val);
                    point1 = Vector2f(x, y);
                }
                check_mp_size(mp);
            }else if(mod == dyns){
                Vector2f pos = win.mapPixelToCoords(Mouse::getPosition(win));
                int x = int(pos.x) / 16 * 16, y = int(pos.y + 16) / 16 * 16;
                x += 8;
                if(Mouse::isButtonPressed(Mouse::Left)){
                    if(!dynamic.empty()){
                        if(get<0>(dynamic.back()) != x || get<1>(dynamic.back()) != y)
                            dynamic.push_back(make_tuple(x, y, for_dyn.s));
                    }else
                        dynamic.push_back(make_tuple(x, y, for_dyn.s));
                }else{
                    for(int i=dynamic.size()-1;i>=0;i--){
                        if(get<0>(dynamic[i]) == x && get<1>(dynamic[i]) == y){
                            swap(dynamic[i], dynamic.back());
                            dynamic.pop_back();
                        }
                    }
                }
            }else if(mod == gate){
                Vector2f pos = win.mapPixelToCoords(Mouse::getPosition(win));
                int x = (pos.x+16*(pos.x>0)-8) / 16, y = (pos.y+16*(pos.y>0)-8) / 16;
                if(gatestage == 1){
                    point_for_gate[1] = Vector2f(x*16, y*16);
                }else{
                    point_for_gate[0] = Vector2f(x*16, y*16);

                    gatestage = 1;
                }
            }
        }else{
            if(mod == gate && gatestage == 1){gatestage = 0;}

        }

        if(GTK(Space) && Mouse::isButtonPressed(Mouse::Left)){
            if(is_space){
                point_for_space -= Vector2f(Mouse::getPosition(win));
                view.move(point_for_space);
                point_for_space = Vector2f(Mouse::getPosition(win));
            }else{
                point_for_space = Vector2f(Mouse::getPosition(win));
                is_space = 1;
            }
        }else{
            is_space = 0;
        }

        if(!for_dyn.is_enter && gatestage != 2 && gatestage != 3){
            if(GTK(W))mod = walls;
            else if(GTK(D)){
                for_dyn.s = "";
                for_dyn.is_enter= 1;
                mod = dyns;
            }else if(GTK(A)){
                mod = gate;
            }else if(GTK(C)){
                mp.resize(0);
                gor_por.resize(0);
                dynamic.resize(0);
            }
        }
        //if(mod == dyns)cout<<for_dyn.s<<'\n';
        //cout<<gatestage<<'\n';

        win.setView(view);
        win.clear(Color(150, 150, 150));
        {
            Vector2f pos = -view.getSize(), sz;
            pos.x /= 2;     pos.y /= 2;
            pos += view.getCenter();
            sz = pos + view.getSize();
            for(int i=pos.x-int(pos.x)%16; i < sz.x;i+=16){
                for(int j=pos.y-int(pos.y)%16; j < sz.y;j+=16){
                    {
                        if(i % 32 == j % 32){
                            RectangleShape sp(Vector2f(16, 16));
                            sp.setFillColor(Color(125, 125, 125));
                            sp.setPosition(i, j);
                            win.draw(sp);
                        }
                    }

                    CircleShape sp(1);
                    sp.setPosition(i, j);
                    if(mp.size())
                        if((j/16==mp.size() && i/16 <= mp[0].size()) || (i/16==mp[0].size() && j/16 <= mp.size())){
                            sp.setRadius(3);
                            sp.setFillColor(Color::Yellow);
                        }
                    if(j * i == 0 && min(i, j)>=0){
                        sp.setRadius(3);
                        sp.setFillColor(Color::Yellow);
                    }
                    win.draw(sp);

                }
            }

        }
        for(int i=0;i<mp.size();i++)
        for(int j=0;j<mp[i].size();j++){
            RectangleShape sp(Vector2f(16, 16));
            sp.setPosition(j*16, i*16);
            if(mp[i][j])
                win.draw(sp);

        }
        if(mod == dyns || mod == gate){
            Font font;
            font.loadFromFile("input/arial.ttf");
            string s=for_dyn.s;
            if(mod == gate){
                if(gatestage == 2)s = gatestring[0];
                if(gatestage == 3)s = gatestring[1];
            }
            Text txt(String(s), font);
            auto pos = -view.getSize();
            pos.x /= 2;     pos.y /= 2;
            pos += view.getCenter();
            txt.setPosition(pos);
            txt.setColor(Color(255, 0, 255));
            win.draw(txt);
        }
        for(auto i:dynamic){
            CircleShape sp(3);
            sp.setPosition(get<0>(i)-3, get<1>(i)-3);
            sp.setFillColor(Color(0, 255, 255));
            win.draw(sp);
        }
        if(point_for_gate[1] != point_for_gate[0]){
            RectangleShape sp(point_for_gate[1] - point_for_gate[0] + Vector2f(1, 1));
            sp.setPosition(point_for_gate[0]);
            sp.setFillColor(Color(0, 255, 0, 150));
            win.draw(sp);
        }else{
            CircleShape sp(10);
            sp.setPosition(point_for_gate[0] - Vector2f(sp.getRadius(), sp.getRadius()));
            sp.setFillColor(Color(0, 255, 0, 150));
            win.draw(sp);
        }
        for(auto i:gor_por){
            IntRect r = get<0>(i);
            RectangleShape sp(Vector2f(r.width, r.height));
            sp.setPosition(r.left, r.top);
            sp.setFillColor(Color(0, 255, 0, 50));
            win.draw(sp);
        }
        for(auto i:deep_por){
            CircleShape sp(10);
            sp.setPosition(get<0>(i) - Vector2f(sp.getRadius(), sp.getRadius()));
            sp.setFillColor(Color(0, 255, 0, 75));
            win.draw(sp);
        }
        win.display();
    }
	return 0;
}
