#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "fordraw.h"
#include <set>
#include "audio.h"

using namespace sf;
using namespace std;
#define GTK(key)   Keyboard::isKeyPressed(Keyboard:: key )


class DeepGate{
public:
    double x, y;
    string file;
    int num;
    Texture tx[2];
    Sprite sp[2];
    DeepGate(){
        tx[0].loadFromFile("input/map/img/gate.png");
        tx[1].loadFromFile("input/map/img/gateframe.png");

        for(int i=0;i<2;i++){
            sp[i].setTexture(tx[i]);
            Vector2f sz = Vector2f(tx[i].getSize());
            sz.x = sz.x / 2;
            sz.y = sz.y;
            sp[i].setOrigin(sz);
        }
    }

    DeepGate(double X, double Y):DeepGate(){
        x=X; y=Y;
    }

    DeepGate(double X, double Y, string File, int Num):DeepGate(X, Y){
        file = File; num = Num;
    }

    DeepGate(Vector2f pos):DeepGate(){
        x=pos.x; y=pos.y;
    }

    void draw(RenderWindow &win){
        Vector2f pos = win.getView().getCenter();
        pos -= Vector2f(x, y);
        double pw = -0.01;
        pos = Vector2f(pos.x * pw, pos.y * pw);
        for(int i=1;i>-1;i--){
            sp[i].setTexture(tx[i]);
            if(i == 0)pos = Vector2f(0, 0);
            sp[i].setPosition(x + pos.x, y + pos.y);
            win.draw(sp[i]);
        }
    }

    IntRect getRect(){
        Vector2u sz = tx[0].getSize();
        return IntRect(x - sz.x/2, y - sz.y, sz.x, sz.y);
    }
};

class PaperAbil{
public:
    double x, y, w, h, dy=0;
    bool is=1;
    int ind;
    string id="";
    Texture tx;
    Sprite sp;
    double yofset=-7;
    PaperAbil(string s, long X, long Y, int Ind){
        ind = Ind;
        bool f=0;
        for(int i=0;i<s.size();i++){
            if(f)id += s[i];
            if(s[i] == '_')f=1;

        }
        tx.loadFromFile("input/Player/paperlist.png");
        sp.setTexture(tx);
        Vector2u sz = tx.getSize();
        w = sz.x; h = sz.y;
        x=X-w/2;
        y=Y-h/2;
    }

    void upd(float time, vector <Particle> &partic){
        if(!is)return;

        dy -= yofset * time * 5;
        yofset += dy * time;

        Color c(0, 150, 255);
        if(set<string>({"sword", "shield", "sword&shield"}).count(id)){
            c = Color(0, 255, 0);
        }else if(set<string>({"heal"}).count(id)){
            c = Color(255, 255, 0);
        }



        double ang = (rand() % 1000) * 6.28 / 1000.0, s = 25;
        Particle part(x+w/2, y+h/2+yofset, cos(ang) * s, sin(ang) * s, 2, 1, 5, c, Color(c.r, c.g, c.b, 0));
        partic.push_back(part);
    }

    void draw(RenderWindow &win){
        if(!is)return;
        sp.setTexture(tx);
        sp.setPosition(x, y+yofset);
        win.draw(sp);
    }

    bool xPlayer(IntRect pl){
        if(pl.intersects(IntRect(x, y, w, h))){
            is = 0;
            return 1;
        }
        return 0;
    }
};

class Breakble{
public:
    int br=0;
    int ind;
    Breakble(int Ind, int Br=0){ind = Ind; br=Br;}
    virtual bool brea(IntRect a, vector <Particle> &par){return 0;};
    virtual void draw(RenderWindow &win){};
};

class Crystal:public Breakble{
public:
    long x, y, w, h;
    Texture tx;
    Sprite sp;
    Crystal(long X, long Y, int Ind, int Br):Breakble(Ind, Br){
        //br=0;
        string img = "input/map/img/crystal.png";
        tx.loadFromFile(img);
        sp.setTexture(tx);
        w = tx.getSize().x / 2;
        h = tx.getSize().y;
        x=X-w/2;
        y=Y-h+1;
    }

