#ifndef ABILITY_H_INCLUDED
#define ABILITY_H_INCLUDED

#include <iostream>
#include "fordraw.h"

using namespace std;
using namespace sf;

class Param{
public:
    double val, mxval, time=0;

    Param(double max_value){
        val = max_value;
        mxval = max_value;
    }

    void change(double change_value){
        val = min(mxval, max(change_value + val, 0.0));
    }

    void show(RenderWindow &win, double x, double y, double w, double h, double t, Color c){
        indicator(x, y, w, h, Color::Black, c, t).draw(win, val, mxval);
    }
};


class Ability{
public:
    double val, mxtime, time=0, manacost, staminacost=0;
    bool Can=0, was=0, unlock=0;

    Ability(double Max_time, double mana_cost=0, double Value=0, double Staminacost=0){
        mxtime = Max_time;
        val = Value;
        manacost=mana_cost;
        staminacost=Staminacost;
    }

    void upd(double Time){ time = min(Time+time, mxtime); }
    void restart(){ time = 0; }
    bool is_over(){ return time >= mxtime; }
    bool cast(double &mana, double &stamina){
        if(!unlock)return 0;
        if(mana >= manacost && stamina >= staminacost){
            mana -= manacost;
            stamina -= staminacost;
            return 1;
        }return 0;
    }
    void say(){ cout<<"can\t"<<Can<<"\twas\t"<<was<<"\ttime\t"<<time; }
    bool can(){ return Can & unlock; }
};


#endif // ABILITY_H_INCLUDED
