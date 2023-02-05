#ifndef BOSS1_H_INCLUDED
#define BOSS1_H_INCLUDED

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

struct fog_wall{
    double x, y, h=64;
    bool activ=0;
    bool waspl=0;
    float tm=0;
    Texture tx;
    Sprite sp;

    fog_wall(){}
    fog_wall(float X, float  Y){
        x=X; y=Y;
        tx.loadFromFile("input/map/img/undergroundspike.png");
    }

    float timer = 0, mxtimer=1, speadtimer=5;

    bool upd(Player &p){
        bool frret=0;
        bool f = waspl;
        if(p.x <= x && x <= p.x + p.w && p.y <= y + h && y <= p.y + p.h){
            if(activ){p.x = x - p.w - 1;}
            waspl = 1;
        }
        else waspl = 0;
        if(f && !waspl && x > p.x){
            activ = 1;
            frret=1;
            audio.setMusic("boss");
        }
        if(activ){
            p.stableposition = Vector2f(x+10, y+h);
            timer = min(mxtimer, tm*speadtimer+timer);
        }

        return frret;
    }

    void draw(RenderWindow &win){
        sp.setTexture(tx);
        sp.setPosition(x, y-h*(timer/mxtimer)+h);
        sp.setTextureRect(IntRect(0, 0, 16, 64*timer/mxtimer));
        win.draw(sp);
    }
};

class Boss1: public Enemy{
public:
    fog_wall fog;

    char particle_hit='0';

    struct{
        double x=-1000, y=-1000, w=20, h=70;

        void draw(RenderWindow &win){
            RectangleShape sp(Vector2f(w, h));
            sp.setPosition(x, y);
            sp.setFillColor(Color(255,255,255,150));
            win.draw(sp);
        }

        IntRect getRect(){
            return IntRect(x, y, w, h);
        }
    }_shield;

    bool onGround = 0;

    AnimManager anim = AnimManager("input/Enemys/BOSS1/sprite.png", "input/Enemys/BOSS1/anim.txt");

    enum Atacks{no, kick, jump, fire, wave, shield}Atack;
    int as = 0; /// stage of atack
    double mxhp=300;
    float before_next_atack=0;
    float bullettimer=0;
    float atack_timer=0;

    Boss1(double X, double Y, double Viewoutline):Enemy(x, y){
        fog = fog_wall(886, 368);
        w=43; h=63;
        x=X-w/2;y=Y-h;
        Atack = Atacks::no;
        stype = "BOSS";
        hp = mxhp;
        speed = 150;
    }

    int getstate(){
        Pause.addPopap("titles");
        audio.setMusic("end");
        if(alive) return 0;
        return -2;
    }

    void draw(RenderWindow &win){
        anim.draw(win, x, y, w, dir);
        //_shield.draw(win);
    }

    void draw_interface(RenderWindow &win, double viewoutline){
        View v=win.getView();
        double inh=10;
        double X=viewoutline+inh, Y=v.getSize().y - inh*2;
        indicator(X, Y, v.getSize().x-viewoutline*2-inh*2, inh, Color(0,0,0), Color(255,0,0), inh/4).draw(win, hp, mxhp, 1);
        fog.draw(win);
    }

