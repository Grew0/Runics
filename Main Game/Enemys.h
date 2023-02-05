#ifndef ENEMYS_H_INCLUDED
#define ENEMYS_H_INCLUDED

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Map.h"
#include "Animation.h"
#include "Ability.h"
#include "fordraw.h"
#include "control.h"
#include "Entity.h"
#include <list>

using namespace sf;
using namespace std;
#define GTK(key)   Keyboard::isKeyPressed(Keyboard:: key )

class Enemy : public Entity{
public:
    struct {double mx=0.5, val=0;} resist;
    double hp=0;
    double damage;
    Enemy(double X, double Y){
        type = "ene";
        type2 = "contact";
        x=X; y=Y;
        w = 100;
        h = 100;
        damage = 4;
    }

    void draw(RenderWindow &win){
        draw_entity(win, 255, 0, 0);
    }

    void hit(Entity &o){
        o.get_hit(damage, x+w/2, y+h/2);
    }

    void get_hit(double dam, double X, double Y){
        if(resist.val > 0)return;
        resist.val = resist.mx;
        hp -= dam;
        if(hp < 0) alive = 0;
    }

    virtual void take_block(long double X, bool dirofblock){
    }

    virtual void atack(Player &p){
    }

};

class EnP:public Enemy{
public:
    Texture tx;
    Sprite sp;
    double dir=0;
    double animt=0, animsp = 7;
    double tm = -1, speed = 20;
    EnP(double X, double Y):Enemy(X, Y){
        w = 20*2; h = 12*2;
        hp = 4;
        x=X;y=Y;
        tx.loadFromFile("input/Enemys/spike.png");
        sp.setTexture(tx);
    }
    void upd(float time, Map &mp, list<Entity*> &es, vector <Particle> &partic){
        resist.val = max(0.0, resist.val - time);
        double pX=0;
        for(Entity *pl: es){
            if(pl->type == "pl"){
                pX = pl->x;
                break;
            }
        }
        dx = dir * speed;
        tm -= time;
        dir += 5.0 / (pX - x) * time * 10;
        if(tm <= 0){
            tm = 0 + (rand() % 10) / 100.0;
            dir += rand()%3-1;
            dir = min(max(-5.0, dir), 5.0);
        }
        if(x_map(mp, x+dx*time, y)){
            dir = !dir;
        }else x += dx*time;
        if(!x_map(mp, x, y+dy*time)){
            y += dy * time;
            dy += time*30000;
        }dy /= 2.0;
        if(hp <= 0)alive = 0;
        animt += animsp * time;
        while(animt >= 4)animt -= 4;
    }
    void take_block(long double X, bool dirofblock){
        if(dirofblock){
            x = X + 12;
        }else{
            x = X - 12 - w;
        }
        dir = 0;
    }

    void draw(RenderWindow &win){
        sp.setPosition(x, y);
        sp.setScale(2, 2);
        sp.setTextureRect(IntRect(int(animt) * 20, 0, 20, 12));
        win.draw(sp);
    }
};

class Spear : public Enemy{
public:
    bool onGround = 0;
    int isAtack = 0;
    float timer = 0, mx_timer = 1;
    float angle = 0;

    AnimManager anim = AnimManager("input/Enemys/spear/spear.png", "input/Enemys/spear/anim.txt");

    Spear(double X, double Y) : Enemy(X, Y){
        IntRect sz = anim.anims[0].fr[0];
        w = sz.width; h = sz.height;
        x += w / 2;
        y -= h+7+anim.anims[0].fr[0].height;
        speed = 300;
        /*anim.setAnim(2);
        /*anim.anims[2].sp.setScale(5, 5);
        for(Vector2f &fr : anim.anims[2].shift){
            fr.x *= 5;
            fr.y *= 5;
        }*/
        hp = 12;
    }

    void draw(RenderWindow &win){
        /*{
            IntRect rect = getRect();
            RectangleShape spr(Vector2f(rect.width, rect.height));
            spr.setPosition(rect.left, rect.top);
            spr.setFillColor(Color(255, 0, 0));
            win.draw(spr);
        }*/

        if(isAtack < 1){
            if(dx * (dir*2-1) < 0)anim.anims[2].speed = -abs(anim.anims[2].speed);
            else anim.anims[2].speed = abs(anim.anims[2].speed);

            if(dir)anim.draw(win, x+w/2+w*0.25, y+h-2, 0, 0);
            else anim.draw(win, x+w/2-w*0.25, y+h-2, 0, 1);

            double &nw = anim.anims[2].nw;
            int mx = anim.anims[2].mx;
            nw += mx / 2;
            if(nw > mx) nw -= mx;

            if(dir)anim.draw(win, x+w/2-w*0.40, y+h-2, 0, 0);
            else anim.draw(win, x+w/2+w*0.40, y+h-2, 0, 1);

            nw -= mx / 2;
            if(nw < 0) nw += mx;
        }
        anim.setAnim(0);
        anim.draw(win, x, y, w, !dir);

        anim.setAnim(2 - (dx == 0));

        /*/Vertex vr[2];
        vr[0] = Vertex(Vector2f(x, y-10), Color(0, 0, 255));
        vr[1] = Vertex(Vector2f(x+cos(angle*(1-timer)*3.14/180.0)*w, y+sin(angle*(1-timer)*3.14/180.0)*w-10), Color(255,0,0));
        win.draw(vr, 2, Lines);/**/
    }