    void draw(RenderWindow &win){
        sp.setTextureRect(IntRect(w * br, 0, w, h));
        sp.setPosition(x, y);
        win.draw(sp);
    }

    bool brea(IntRect a, vector <Particle> &par){
        if(br == 0 && a.intersects(IntRect(x, y, w, h))){
            br = 1;
            for(int i=0;i<=500;i++){
                    double j = (rand()%10000) * 6.28 / 10000.0;
                    double speed = (rand() % 1000) / 10.0;
                    par.push_back(Particle(
                        x+w/2+sin(j)*10, y+h/2+cos(j)*10, sin(j)*speed, cos(j)*speed,
                                           1, 0.5 + (rand() % 10) / 10.0, 100, Color(255, 255, 0), Color(255, 255, 0)
                    ));

            }
            return 1;
        }return 0;
    }

};

class RestCrystal{
public:
    long x, y, w, h, s;

    Texture tx;
    Sprite sp;
    RestCrystal(long X, long Y){
        string im = "input/map/img/restnew.png";
        tx.loadFromFile(im);
        w = tx.getSize().x;
        h = tx.getSize().y;
        s = 100;
        x=X-w/2;y=Y-h;
    }

    void draw(RenderWindow &win){
        sp.setPosition(x, y);
        sp.setTexture(tx);
        win.draw(sp);
    }

    bool activate(bool key, IntRect pl, vector <Particle> &par){
        if(pl.intersects(IntRect(x-s, y-s, w+s*2, h+s)) && key){
            for(int i=0;i<=150;i++){
                    double j = (rand()%10000) * 6.28 / 10000.0;
                    double speed = (rand() % 1000) / 10.0;
                    par.push_back(Particle(
                        x+w/2+sin(j)*10, y+h/2+cos(j)*10, sin(j)*speed, cos(j)*speed,
                                           1, 0.5 + (rand() % 1001) / 1000.0, 100, Color(255, 255, 255), Color(0, 255, 255)
                    ));

            }
            return 1;
        }
        return 0;
    }
};

struct GorizontalGate{
    double x, y, w, h;
    string s;
    int num;
    bool dir;

    IntRect getRect(){
        return IntRect(x, y, w, h);
    }
};

struct MapSpikes{
    vector <Vector2f> coor;
    Texture tx;
    Sprite sp;
    MapSpikes(){
        tx.loadFromFile("input/map/img/spike2.png");
        sp.setTexture(tx);
        Vector2f sz = Vector2f(tx.getSize());
        sz.x /= 2;
        sp.setOrigin(sz);
    }
    void draw(RenderWindow &win){
        for(Vector2f ps:coor){
            sp.setPosition(ps);
            win.draw(sp);
        }
    }

    bool xPlayer(int &damage, IntRect pl){
        Vector2u sz = tx.getSize();
        for(Vector2f ps: coor){
            int x = ps.x-sz.x/2, y = ps.y-sz.y;
            if(IntRect(x, y, sz.x, sz.y).intersects(pl)){
                damage = 4;
                return 1;
            }

        }
    }
};

class UnderSpikes{
public:
    double x, y, w, h;
    double pw=1, spead=3;
    int dm = 1;
    Texture tx;
    Sprite sp;
    UnderSpikes(double X, double Y){
        x=X; y=Y;
        tx.loadFromFile("input/map/img/undergroundspike.png");
        w = tx.getSize().x;
        h = tx.getSize().y;
        sp.setTexture(tx);
        x -= w/2;
    }

    bool xpl(IntRect pl){
        return pl.intersects(IntRect(x, y-getHeight()+1, w, getHeight())) && (pw > 0);
    }

    double getHeight(){
        return h * (1-pw*pw);
    }

    void upd(float time){
        pw += time * spead;
        if(pw > 1) pw = 1;
    }

    void draw(RenderWindow &win){
        sp.setTexture(tx);
        sp.setTextureRect(IntRect(0, 0, w, getHeight()));
        sp.setPosition(x, y-getHeight()+2);
        win.draw(sp);
    }

};

class Tablet{
public:
    long x, y, w, h;
    Texture tx;
    Sprite sp;
    string file;
    Tablet(string s, long X, long Y){
        file=s;x=X;y=Y;
        tx.loadFromFile("input/map/img/tablet.png");
        w=tx.getSize().x;
        h=tx.getSize().y;
        x-=w/2;
        y-=h;
        sp.setTexture(tx);
    }

