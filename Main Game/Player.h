#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Animation.h"
#include "fordraw.h"
#include "Pause.h"
#include "audio.h"
#include <SFML/Audio.hpp>


class Player : public Entity{
public:
    Vector2f stableposition;

    struct{
        int x, y;
        string room;
    }checkpoint;

    AnimManager anim = AnimManager("input/Player/pixel hero.png", "input/Player/Anims/anim.txt");

    ///Anim anim = Anim("input/Hero.png", 256, 0, 256, 256, 6, 10, Vector2f(-50, -15));

    Ability jump = Ability(1, 0, -350), fire = Ability(0.5, 0.1);
    Ability punch = Ability(0.1, 0, 10), shield = Ability(1, 3);
    Ability STOM = Ability(0.01, -1, 0, 1);
    Param hp = Param(8), stamina = Param(100), mana = Param(100), defense = Param(12);
    ///unsigned const int key_r=72, key_l=71, key_up=73, key_down=74, key_j=25,
    ///key_dash=2, key_run=38, key_mana=0; /// Keys

    Key key_r=Key(3), key_l=Key(0), key_up=Key(22), key_down=Key(18),
    key_j=Key(9), key_dash=Key(20), key_run=Key(38), key_mana=Key(8),
    key_healing=Key(24), key_punch=Key(7), key_shield=Key(10); /// Keys

    double stop, limstop;
    Ability resist = Ability(2), healing = Ability(0.5, 10);

    Texture hptx;
    Sprite sphp;
    float adcontorl=0;


    Player(){
        x=1500; y=1500;
        w=18; h=52;

        resist.time = resist.mxtime;

        limstop = 250;
        speed=2500;
        stop = speed / 2;

        type = "pl";

        punch.manacost = 1;
        punch.staminacost = 10;

        hptx.loadFromFile("input/images/hpsnew.png");
        sphp.setTexture(hptx);
    }

    void get_hit(double dam, double X, double Y){
        if(!resist.is_over())return;
        audio.play_sound("player_hit");
        if(shield.was && (X > x+w/2) == dir){
            ///resist.restart();
            shield.staminacost = shield.mxtime * dam;
            shield.manacost = shield.mxtime * dam * 0.1;
            if(shield.cast(mana.val, stamina.val)){
                return;
            }
        }
        resist.restart();
        jump.was = 0;
        dx += (-2*dir+1) * speed / 4.0;
        dy -= 100;
        defense.val -= dam;
        if(defense.val < 0){
            hp.change(defense.val);
            defense.val = 0;
        }
    }

    void recover(){
        hp.val = hp.mxval;
        if(healing.unlock)
            defense.val = defense.mxval;
        else
            defense.val = 0;
        mana.val = mana.mxval;
        resist.time = resist.mxtime;
        dx = dy = 0;
    }

    void draw_interface(RenderWindow &win, float length_of_dark_rects){
        double hp_size = 25;
        double X, Y;
        {
            View v = win.getView();
            X = v.getCenter().x - v.getSize().x / 2 + length_of_dark_rects;
            Y = v.getCenter().y - v.getSize().y / 2;
        }
        sphp.setPosition(X+2, Y+2);
        for(int i=0;i<hp.mxval/4;i++){
            sphp.setTextureRect(IntRect((4-max(min<int>(hp.val-i*4, 4), 0)) * hp_size, 0, hp_size, hp_size));
            win.draw(sphp);
            sphp.move(hp_size, 0);
        }
        if(healing.unlock)
        for(int i=0;i<defense.mxval/4;i++){
            sphp.setTextureRect(IntRect((4-max(min<int>(defense.val-i*4, 4), 0)) * hp_size, hp_size, hp_size, hp_size));
            win.draw(sphp);
            sphp.move(hp_size, 0);
        }
        stamina.show(win, 5+length_of_dark_rects, 37, 120, 7, 2, Color(150, 255, 0));
        mana.show   (win, 5+length_of_dark_rects, 52, 100, 7, 2, Color(0, 150, 255));

    }

    void draw(RenderWindow &win){
        if(!resist.is_over())
            if(int(resist.time * (1 + (resist.time > 1.5)*3) * 5) % 2)
                return;

        //draw_entity(win, 0, 255, 0);
        anim.draw(win, x, y, w, !dir);

    }