    void upd(float time, Map &mp, list<Entity*>&es, vector<Particle> &partic){


        /// ANIM CHOOSING
            IntRect rc;
            switch(Atack){
                case Atacks::no:
                    if(dx == 0 || !onGround)anim.setAnim(0);
                    else anim.setAnim(1);

                break; /// No atakc end
                case Atacks::kick:
                    switch(anim.choose){
                        case 2:
                            if(anim.is_end())anim.setAnim(3);
                        break;
                        double t0;
                        case 3:
                            if(anim.is_end()){
                                anim.setAnim(4);
                                t0 = anim.anims[4].mx / anim.anims[4].speed;
                                rc = anim.anims[4].fr[3];
                                audio.play_sound("punch2");
                                es.insert(es.end(), new sword_attack("sab", 4, &x, &y, (w-rc.width-w)*dir+w, h-rc.height, rc.width-w, rc.height, t0, !dir));

                            }
                        break;
                        case 4:
                            if(anim.is_end()){
                                anim.setAnim(0);
                                atack_restart();
                            }
                        break;
                        default:
                            if(as == 1)
                                anim.setAnim(2);
                            else if(dx == 0 || !onGround)anim.setAnim(0);
                            else anim.setAnim(1);
                        break;
                    }
                break; /// Kick end
                case Atacks::wave:
                    switch(anim.choose){
                        case 10:
                            if(anim.is_end())anim.setAnim(11);
                        break;
                        case 11:
                            if(anim.is_end())anim.setAnim(12);
                        break;
                        case 12:
                            if(anim.is_end()){
                                audio.play_sound("bossshield");
                                Main_Map.unspkwave.push_back(underspikewave(spikesnum(x+15+w-dir*(30+w)), 0.1, dir));
                                atack_restart();
                                anim.setAnim(0);
                            }
                        break;
                        default:
                            if(as == 1){
                                if(anim.anims[1].speed < 0)
                                    anim.anims[1].speed *= -1;
                                anim.setAnim(10);
                            }else{
                                if(dx != 0){
                                    anim.setAnim(1);
                                    if(anim.anims[1].speed > 0)
                                        anim.anims[1].speed *= -1;
                                }

                            }
                        break;
                    }
                break; /// Wave end
                case Atacks::fire:
                    switch(anim.choose){
                        case 7:
                            anim.setAnim(8);
                            atack_timer = (rand() % 101) / 200.0 + 0.5;
                            audio.play_sound("boss_shot");
                        break;
                        case 8:
                            if(onGround){
                                if(bullettimer<=0){
                                    float X=x+70, Y=y+17;
                                    if(dir)X = x - 70 + w;
                                    es.insert(es.end(), new Bullet("en", X, Y, -(dir*2-1)*(200+rand()%100), rand()%5-2,  5, Color(255,255,0), 10+rand()%181));
                                    bullettimer += (rand()%101)/1500.0;
                                    float partang = ((rand()%1001)/500.0-1) * 1.57, partsp = (50+rand()%10);
                                    partic.push_back(Particle(X, Y, -(dir*2-1)*cos(partang)*partsp, sin(partang)*partsp, 3, 1.5, 0, Color(255,100,0), Color::Black));
                                    partic.back().ddx = partic.back().dx / partic.back().t;
                                }
                                bullettimer -= time;
                                atack_timer -= time;
                                if(atack_timer <= 0){
                                    audio.stop_sound("boss_shot");
                                    atack_restart();
                                }
                            }
                        break;
                        default:
                            if(as == 1){
                                if(anim.anims[1].speed < 0)
                                    anim.anims[1].speed *= -1;
                                anim.setAnim(7);
                            }else{
                                if(dx != 0){
                                    anim.setAnim(1);
                                    if(anim.anims[1].speed > 0)
                                        anim.anims[1].speed *= -1;
                                }
                            }
                        break;
                    }
                break; /// Fire end
                case Atacks::jump:
                    if(onGround){
                        if(anim.choose == 9){
                            if(anim.is_end()){
                                anim.setAnim(0);
                                dy = -440;
                                onGround = 0;
                            }
                        }else{
                            anim.setAnim(9);
                            anim.anims[9].nw = 0;
                        }
                    }else{
                        anim.setAnim(0);

                    }
                break; /// Jump end
                case Atacks::shield:
                    switch(anim.choose){
                        case 5:
                            if(anim.is_end())anim.setAnim(6);
                        break;
                        case 6:
                            if(dir){
                                _shield.x = x - 15;
                                _shield.y = y - 10;
                            }else{
                                _shield.x = x + w - 3;
                                _shield.y = y - 10;
                            }
                        break;
                        default:
                            anim.setAnim(5);
                        break;
                    }
                break; /// Shield end
            }
            //cout<<Atacks(Atack)<<'\n';
        /// END

        /// XY movements

        dy += grav*time;

        y += dy*time;
        onGround = 0;
        if(Main_Map.xWalls(getRect())){
            y -= dy*time;
            if(dy > 0){
                onGround = 1;
                if(dy > 400 && Atack == Atacks::jump){
                    audio.play_sound("fall");
                    for(UnderSpikes &q: Main_Map.under_spikes)
                        q.pw = -1;
                    atack_restart();
                }
            }
            dy=0;
        }

        x += dx * time;
        if(Main_Map.xWalls(getRect())){
            x -= dx*time;
            dx = 0;
            if(as == 0)
                switch(Atack){
                    case Atacks::fire:
                    case Atacks::wave:
                    case Atacks::kick:
                        as = 1;
                    break;
                }
        }

        /// OTHER UPDs
            resist.val = max(0.0, resist.val-time);

            anim.upd(time);

            before_next_atack -= time;
            if(before_next_atack < 0 /*|| Atack==Atacks::no*/){
                //Atack = Atacks(rand() % 5 + 1);
                atack_choose();
                before_next_atack = 100;
            }

            fog.tm = time;

            if(particle_hit != '0'){
                for(int i=100;i-->0;){
                    double angle = (rand()%1001)/1000.0*3.14, speed_of = 250*(rand()%1001)/1000.0;
                    partic.push_back(
                                    Particle(x+w/2, y+h/5+rand()%int(h*3/5), sin(angle)*speed_of*(particle_hit=='l'?(-1):(1)), cos(angle)*speed_of,
                                    2, (rand()%101)/100.0, 0, Color::Yellow, Color(50,50,0))
                                    );
                }
                particle_hit = '0';
            }

        }

