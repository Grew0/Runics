#ifndef PAUSE_H_INCLUDED
#define PAUSE_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "control.h"
#include "conio.h"
#include <windows.h>

using namespace std;
using namespace sf;

struct{
    double t=0, mx=0, mx2=0;
    Texture tx;
    void reset(float time, double Mx2=0, RenderWindow *win=0){
        t = mx = time;
        if(Mx2 != 0){
            mx2 = Mx2 + time;
            t = mx2;
            tx.loadFromImage(win->capture());
        }else{
            mx2 = mx + 1;
        }
    }

    void upd(float time){
        t = max(0.0, t - time);
    }

    void draw(RenderWindow &win){
        View view = win.getView();
        Vector2f sz = view.getSize();
        RectangleShape sp(sz+sz);
        sp.setPosition(view.getCenter() - sz);
        if(t <= mx){
            if(mx == 0){
                sp.setFillColor(Color(0,0,0,0));
            }else
                sp.setFillColor(Color(0,0,0, (t/mx) * 255));
        }else{
            Sprite spp(tx);
            spp.setPosition(view.getCenter() - Vector2f(sz.x/2, sz.y/2));
            spp.setScale(sz.x / tx.getSize().x, sz.y / tx.getSize().y);
            win.draw(spp);
            if(t <= mx2 && mx2 != 0){
                sp.setFillColor(Color(0,0,0, ((mx2-t) / (mx2-mx)) * 255));
            }

        }
        win.draw(sp);
    }

}BlackSkreen;

class PauseClass{
public:
    Clock cl;
    float tim = 0;
    float titles_timer=0;
    bool is=0;
    int checktimer=0;
    Texture tx;
    Sprite sp;
    Key k = Key(36);
    Font font;
    Text txt;
    Key up = Key(22), down = Key(18), enter=Key(58), left=Key(0), right=Key(3);
    int ch=0, mxch=5;
    vector <string> text;
    vector <string> future_popaps;
    enum TYPE{pause, popap, titles};
    TYPE types;
    void addPopap(string s){
        future_popaps.push_back(s);
    }

    Anim anim;

    void check_future(RenderWindow &win){
        if(!is && future_popaps.size() && checktimer == 0){
            if(future_popaps.back() == "titles"){
                setPause(win, 2, "0");

            }else
                setPause(win, 1, future_popaps.back());
            future_popaps.pop_back();
            checktimer = 5;
        }
        if(!is){
            checktimer = max(0, checktimer-1);
        }
    }

    void setPause(RenderWindow &win, int type=0, string file=""){
        is = 1;
        Vector2f sz = Vector2f(win.getView().getSize());
        RectangleShape rc(sz);
        sz.x /= 2.0; sz.y /= 2.0;
        sp.setPosition(win.getView().getCenter() - sz);
        tx.loadFromImage(win.capture());
        font.loadFromFile("input/fonts/Pixel Times.ttf");
        txt.setFont(font);
        ch = 0;
        types = TYPE(type);
        if(types == TYPE::popap){
            text.resize(0);
            ifstream in;
            in.open("input/map/popaps/" + file + ".txt");
            string s;
            while(getline(in, s)){
                text.push_back(s);
            }
        }else if(types == TYPE::titles){
            titles_timer = 0;
            setlocale(LC_ALL, "");
            text.resize(0);
            ifstream in;
            in.open("input/titles/0.txt");
            string s;
            while(getline(in, s)){
                text.push_back(s);
            }
        }
        tim = 0;
    }

    void draw(RenderWindow &win){
        ///win.clear();
        sp.setTexture(tx);
        Vector2f sz = Vector2f(win.getView().getSize());
        sp.setScale(sz.x/tx.getSize().x, sz.y/tx.getSize().y);
        sp.setColor(Color(150, 150, 150));
        sz.x /= 2.0; sz.y /= 2.0;
        sp.setPosition(win.getView().getCenter() - sz);
        win.draw(sp);

        if(types == TYPE::titles)
            win.clear();

        setlocale(LC_ALL, "Russian");
        txt.setOrigin(0, 0);
        switch(types){
        case TYPE::pause:
                for(int i=0;i<mxch;i++){
                    txt.setPosition(win.getView().getCenter());
                    switch(i){
                        case 0:
                            txt.setString(String("Продолжить"));
                        break;
                        case 1:
                            txt.setString(String("Выход"));
                        break;
                        case 2:
                            txt.setString(String("Новая Игра"));
                        break;
                        case 3:
                            txt.setString(String("Громкость музыки " + audio.strVolMus()));
                        break;
                        case 4:
                            txt.setString(String("Громкость звуков " + audio.strVolSd()));
                        break;
                    }
                    if(i == ch)txt.setFillColor(Color(255, 255, 0));
                    else txt.setFillColor(Color(255, 255, 255));
                    txt.move(-txt.getLocalBounds().width/2, 20 * (i*2-4)-txt.getLocalBounds().height/2);
                    win.draw(txt);
                }
            break;
            case TYPE::popap:{
                Vector2f cen = win.getView().getCenter();
                for(int i=0;i<text.size();i++){
                    txt.setString(String(text[i]));
                    FloatRect rc = txt.getGlobalBounds();
                    txt.setPosition(-rc.width/2, -rc.height/2 + 40 * (i) - 20 * text.size());
                    txt.move(cen);
                    txt.setFillColor(Color::White);
                    win.draw(txt);
                }

            }break;
            case TYPE::titles:
                double ylast=0;
                Vector2f ps = win.getView().getCenter();
                for(int i=0;i<text.size();i++){
                    txt.setString(String(text[i]));
                    txt.setOrigin(txt.getGlobalBounds().width/2, txt.getGlobalBounds().height/2);
                    txt.setPosition(ps + Vector2f(0, -min(titles_timer, 745.f)+i*txt.getGlobalBounds().height*1.5));
                    ylast = txt.getPosition().y;
                    win.draw(txt);
                }
                anim.draw(win, ps.x, ylast + 100, 0, 0);
            break;
        }
        ///win.display();
    }

    string upd(RenderWindow &win, float time){
        switch(types){
            case TYPE::pause:
                if(up.press())ch --;
                if(down.press())ch ++;
                if(ch < 0)ch += mxch;
                if(ch >= mxch)ch -= mxch;

                if(enter.press()){
                    switch(ch){
                        case 3:
                        case 0:
                            is = 0;
                        break;
                        case 1:
                            win.close();
                        break;
                        case 2:
                            is=0;
                            return "NewGame";
                    }

                }
                if(ch == 3){
                    audio.setVolMus(audio.volm-left.press()+right.press());
                }
                if(ch == 4){
                    audio.setVolSd(audio.vols-left.press()+right.press());
                }
                left.upd();
                right.upd();
                up.upd();
                down.upd();
                enter.upd();

            break;
            case TYPE::popap:
                if(tim > 1){
                    for(int i=0;i<Keyboard::KeyCount;i++)
                        if(GTK(Key(i)))
                            is = 0;
                }
            break;
            case TYPE::titles:
                if(titles_timer == 0)cl.restart();
                //cout<<titles_timer<<'\n';
                titles_timer += time*50;
                if(titles_timer > 945){
                    return "NewGame";
                }
                anim.upd(time);
                anim.speed = 7;
            break;
        }
        tim += time;

        return "";
    }
}Pause;


#endif // PAUSE_H_INCLUDED
