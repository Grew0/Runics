#include <iostream>
#include <SFML/Graphics.hpp>
#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

using namespace std;
using namespace sf;

class phys{
public:
    double x, y, dx, dy;
};

class entity:public phys{
public:
    double w, h;

    entity(){};
    entity(double X, double Y, double W, double H){
        x = X; y = Y; w = W; h = H;
    };

    void upd(double time){
        x += dx * time;
        y += dy * time;
        return;
    }

    virtual void xother(){}
    virtual void draw(){}
};

class player:public entity{
public:
    player(){}
    player(double X, double Y, double W, double H):entity(X, Y, W, H){}
    draw(RenderWindow &win){
        RectangleShape dr(Vector2f(w, h));
        dr.setPosition(x, y);
        win.draw(dr);
    }
};

#endif // CLASSES_H_INCLUDED