    void upd(float time, Map &mp, list <Entity*> &es,  vector <Particle> &partic){
        key_j.can = 1;
        if(adcontorl == 0)key_l.can = key_r.can = 1;
        else dx = key_l.can = key_r.can = 0;

        adcontorl = max(adcontorl - time, 0.0f);
        /// Animation choosing
            if(dx == 0){
                anim.setAnim(0);
            }else{
                anim.setAnim(1);
                anim.anims[1].speed = (2 + (key_run.gtk() && stamina.val > 0)) * 3;
            }
            if(!onGround)
                if(dy < 0)
                    anim.setAnim(2);
                else
                    anim.setAnim(3);
            if(punch.was){
                if(onGround){
                    dx = 0;
                }
                anim.setAnim(4);
            }
            if(key_shield.gtk() && shield.unlock){
                if(!shield.was){
                    if(!punch.was){
                        shield.was = 1;
                        shield.val = 1;
                    }
                }else{
                    if(onGround){
                        dx=0;
                        key_l.can = key_r.can = 0;
                    }
                    key_j.can = 0;
                    if(shield.val){
                        anim.setAnim(5);
                        if(anim.is_end()){
                            shield.val = 0;
                            anim.anims[5].nw=0;
                            anim.setAnim(6);
                        }
                    }else anim.setAnim(6);

                }
            }else{
                shield.was = 0;
                shield.val = 1;
            }

            anim.endofchoosing();

            if(anim.choose == 1){
                if(anim.anims[1].speed > 6){
                    audio.play_sound("fast step");
                }else
                    audio.play_sound("step");
            }else{
                audio.pause_sound("step");
                audio.pause_sound("fast step");
            }
        /// Particale system
            if(onGround && dx != 0 && rand() % 1000 < 2 + 10 * (key_run.gtk() && stamina.val > 0)){
                double v0 = -100;
                Particle qwe(x+w/2, y+h, dir*2-1, v0, 2, abs(2*v0/grav), 2,
                         Color(150, 150, 100),
                         Color(100, 100, 150)
                         );
                qwe.ddy = grav;
                partic.push_back(qwe);
            }
        /// control
            /// running
                if(dx > -limstop * (1 + key_run.gtk() * onGround * (stamina.val > 0))){
                    dx -= key_l.gtk() * speed * time;
                    dx = max<double>(dx, -limstop * (1 + key_run.gtk()));
                }
                if(dx < limstop * (1 + key_run.gtk() * onGround * (stamina.val > 0))){
                    dx += key_r.gtk() * speed * time;
                    dx = min<double>(dx, limstop * (1 + key_run.gtk()));
                }

            /// jump
                if(key_j.gtk() && jump.can()){
                    jump.Can = 0;

                    jump.was = 1;
                    dy += jump.val;
                    jump.restart();
                }
                if(!key_j.gtk() && jump.was){
                    jump.was = 0;
                    dy = max<double>(0, dy);
                }

            /// stamina to mana
                if(onGround && key_mana.gtk() && mana.val < mana.mxval){
                    if(STOM.is_over())
                    if(STOM.cast(mana.val, stamina.val)){
                        mana.change(0);
                        stamina.val -= STOM.staminacost;
                        stamina.time = 0;
                        STOM.restart();

                    }
                }
            /// fire
                /*if(GTK(X) && fire.is_over())
                    if(fire.cast(mana.val)){
                        fire.restart();
                        es.insert(es.begin(), new Bullet(x, y, dir));
                    }*/
            /// Punch
                if(anim.choose != 4){
                    if(key_punch.press() && !shield.was){
                        if(punch.cast(mana.val, stamina.val)){
                            audio.play_sound("punch");
                            punch.was = 1;
                            anim.anims[4].nw = 0;
                            float t = anim.anims[4].mx / anim.anims[4].speed;
                            if(dir){
                                es.insert(es.end(), new sword_attack("sapl", 5, &x, &y, 0, -5, 80, h+10, t, dir));
                            }else es.insert(es.end(), new sword_attack("sapl", 5, &x, &y, -80+w, -5, 80, h+10, t, dir));
                        }
                    }
                }else{
                    if(anim.is_end())
                        punch.was = 0;
                }


            /// healing
                if(key_healing.gtk() && healing.is_over() && resist.is_over() && onGround){
                    if(defense.val != defense.mxval)
                    if(healing.cast(mana.val, stamina.val)){
                        healing.restart();
                        defense.change(1);
                    }
                }
        ///logic
            /// Running
                if(dx > stop * time) dx -= stop * time;
                else if(dx < -stop * time) dx += stop * time;
                else dx = 0;
            /// X walls upd
                /// X
                    x += dx*time;
                    if(x_map(mp, x, y)){
                        x -= dx*time;
                        dx = 0;
                    }
                /// Y
                    onGround = 0;
                    y += dy*time;
                    if(x_map(mp, x, y)){
                        if(dy > 0){
                            if(dy > 50)audio.play_sound("fall");
                            onGround = 1;
                        }
                        if(dy < 0){
                            jump.time = jump.mxtime;
                        }
                        y-=dy*time;
                        dy = 0;
                    }

            /// on ground or on wall updates
                jump.Can = 0;
                if(onGround){
                    stableposition = Vector2f(x, y);
                    jump.Can = 1;
                }
            ///stamina
                if(key_run.gtk() && dx != 0 && onGround){
                    stamina.val -= 15 * time;
                    stamina.time = 0.1;
                }
                if(stamina.val < 0){
                    stamina.val = 0;
                    stamina.time = 0.1+time;
                }
                if(stamina.time > 0){
                    stamina.time -= time;
                }else{
                    if(!punch.was && !shield.was)
                        stamina.val += 25 * time;
                    stamina.val = min(stamina.val, stamina.mxval);
                }
            /// Mana

        /// Ability getting
            for(PaperAbil &ps: Main_Map.papers){
                if(ps.is && ps.xPlayer(getRect())){
                    unlock_abil(ps.id);
                }
            }

        /// Other upd functions
            upd_entity(time, mp, es);
            jump.upd(time);
            fire.upd(time);
            anim.upd(time);
            resist.upd(time);
            healing.upd(time);
            punch.upd(time);
            STOM.upd(time);
            ///Key upds
                key_dash.upd();
                key_down.upd();
                key_j.upd();
                key_l.upd();
                key_mana.upd();
                key_r.upd();
                key_run.upd();
                key_up.upd();
                key_punch.upd();
                key_shield.upd();
                key_healing.upd();
    }