    bool activate(IntRect p){
        int side=10;
        return p.intersects(IntRect(x-side, y-side, w+side*2, h+side));
    }

    void draw(RenderWindow &win){
        sp.setPosition(x, y);
        sp.setTexture(tx);
        win.draw(sp);
    }
};

struct underspikewave{
    int i;
    double t, mxt;
    bool alive=1;
    bool dir;

    underspikewave(int I, double MXT, bool DIR=0){
        i=I;
        t=mxt=MXT;
        dir=DIR;
    }
    template <class T>

    void upd(float time, T* mp){
        t -= time;
        while(t < 0){
            t += mxt;
            mp->activateUnderSpike(i);
            i += -2*(dir)+1;
            if(mp->under_spikes.size() <= i || i < 0){
                alive = 0;
                return;
            }
        }
    }

};

class unspwaveactivator{
public:
    double t=0, mxt, speed;
    bool dir;

    void create(double MXT, double SPEED, bool DIR){
        t=0;
        mxt=MXT;
        dir=DIR;
        speed = SPEED;
    }
    unspwaveactivator(double MXT, double SPEED, bool DIR){
        create(MXT, SPEED, DIR);
    }
    unspwaveactivator(string s){
        dir = s[s.size()-1] - '0';
        s.pop_back();
        if(s == "fast"){
            create(1.5, 30, dir);
        }if(s == "medium"){
            create(1.5, 10, dir);
        }if(s == "slow"){
            create(2.5, 3, dir);
        }
    }

    template <class T>
    void upd(float time, T* mp){
        t -= time;
        while(t < 0){
            t += mxt;
            int sz = mp->under_spikes.size();
            mp->unspkwave.push_back(underspikewave((sz-1)*dir, 1.0/speed, dir));
        }
    }
};

class Map{
public:
    vector <string> visited_rooms;
    vector <vector <int> > mp;
    vector <GorizontalGate> gorgates;
    vector <PaperAbil> papers;
    vector <DeepGate> dgates;
    vector <Firefly> frflys;
    vector <Tablet> tablets;
    vector <UnderSpikes> under_spikes;
    vector <underspikewave> unspkwave;
    vector <unspwaveactivator> unspkwvacts;
    MapSpikes spikes;

    vector <Breakble*> br;
    vector <RestCrystal> rest;
    IntRect vborder; /// borders for view
    Texture tx;
    Sprite sp;
    Texture btx;
    Sprite bsp;
    Shader sh;
    Texture w_adv_tx;
    Sprite w_adv_sp;
    int px=16;
    string file_of_room="";

    Map(){
        w_adv_tx.loadFromFile("input/images/W advice.png");
        w_adv_sp.setTexture(w_adv_tx);
        Vector2u wadtxsz = w_adv_tx.getSize();
        wadtxsz.x /= 2;
        w_adv_sp.setOrigin(Vector2f(wadtxsz));
    }

    void activateUnderSpike(int i){
        if(i >=0 && i < under_spikes.size()){
            if(under_spikes[i].pw == 1)
                under_spikes[i].pw=-1;
            else if(under_spikes[i].pw > 0)
                under_spikes[i].pw*=-1;
            }
    }

    bool xWalls(IntRect person){
         for(int i=person.top/px;i<=(person.top+person.height)/px;i++){
            if(i<0 || i >= mp.size())continue;
            for(int j=person.left/px;j<=(person.left+person.width)/px;j++){
                if(j<0 || j >= mp[i].size())continue;
                if(mp[i][j] != 0){return 1;}
        }
        }
        return 0;
    }

    int getmp(int x, int y){
        if(y < 0 || y >= mp.size())return 1;
        if(x < 0 || x >= mp[y].size())return 1;
        return mp[y][x];
    }