    void atack(Player &p){
        /// Fog upd
            if(fog.upd(p)){
                atack_choose();
                before_next_atack = 100;
            }
        ///

        if(Atack != Atacks::shield)
        dir = p.x - x < 0;
        resist.mx = p.anim.anims[4].mx / p.anim.anims[4].speed;

        switch(Atack){
            case Atacks::kick:
                if(as == 0){
                    if(abs(p.x+p.w/2 - x - w/2) < w/2+p.w/2+25){
                        dx = 0;
                        as = 1;
                    }else
                        dx = (-2*dir+1) * speed;
                }
            break;
            case Atacks::fire:
            case Atacks::wave:
                if(as == 0){
                    if(abs(p.x+p.w/2 - x - w/2) > 200){
                        dx = 0;
                        as = 1;
                    }else
                        dx = (2*dir-1) * speed;
                }

            break;
        }
        if(Main_Map.under_spikes[0].dm == 1){
            for(auto &i:Main_Map.under_spikes)i.dm=4;
        }
        if(_shield.getRect().intersects(p.getRect())){
            p.get_hit(4, p.x + p.w * !p.dir, 0);
        }
    }

    void atack_choose(bool needagain=1){
        if(!fog.activ)return;
        Atacks previousa=Atack;
        Atack = Atacks(1+rand()%5);
        if(previousa == Atack && needagain || Atack == Atacks::no)atack_choose();
    }

    void atack_restart(){
        as = 0;
        anim.main_reset();
        Atack = Atacks::no;
        before_next_atack = (1+(rand() % 101 / 100.0) + (hp)/mxhp - 1);
        _shield.y = -1000;
    }

    int spikesnum(double X){
        return (X-Main_Map.under_spikes[0].x+Main_Map.under_spikes[0].w/2)/Main_Map.under_spikes[0].w;
    }

    void get_hit(double dam, double X, double Y){
         if(anim.choose == 6){
            if((X<x+w/2)^dir){
                audio.play_sound("enemy_hit");
                Enemy::get_hit(dam*2, X, Y);
                atack_restart();
            }else{
                audio.play_sound("bossshield");
                resist.val = resist.mx;
                atack_restart();
                Atack = Atacks::kick;
                as = 1;
                anim.setAnim(3);
            }
        }else{
            audio.play_sound("enemy_hit");
            if(resist.val==0)particle_hit = 'l' + (X<x+w/2)*10;
            Enemy::get_hit(dam, X, Y);
        }
    }


};



#endif // BOSS1_H_INCLUDED