    void upd(float time, Map &mp, list<Entity*> &es, vector <Particle> &partic){
        if(time > 0.5)return;
        //cout<<index_in_map<<'\n';

        resist.val = max(0.0, resist.val - time);

        anim.upd(time);

        if(isAtack != 3)
        if(Main_Map.xWalls(IntRect(x, y, w, h))){
            bool lr[2] = {Main_Map.xWalls(IntRect(x, y-3, 1, 1)), Main_Map.xWalls(IntRect(x+w, y-3, 1, 1))};
            if(lr[0] && !lr[1]){
                x --;
            }else
            if(!lr[0] && lr[1]){
                x ++;
            }else{
                y--;
            }

        }

        switch(isAtack){
            case 1:
                if(rand() % 100 < 10){
                    partic.push_back(Particle(x + rand() % int(w), y, 0, 0, 2, 1, 50, Color(255, 255, 0), Color(255, 255, 0, 0)));
                    partic.back().ddy = grav;
                }
            break;
            case 2:
            case 3:
                if(rand() % 100 < 10){
                    double angleForPartic = (rand() % 1001) / 1000.0 * 6.28, streng = rand() % 11 + 40;
                    double alpha = angle * (1-timer) * 3.14 / 180.0;
                    double X = x + w * cos(alpha) * dir;
                    double Y = y + w * sin(alpha) * dir;
                    partic.push_back(Particle(X, Y, cos(angleForPartic)*streng, sin(angleForPartic)*streng, 2, 1, 500, Color(255, 255, 0), Color(255, 255, 0, 0)));
                }
            break;


        }

        if(isAtack == 3){
            x += dx * time;
            y += dy * time;
            if(Main_Map.xWalls(getRect())){
                x -= dx * time * 2;
                y -= dy * time * 2;
                take_block(0, dx > 0);
            }
            return;
        }

        if(isAtack < 2 || isAtack > 3){
            dy += grav * time;
        }
        if(isAtack != 2)
            y += dy * time;
        onGround = 0;
        if(Main_Map.xWalls(getRect())){
            if(dy > 0)onGround = 1;
            y -= dy * time;
            dy = 0;
            if(isAtack == 3)
                isAtack = 4;
        }

        x += dx * time;
        if(Main_Map.xWalls(getRect())){
            x -= dx * time;
            dx = 0;
            if(isAtack == 3)
                isAtack = 4;
        }

        if(isAtack == 2){
            timer -= time;

            anim.anims[0].sp.setRotation((angle - 180 * !dir) * (1-timer));

            if(dir){
                double alpha = angle * (1-timer+time) * 3.14 / 180.0;
                x -= w * cos(alpha);
                y -= w * sin(alpha);
                alpha = angle * (1-timer) * 3.14 / 180.0;
                x += w * cos(alpha);
                y += w * sin(alpha);

            }

            if(timer < 0){
                isAtack = 3;
            }
        }
        if(onGround && isAtack == 4){
            dx = 0;
            isAtack = 5;
            timer = 1;
        }
        if(isAtack == 5){
            timer -= time;
            if(timer < 0){
                isAtack = 0;
                y -= anim.anims[0].fr[0].height + 7;
            }
        }

    }

    IntRect getRect(){
        switch(isAtack){
            case 0:return IntRect(x, y, w, h + anim.anims[1].fr[0].height - 2);
            case 2:return IntRect(x+dir*cos(angle*(1-timer)*3.14/180.0)*w, y+dir*sin(angle*(1-timer)*3.14/180.0)*w, 5, 5);
            case 3:return IntRect(x+dir*cos(angle*3.14/180.0)*w, y+dir*sin(angle*3.14/180.0)*w, 5, 5);
            default: return IntRect(x, y, w, h);
        }
    }

    void atack(Player &p){
        double X = p.x + p.w / 2 - x - w / 2;
        if(onGround && isAtack < 1){
            speed = p.limstop - 0.5;
            dir = X > 0;
            if(X > 0)
                if(X > 200) dx = speed;
                else dx = -speed;
            else
                if(X < -200) dx = -speed;
                else dx = speed;

            if(abs(abs(X) - 200) < 10){
                dx = 0;
                isAtack = 1;
                dy -= sqrt(2 * 100 * grav);
                onGround = 0;
            }
        }

        if(isAtack == 1 && abs(dy) < 3){
            isAtack = 2;
            dy = 0;
            timer = mx_timer;
        }

        if(isAtack == 2){
            angle = tanh(float(p.y+p.h/2-y-h/2)/(p.x+p.w/2-x-w/2)) * 180 / 3.14;
            if(p.x+p.w/2-x-w/2 < 0)angle += 180;
            if((X > 0) ^ dir){
                dir = !dir;
                angle = 180 - angle;
                Sprite &SP = anim.anims[0].sp;
                SP.setRotation(SP.getRotation() * -1);
            }
        }
        if(isAtack == 3){
            dx = cos(angle * 3.14 / 180.0)*speed*2;
            dy = sin(angle * 3.14 / 180.0)*speed*2;
        }
        if(isAtack == 4){
            anim.anims[0].sp.setRotation(0);
        }
    }

    void take_block(long double X, bool dirofblock){
        if(dirofblock){
            dx = 100;
            dy = -100;
        }else{
            dx = -100;
            dy = -100;
        }
        isAtack = 4;
        anim.anims[0].sp.setRotation(0);

    }

    void get_hit(double dam, double X, double Y){
        Enemy::get_hit(dam, X, Y);
        if(isAtack == 5)y -= anim.anims[0].fr[0].height + 7;
        if(isAtack == 5 || isAtack == 4 || isAtack == 3)
        take_block(X, X<x);
        isAtack = 0;
    }

    void hit(Entity &o){
        o.get_hit(damage, x+w/2, y+h/2);
        take_block(0, o.dir);
    }
};


#endif // ENEMYS_H_INCLUDED