    void unlock_abil(string s){
        if(s == "jump"){
            jump.unlock = 1;
            Pause.addPopap(s);
        }else if(s == "heal"){
            healing.unlock = 1;
            Pause.addPopap(s);
        }else if(s == "sword"){
            Pause.addPopap(s);
            punch.unlock = 1;
        }else if(s == "STOM"){
            STOM.unlock = 1;
            Pause.addPopap(s);
        }else if(s == "shield"){
            shield.unlock = 1;
            Pause.addPopap(s);
        }else if(s == "sword&shield"){
            shield.unlock = 1;
            punch.unlock = 1;
            Pause.addPopap("sword");
            Pause.addPopap("shield");
        }
    }

    void save(string room){
        ofstream out;
        out.open("input/Player/save/0.txt");
        out << room << '\n' << int(stableposition.x) << ' ' << int(stableposition.y) << '\n';
        out << checkpoint.x << ' ' << checkpoint.y << ' ' << checkpoint.room << '\n';
        out << hp.val << ' ' << defense.val << '\n';
        out << dir << '\n';
        out << jump.unlock << '\n';
        out << punch.unlock << '\n';
        out << shield.unlock << '\n';
        out << healing.unlock << '\n';
        out << STOM.unlock;
        out.close();
    }

    string load(string file=""){
        string s;
        ifstream in;
        if(file == "ng"){
            in.open("input/Player/save/orig.txt");
        }else
            in.open("input/Player/save/0.txt");
        in >> s >> x >> y;
        in >> checkpoint.x >> checkpoint.y >> checkpoint.room;
        in >> hp.val >> defense.val;
        in >> dir;
        in >> jump.unlock;
        in >> punch.unlock;
        in >> shield.unlock;
        in >> healing.unlock;
        in >> STOM.unlock;
        in.close();
        return s;
    }

    void to_stability(){
        dy = dx = 0;
        x = stableposition.x;
        y = stableposition.y;

    }

    Vector2f getCenter(){
        return Vector2f(x+w/2, y+h/2);
    }

    void check_y(){
        while(Main_Map.xWalls(getRect()))y--;
    }

};


#endif // PLAYER_H_INCLUDED