    void draw_advice(double x, double y, bool onGround, RenderWindow &win){
        for(RestCrystal i:rest){
            double X = i.x+i.w/2-x, Y=i.y+i.h/2-y;
            if(sqrt(X*X + Y*Y) < 100){
                w_adv_sp.setPosition(i.x+i.w/2, i.y-i.h-3);
                //w_adv_sp.setPosition(x, y);
                w_adv_sp.setTexture(w_adv_tx);
                win.draw(w_adv_sp);
            }
        }
        if(onGround){
            for(Tablet i: tablets){
                double X = i.x+i.w/2-x, Y=i.y+i.h/2-y;
                if(sqrt(X*X + Y*Y) < 200){
                    w_adv_sp.setPosition(i.x+i.w/2, i.y-3);
                    //w_adv_sp.setPosition(x, y);
                    w_adv_sp.setTexture(w_adv_tx);
                    win.draw(w_adv_sp);
                }
            }
        }
        for(DeepGate i: dgates){
            if(i.getRect().intersects(IntRect(x-20, y, 40, 1))){
                w_adv_sp.setPosition(i.x, i.y-3-i.tx[0].getSize().y);
                w_adv_sp.setTexture(w_adv_tx);
                win.draw(w_adv_sp);
            }

        }
    }

    void draw_front_ground(RenderWindow &win){
        View view = win.getView();
        Vector2f s = view.getSize(), p = view.getCenter();
        int x=max<int>(p.x-s.x/2, 0), y=max<int>(p.y-s.y/2, 0), w=s.x, h=s.y;


        /// Firefly
        px = 16;
        for(Firefly &i: frflys){
            Vector2f ps = i.sp.getPosition();
            if(getmp(int(ps.x / px), int(ps.y / px)) == 0);
                i.draw(win);
        }

        ///Front
        {
            for(int i=y/px;i<min<int>(mp.size(), (y+h+px)/px);i++)
            for(int j=x/px;j<min<int>(mp[i].size(), (x+w+px)/px);j++){
                if(mp[i][j] == 0)continue;
                int pow = mp[i/px][j/px];
                {
                    int pw;

                    sp.setPosition(j*px, i*px);
                    pw = getmp(j-1, i) + getmp(j-1, i-1)*2 + getmp(j, i-1)*4;
                    sp.setTextureRect(IntRect(pw * px / 2, 0, px/2, px/2));
                    win.draw(sp);
                    sp.setPosition(j*px + px / 2, i*px);

                    pw = getmp(j, i-1) + getmp(j+1, i-1)*2 + getmp(j+1, i)*4;
                    sp.setTextureRect(IntRect(pw * px / 2, px/2, px/2, px/2));
                    win.draw(sp);
                    sp.setPosition(j*px, i*px + px / 2);

                    pw = getmp(j-1, i) + getmp(j-1, i+1)*2 + getmp(j, i+1)*4;
                    sp.setTextureRect(IntRect(pw * px / 2, px, px/2, px/2));
                    win.draw(sp);
                    sp.setPosition(j*px + px / 2, i*px + px / 2);

                    pw = getmp(j, i+1) + getmp(j+1, i+1)*2 + getmp(j+1, i)*4;
                    sp.setTextureRect(IntRect(pw * px / 2, px/2*3, px/2, px/2));
                    win.draw(sp);

                }
            }
        }
    }

    void draw_middle_ground(RenderWindow &win){
        ///Deep Gates
            for(DeepGate i: dgates)i.draw(win);



        for(Breakble* i:br){
            (*i).draw(win);
            ///cout<<(*it)->br<<'\n';
        }
        for(auto &i: rest)i.draw(win);
        for(auto &i: papers)i.draw(win);
        for(auto &i: tablets)i.draw(win);
        spikes.draw(win);
        for(auto &i: under_spikes)i.draw(win);

    }

    void draw_back_ground(RenderWindow &win){
        View view = win.getView();
        Vector2f s = view.getSize(), p = view.getCenter();
        int x=max<int>(p.x-s.x/2, 0), y=max<int>(p.y-s.y/2, 0), w=s.x, h=s.y;

        //cout<<view.getCenter().x<<' '<<view.getCenter().y<<'\n';
        //sh.setUniform("view", view.getCenter());
        //sh.setUniform("size", Vector2f(btx.getSize()));
        //sh.setUniform("sizeof", Vector2f(view.getSize()));
        //sh.setUniform("texture", btx);
        //sh.setUniform("coord", Vector2f(x, y));
        //sh.setUniform("coof", 0.9f);
        Texture txfodr;
        //txfodr.create(w+10, h+10);
        txfodr.loadFromFile("input/map/img/stone.png");
        txfodr.setRepeated(1);
        Sprite spfordr(txfodr, IntRect(x*1.04, y*1.04, w+10, h+10));

        spfordr.setPosition(x, y);
        win.draw(spfordr, &sh);


    }

