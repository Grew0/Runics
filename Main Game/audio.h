#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>

using namespace std;
using namespace sf;

struct AudioManager{
    map <string, SoundBuffer> buf;
    map <string, Sound> sd;
    string namemus="";


    int volm=10, vols=10;

    Music msnw;

    AudioManager(){
        this->push("player_hit", "input/sound/hit.wav");
        this->push("enemy_hit", "input/sound/plswhit.wav");
        this->push("punch", "input/sound/sword.wav");
        this->push("punch2", "input/sound/sword.wav");
        this->push("step", "input/sound/step.wav");
            sd["step"].setLoop(1);
        this->push("fast step", "input/sound/faststep.wav");
            sd["fast step"].setLoop(1);
        this->push("restcrystal", "input/sound/restcrystal.wav");
        this->push("crystalcrash", "input/sound/crystalcrash.wav");
        this->push("boss_shot", "input/sound/boss_shot.wav");
        this->push("fall", "input/sound/fall.wav");
        this->push("bossshield", "input/sound/bossshield.wav");

        setMusic("level");
        //msnw.openFromFile("input/music/Levels.wav");
        msnw.play();
        msnw.setLoop(1);
    }

    void setMusic(string s){
        if(s == namemus)return;
        namemus = s;
        if(s == "boss"){
            msnw.openFromFile("input/music/Boss.wav");
        }else if(s == "level"){
            msnw.openFromFile("input/music/Levels.wav");
        }else if(s == "end"){
            msnw.openFromFile("input/music/Titles.wav");
        }
        msnw.play();
    }

    void push(string name, string file){
        buf[name].loadFromFile(file);
        sd[name] = Sound(buf[name]);
    }

    void setVolMus(int nwVol){
        volm = min(max(nwVol, 0), 10);
        msnw.setVolume(volm * 10);
    }

    void setVolSd(int nwVol){
        vols = min(max(nwVol, 0), 10);
        upd_vol_sd();
    }

    string strVolSd(){
        if(vols == 10)return "10";
        string s="0";
        s[0] += vols;
        return s;
    }

    string strVolMus(){
        if(volm == 10)return "10";
        string s="0";
        s[0] += volm;
        return s;
    }


    void upd_vol_sd(){
        for(auto &i: sd)
            i.second.setVolume(vols*10);
    }

    void play_sound(string name){
        if(sd[name].getStatus() > 0){
            if(!set<string>({"enemy_hit", "step", "fast step"}).count(name))
                sd[name].setBuffer(buf[name]);

            if(!set<string>({"enemy_hit", "step", "fast step"}).count(name))
                sd[name].play();
        }else{
            sd[name].play();
        }
    }

    void stop_sound(string name){ sd[name].stop(); sd[name].setBuffer(buf[name]); }
    void pause_sound(string name){ sd[name].pause(); sd[name].setBuffer(buf[name]); }

} audio;

#endif // AUDIO_H_INCLUDED
