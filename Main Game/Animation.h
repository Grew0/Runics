#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

class Anim{
public:
    Sprite sp;
    int mx;
    vector <Vector2f> shift;
    vector <IntRect> fr;
    double nw=0, speed;
    bool cicle=1;

    /*Anim(Texture &tx, string file){
        ifstream in(file);
        in>>mx>>speed>>cicle;
        fr.resize(mx);
        shift.resize(mx);
        for(int i=0;i<mx;i++){
            in>>fr[i].left>>fr[i].top>>fr[i].width>>fr[i].height;
            in>>shift[i].x>>shift[i].y;
        }
        sp.setTexture(tx);
        ///sp.setScale(0.75, 0.75);
    }*/

    void upd(float time){
        ///cout<<time * speed<<'\n';
        nw += time * speed;
        while(nw >= mx && cicle)nw -= mx;
        while(nw < 0 && cicle)nw += mx;
    }

    bool is_end(){
        return nw >= mx;
    }

    void draw(RenderWindow &win, double X, double Y, double W, bool flip){
        int nw1 = min(mx-1, int(nw));
        //if(is_end())return;
        //sp.setPosition(shift[int(nw)] + Vector2f(X, Y));
        if(!flip){
            sp.setPosition(shift[nw1] + Vector2f(X, Y));
            sp.setTextureRect(fr[nw1]);
        }else{
            sp.setPosition(Vector2f(-fr[nw1].width-shift[nw1].x + W, shift[nw1].y) + Vector2f(X, Y));

            sp.setTextureRect(IntRect(fr[nw1].left+fr[nw1].width, fr[nw1].top, -fr[nw1].width, fr[nw1].height));
        }
        win.draw(sp);
    }

};

class AnimManager{
public:
    vector <Anim> anims;
    int choose=0, chfornow;
    Texture tx;

    AnimManager(string im, string file){
        tx.loadFromFile(im);
        ifstream in;
        in.open(file);
        int n;
        in>>n;
        anims.resize(n);
        for(int q = 0;q<n;q++){
            in>>anims[q].mx>>anims[q].speed>>anims[q].cicle;
            anims[q].sp.setTexture(tx);
            for(int i=0;i<anims[q].mx;i++){
                IntRect r;
                Vector2f v;
                in>>r.left>>r.top>>r.width>>r.height>>v.x>>v.y;
                anims[q].fr.push_back(r);
                anims[q].shift.push_back(v);
            }


        }
    }

    /*void push_anim(string file){
        anims.push_back(Anim(tx, file));
    }*/

    void upd(float time){
        anims[choose].upd(time);
    }

    void draw(RenderWindow &win, double x, double y, double w, bool flip){
        anims[choose].draw(win, x, y, w, flip);
    }

    bool is_end(){
        anims[choose].is_end();
    }

    void setAnim(int num){
        int ch1 = (num % anims.size() + anims.size()) % anims.size();
        chfornow = ch1;
        choose = ch1;
    }

    void endofchoosing(){
        if(chfornow != choose){
            anims[choose].nw = 0;
            choose = chfornow;
        }
    }

    void reset(){
        anims[choose].nw = 0;
    }

    void main_reset(){
        for(auto &i: anims)i.nw = 0;
    }
};

#endif // ANIMATION_H_INCLUDED