    void view_redactor(View & v, float length_of_dark_rects){
        double x = v.getCenter().x, y = v.getCenter().y, w=v.getSize().x/2, h=v.getSize().y/2;
        v.setCenter(
                    min <double> (vborder.width-w+length_of_dark_rects, max <double> (x, vborder.left+w-length_of_dark_rects)),
                    min <double> (vborder.height-h, max <double> (y, vborder.top+h)));
    }

    void save(vector <int> &states){
        ofstream out;
        out.open("input/map/lev/now/" + file_of_room);
        int n = states.size();
        for(Breakble* i:br){
            states[i->ind] = i->br;
        }

        for(PaperAbil i: papers){
            if(!i.is){
                states[i.ind] = -2;
            }
        }

        for(int i=0;i<n;i++){
            out<<states[i]<<' ';
        }

        out.close();
        save_visited_rooms();
    }

    void load(string file, vector <tuple<string, long, long, int> > &ret, vector <int> &states){
        {
            bool add_to_visited=1;
            for(auto &vroom: visited_rooms){
                if(vroom == file){
                    add_to_visited = 0;
                    break;
                }
            }
            if(add_to_visited){
                visited_rooms.push_back(file);
            }
        }
        if(file_of_room != ""){
            save(states);
        }

        file_of_room = file;

        //sh.loadFromFile("input/images/Shader5.frag", Shader::Fragment);

        ifstream in, in2;
        in.open("input/map/lev/orig/" + file);
        in2.open("input/map/lev/now/" + file);
        string s;

        in>>s;
        tx.loadFromFile("input/map/img/" + s);
        sp.setTexture(tx);

        in>>s;
        btx.loadFromFile("input/map/img/" + s);
        bsp.setTexture(btx);

        papers.clear();
        tablets.clear();
        under_spikes.clear();
        unspkwave.clear();
        unspkwvacts.clear();
        rest.clear();
        spikes.coor.clear();
        br.clear();

        {
            int n, x, y, st;
            in >> n;
            states.resize(n, 0);
            if(!in2){
                ofstream out;
                out.open("input/map/lev/now/" + file);
                for(int i=0;i<n;i++)out<<"0 ";
                out.close();

                in2.open("input/map/lev/now/" + file);
            }

            for(int i=0;i<n;i++){
                in>>s>>x>>y;
                in2>>st;

                states[i] = st;

                if(st == -1 || st == -2)continue;
                if(s == "RestCrystal"){
                    rest.push_back(RestCrystal(x, y));
                }else if(s == "Spike"){
                    spikes.coor.push_back(Vector2f(x, y));
                }else if(s == "Crystal"){
                    br.push_back(new Crystal(x, y, i, st));
                }else{
                    ret.push_back(make_tuple(s, x, y, i));
                }
            }
        }
        {
            gorgates.resize(0);
            int n, x, y, w, h, num;
            string s;
            bool dir;
            in >> n;
            while(n -- > 0){
                in >> x >> y >> w >> h >> s >> num >> dir;
                gorgates.push_back({x, y, w, h, s, num, dir});
            }
        }
        {
            dgates.resize(0);
            int n, x, y, num;
            string s;
            in>>n;
            while(n -- > 0){
                in>>x>>y>>s>>num;
                dgates.push_back(DeepGate(x, y, s, num));
            }
        }
        mp.resize(0);
        while(in>>s){
            mp.resize(mp.size() + 1);
            mp[mp.size()-1].resize(s.size());
            for(int i=0;i<s.size();i++){
                mp[mp.size()-1][i] = (s[i] == '_');
            }
        }
        in.close();
        vborder = IntRect(0, 0, mp[0].size()*px, mp.size()*px);
    }

    void for_br(IntRect a, vector <Particle> &par){
        for(auto it=br.begin();it!=br.end();it++){
            if((*it)->brea(a, par))
                audio.play_sound("crystalcrash");
        }

    }

