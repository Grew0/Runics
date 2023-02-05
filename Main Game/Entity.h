#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Map.h"
#include "Animation.h"
#include "Ability.h"
#include "fordraw.h"
#include "control.h"
#include <list>

using namespace sf;
using namespace std;
#define GTK(key)   Keyboard::isKeyPressed(Keyboard:: key )

class Entity{
public:
    int index_in_map = -1;
    long double x, y, w, h, dx=0, dy=0, grav=400, speed=0;
    bool dir=0, onGround=0, alive=1;

    string type = "en", type2="not", stype="not";/// en = Entity, pl = Player

    Entity(){}

    bool x_map(Map &mp, double X, double Y){
        return mp.xWalls(IntRect(X, Y, w, h));
    }

    void upd_entity(float time, Map &mp, list <Entity*> &es){
        if(dx > 0)dir=1;
        if(dx < 0)dir=0;


        dy += grav*time;
    }

    void draw_entity(RenderWindow &win, short c1, short c2, short c3){
        RectangleShape sp(Vector2f(w, h));
        sp.setOutlineColor(Color(c1/1.5, c2/1.5, c3/1.5, 100));
        sp.setFillColor(Color(c1, c2, c3, 0));
        sp.setOutlineThickness(-3);
        sp.setPosition(x, y);
        win.draw(sp);
    }

    void setCoord(double X, double Y){
        x=X;y=Y;
    };

    bool x_other(Entity &o){
        return o.getRect().intersects(getRect());
    }

    IntRect getRect(){
        return IntRect(x, y, w, h);
    }

    virtual void upd(float time, Map &mp, list <Entity*> &es, vector <Particle> &partic){};
    virtual void draw(RenderWindow &win){};
    virtual void hit(Entity &o){};
    virtual void get_hit(double dam, double X, double Y){};
    virtual int getstate(){
        if(alive) return 0;
        return -1;
    }
};

/*
class Bullet:public Entity{
public:
    double t = 0.9;

    Bullet(float X, float Y, bool d){
        type = "PBullet";
        x=X;y=Y;w=64;h=w;
        dx = 300 * (2*d-1);
        //dy = (rand() % 51 - 25) / 5.0 - 100;
    }

    void upd(float time, Map &mp, list <Entity*> &es, vector <Particle> &partic){
        x += dx * time;
        if(x_map(mp, x, y))
            alive = 0;
        t -= time;
        if(t < 0)alive = 0;
    }

    void draw(RenderWindow &win){
        RectangleShape sp(Vector2f(w, h));
        Texture tx;
        tx.loadFromFile("input/images/Fire.png");
        sp.setTexture(&tx);
        sp.setPosition(x, y);
        win.draw(sp);
    }
};

*/

class sword_attack : public Entity{
    public:
    long double *ox, *oy, shiftx, shifty, T, dam;

    sword_attack(string Type, long double damage, long double *XX, long double *YY, double X, double Y, double W, double H, double time, bool Dir){
        type = Type;
        dam = damage;
        shiftx=X; shifty=Y;
        ox=XX;
        oy=YY;
        w=W;h=H;
        T = time;
        dir = Dir;
    }

    bool x_other(Entity &o){
        return (x+*ox <= o.x+o.w)&&(o.x <= x+w+*ox)&&(y+*oy <= o.y+o.h)&&(o.y <= y+h+*oy);
    }

    void draw(RenderWindow &win){
        x=shiftx+*ox;
        y=shifty+*oy;
    }

    void upd(float time, Map &mp, list <Entity*> &es, vector <Particle> &par){
        T -= time;
        if(T <= 0)alive = 0;
    }

    void hit(Entity &o){
        o.get_hit(dam, o.x+o.w*!dir, y+h/2);
    }
};

class Bullet:public Entity{
    public:
    RectangleShape sp;
    double ang;
    Bullet(string TP, float X, float Y, float Dx, float Dy, float sz, Color C, float Ang=0){
        w=1;
        h=1;
        type = "bullet";
        type2 = TP;
        sp.setSize(Vector2f(sz, sz));
        sp.setOrigin(sz/2, sz/2);
        sp.setFillColor(C);
        x=X; y=Y; dx=Dx; dy=Dy; ang=Ang;
    }

    void upd(float time, Map &mp, list<Entity*>&es, vector<Particle> &partic){
        x += dx * time;
        y += dy * time;
        sp.rotate(ang * time);
        if(mp.xWalls(IntRect(x, y, 1, 1))){
            alive = 0;
            return;
        }
    }

    void draw(RenderWindow &win){
        sp.setPosition(x, y);
        win.draw(sp);
    }

    void hit(Entity &o){
        o.get_hit(1, o.x+o.w*(dx<0)*2-o.w, 0);
    }

    IntRect getRect(){
        return IntRect(x, y, 1, 1);
    }
};


#endif // ENTITY_H_INCLUDED