    void get_exit(long double &x, long double &y, long double w, long double h, int gatenum){
        for(GorizontalGate &i: gorgates){
            if(i.num == gatenum){
                if(i.dir){
                    x = i.x+i.w+2;
                    y = i.y+i.h-h-1;
                }else{
                    x = i.x-w-2;
                    y = i.y+i.h-h-1;
                }
                return;
            }
        }
        for(DeepGate &i: dgates){
            if(i.num == gatenum){
                x = i.x - w / 2;
                y = i.y - h - 2;
                return;
            }
        }
    }

    bool x_gate(IntRect pl, string &file, int &num, bool deepgateneed=0){
        for(GorizontalGate &i: gorgates){
            if(pl.intersects(i.getRect())){
                file = i.s;
                num = i.num;
                return 1;
            }
        }
        if(deepgateneed)
        for(DeepGate &i: dgates){
            if(pl.intersects(i.getRect())){
                file = i.file;
                num = i.num;
                return 1;
            }
        }
        return 0;
    }

    void clear_map(){
        for(string i: visited_rooms){
            ifstream in;
            in.open("input/map/lev/now/" + i);
            vector <int> vals;
            int n;
            while(in >> n){
                vals.push_back(n);
            }
            in.close();
            ofstream out;
            out.open("input/map/lev/now/" + i);
            for(int j: vals){
                if(j == -1 || j == 1)
                    out<<"0 ";
                else
                    out<<j<<' ';
            }
            out.close();
        }
        visited_rooms.resize(1, file_of_room);
        for(auto &i: br)i->br=0;
        //system("del/Q .\\input\\map\\lev\\now");
    }

    void upd(float &time, vector <Particle> &partic, View &view){
        Vector2f xy = view.getCenter(), wh = view.getSize();
        xy -= wh;
        wh += wh;
        if(frflys.size() < 10)
            while(frflys.size() < 350)
                spawnFireFly(xy, wh, (rand() % 2001) / 1000.0 - 1);

        if(frflys.size() < 350)
            spawnFireFly(xy, wh, -1);

        for(int i=frflys.size()-1;i>=0;i--){
            frflys[i].upd(time, IntRect(xy.x, xy.y, wh.x, wh.y));
            if(frflys[i].t > 1){
                swap(frflys[i], frflys.back());
                frflys.pop_back();
            }
        }
        for(auto &i:papers)i.upd(time, partic);
        for(auto &i:under_spikes)i.upd(time);

        for(int i=unspkwave.size()-1;i>=0;i--){
            unspkwave[i].upd(time, this);
            if(!unspkwave[i].alive){
                swap(unspkwave.back(), unspkwave[i]);
                unspkwave.pop_back();
            }
        }

        for(auto &i:unspkwvacts) i.upd(time, this);
    }

    void spawnFireFly(Vector2f xy, Vector2f wh, float t){
        int x, y;
        x = rand() % int(wh.x);
        y = rand() % int(wh.y);
        frflys.push_back(Firefly(x + xy.x, y + xy.y));
        frflys.back().t = t;
    }

    void load_visited_rooms(){
        visited_rooms.clear();
        ifstream in;
        string s;
        in.open("input/map/visitedrooms.txt");
        while(getline(in, s)){
            visited_rooms.push_back(s);
        }
        in.close();
    }

    void save_visited_rooms(){
        ofstream out;
        out.open("input/map/visitedrooms.txt");
        for(string &s: visited_rooms){
            out<<s<<'\n';
        }
        out.close();
    }

    void newgame(){
        {
            ofstream vsrm;
            vsrm.open("input/map/visitedrooms.txt");
            vsrm.close();
        }
        ifstream allrooms; allrooms.open("input/map/allrooms.txt");
        string room;
        while(allrooms >> room){
            int n=0, rubbish;
            ifstream in; in.open("input/map/lev/now/" + room);
            while(in>>rubbish)n++;
            in.close();
            ofstream out; out.open("input/map/lev/now/" + room);
            while(n-->0)out<<"0 ";
            out.close();
        }
        file_of_room = "";
    }
}Main_Map;


#endif // MAP_H_INCLUDED
