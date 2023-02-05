#include <SFML/Graphics.hpp>
#include "Entity.h"
#include <list>
#include "Animation.h"
#include "fordraw.h"
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Enemys.h"
#include "Pause.h"
#include "boss1.h"
#include "audio.h"
#include <fstream>


using namespace sf;
using namespace std;

#define GTK(key)   Keyboard::isKeyPressed(Keyboard:: key )

vector <int> mapstates;
float length_of_dark_rects;



void global_save(Player &p){
    Main_Map.save(mapstates);
    p.save(Main_Map.file_of_room);
}

void draw(RenderWindow &win, list <Entity*> &es, Player &p, vector <Particle> &partic);

void load_map(RenderWindow &win, string file, list <Entity*> &es, Player &p, vector <Particle> &partic, bool fromgate=0, int gatenum = 0){
    /// Last Draw
        draw(win, es, p, partic);
    ///


    partic.clear();

    es.resize(0);
    es.insert(es.begin(), &p);



    vector <tuple<string, long, long, int> > for_ens;



    Main_Map.load(file, for_ens, mapstates);
    for(auto &i:for_ens){
        string s = get<0>(i), S, S1;
        {
            int i=-1;
            while(++i < s.size()){
                if(s[i] == '_')break;
                S += s[i];
            }
            i++;
            while(i<s.size())S1 += s[i++];
        }
        long x=get<1>(i), y=get<2>(i);
        if(s == "Player"){
            p.x = x;
            p.y = y - p.h - 1;
        }else if(s == "Enep"){
            EnP * en = new EnP(x, y);
            en->y -= en->h+1;
            en->x -= en->w / 2;
            es.insert(es.end(), en);
        }else if(S == "Paper"){
            Main_Map.papers.push_back(PaperAbil(s, x, y, get<3>(i)));
            continue;
        }else if(S == "Spear"){
            es.insert(es.end(), new Spear(x, y));
        }else if(S == "Tablet"){
            Main_Map.tablets.push_back(Tablet(s, x, y));
            continue;
        }else if(s == "UnderSpike"){
            Main_Map.under_spikes.push_back(UnderSpikes(x, y));
            continue;
        }else if(S == "UnSpkA"){
            Main_Map.unspkwvacts.push_back(unspwaveactivator(S1));
            continue;
        }else if(s == "boss1"){
            es.insert(es.end(), new Boss1(x, y, length_of_dark_rects));
        }
        (*--es.end())->index_in_map = get<3>(i);

    }
    if(fromgate){
        Main_Map.get_exit(p.x, p.y, p.w, p.h, gatenum);
    }

    p.stableposition = Vector2f(p.x, p.y);
    global_save(p);
    if(fromgate)
        BlackSkreen.reset(0.5, 0.1, &win);
    return;
}

void draw(RenderWindow &win, list <Entity*> &es, Player &p, vector <Particle> &partic){
    win.clear();
    View view = win.getView();
    view.setCenter(int(p.getCenter().x*10)/10.0, int(p.getCenter().y*10)/10.0);
    Main_Map.view_redactor(view, length_of_dark_rects);
    win.setView(view);
    /// Map Back
        Main_Map.draw_back_ground(win);
    /// Particle
        for(auto &i: partic)i.draw(win);
    /// Map Middle
        Main_Map.draw_middle_ground(win);
    /// Entitys
        for(Entity* i: es){
            if(i->type != "pl")
                i->draw(win);
        }

    ///Dark Texture
        {
            int x = view.getCenter().x - view.getSize().x/2;
            int y = view.getCenter().y - view.getSize().y/2;
            int w=view.getSize().x, h=view.getSize().y;
            Texture darktx;
            darktx.create(w + 50, h + 50);
            Sprite darksp(darktx);
            darksp.setPosition(x-25, y-25);

            DarkShader.sh.setUniform("pl", p.getCenter());
            DarkShader.sh.setUniform("xy", darksp.getPosition());
            DarkShader.sh.setUniform("wh", Vector2f(darktx.getSize()));
            DarkShader.sh.setUniform("radius", h*1.0f);
            win.draw(darksp, &DarkShader.sh);
        }


    /// Map Front
        Main_Map.draw_front_ground(win);
    /// Player
        p.draw(win);
        p.draw_interface(win, length_of_dark_rects);
    /// Boss interface;
        for(auto *i : es)
            if(i->stype == "BOSS"){
                dynamic_cast <Boss1*> (i)->draw_interface(win, length_of_dark_rects);
            }
    /// Map Advice
        Main_Map.draw_advice(p.getCenter().x, p.getCenter().y, p.onGround, win);
    /// Black screen
        BlackSkreen.draw(win);

    /// Black rects view
    {
        RectangleShape blacksp(Vector2f(length_of_dark_rects, win.getView().getSize().y));
        Vector2f position_of_blacksp = win.getView().getCenter();
        blacksp.setFillColor(Color::Black);
        position_of_blacksp.x -= win.getView().getSize().x/2;
        position_of_blacksp.y -= win.getView().getSize().y/2;
        blacksp.setPosition(position_of_blacksp);
        win.draw(blacksp);
        position_of_blacksp.x += win.getView().getSize().x - length_of_dark_rects;
        blacksp.setPosition(position_of_blacksp);
        win.draw(blacksp);

    }

    /// Time experiments
        if(Pause.is)
            Pause.draw(win);
    ///

    win.display();
}

void NewGame(RenderWindow &win, list <Entity*> &es, Player &p, vector <Particle> &partic){
    Main_Map.newgame();
    load_map(win, p.load("ng"), es, p, partic);
    p.check_y();
    p.recover();
}

void upd(RenderWindow &win, float time, list <Entity*> &es, Player &p, vector <Particle> &partic){
    /// Particle
        for(long i=partic.size()-1;i>=0;i--){
            partic[i].upd(time);
            if(partic[i].is_over()){
                swap(partic.back(), partic[i]);
                partic.pop_back();
            }
        }
    /// end
    /// Player death
        if(p.hp.val == 0){
            Main_Map.clear_map();
            load_map(win, p.checkpoint.room, es, p, partic);
            p.x = p.checkpoint.x;
            p.y = p.checkpoint.y;
            p.recover();
            global_save(p);
            audio.setMusic("level");
        }

    /// end
    /// Map
        View view = win.getView();
        Main_Map.upd(time, partic, view);
        /// Player X rest Crystals
            for(auto i:Main_Map.rest){
                vector <Particle> partic_for_future;
                if(i.activate(p.key_up.press(), IntRect(p.x, p.y, p.w, p.h), partic_for_future)){
                    Main_Map.clear_map();
                    load_map(win, Main_Map.file_of_room, es, p, partic);
                    partic = partic_for_future;
                    p.recover();
                    global_save(p);
                    p.checkpoint = {i.x, i.y-p.h-1, Main_Map.file_of_room};
                    audio.play_sound("restcrystal");
                    break;
                }
            }
        /// end
        /// Player X tablets
            for(Tablet &i:Main_Map.tablets){
                if(p.key_up.press() && p.onGround)
                if(i.activate(p.getRect())){
                    Pause.addPopap(i.file);
                }
            }
        /// end

        /// Player X gates
            {
                string file;
                int num;
                if(Main_Map.x_gate(p.getRect(), file, num, p.onGround & p.key_up.press())){
                    p.adcontorl = 0.5;
                    load_map(win, file, es, p, partic, 1, num);
                }
            }
        /// end
        /// Player X spikes
            {
                int damage=0;
                if(Main_Map.spikes.xPlayer(damage, p.getRect())){
                    p.get_hit(damage, p.x + p.w * !p.dir, 0);
                    p.to_stability();
                }
                for(UnderSpikes &i: Main_Map.under_spikes){
                    if(i.xpl(IntRect(p.x, p.y, p.w, p.h))){
                        if(p.resist.is_over()){
                            if(p.defense.val == 0)
                                p.hp.change(-i.dm);
                            else
                                p.defense.change(-i.dm);
                            p.resist.time = 0.1;
                        }
                    }
                }
            }
        /// end
    /// end
    /// Entity
        for(list <Entity*>::iterator it = es.begin(); it != es.end(); it ++){
                (*it)->upd(time, Main_Map, es, partic);
                if(!(*it)->alive){
                    if((*it)->index_in_map >= 0)
                        mapstates[(*it)->index_in_map] = (*it)->getstate();
                    es.erase(it);
                }
                if((*it)->type == "ene"){
                    Enemy* enem = dynamic_cast<Enemy*>(*it);
                    enem->atack(p);
                }
        }
    /// end
    /// Dynamics contacts
        for(auto it = es.begin(); it != es.end(); it++){
            if((*it)->type == "sapl"){
                Main_Map.for_br(IntRect((*it)->x, (*it)->y, (*it)->w, (*it)->h), partic);
            }
        for(auto jt = es.begin(); jt != es.end(); jt++){
            if(it == jt)continue;
            if((*it)->x_other(**jt)){
                if((**it).type == "pl"){
                    if((**jt).type == "ene"){
                        if((**jt).type2 == "contact"){
                            (**jt).hit(**it);
                            if(p.shield.was){
                                Enemy* jte = dynamic_cast<Enemy*>(*jt);
                                jte->take_block(p.x + p.w * p.dir, p.dir);
                            }
                        }

                    }
                    if((**jt).type == "sab"){
                        (**jt).hit(**it);
                        (**jt).alive = 0;
                    }
                    if((**jt).type == "bullet" /*&& (**jt).type2 == "en"*/){
                        (**jt).hit(**it);
                        (**jt).alive = 0;
                    }

                }
                if((*it)->type == "ene" && (*jt)->type == "sapl"){
                    (**jt).hit(**it);
                    if((**it).stype != "BOSS")
                    audio.play_sound("enemy_hit");
                }
            }

        }
        }
    /// end
    /// Black screen
        BlackSkreen.upd(time);
    /// end
    if(Pause.k.press()){
        Pause.setPause(win, 0);
    }

}

int main(){


    ///ƒÀﬂ «¿√–”« » Œ–»√»Õ¿À‹Õ€’ —Œ’–¿Õ≈Õ»…   system("xcopy/y .\\input\\map\\lev\\Real_Saves .\\input\\map\\lev\\orig");


    {/*/
    cout<<"Press button\n";
    while(1){
        for(int i=0;i<1000;i++)
            if(GTK(Key(i)))
                cout<<i<<'\n';
    }
    /**/

    /*/
    ifstream in("input/txt/room0.txt");
    int n;
    in>>n>>n;
    while(in>>n){
        cout<<n/4<<' ';
        in>>n;
        cout<<n/4<<' ';
        in>>n;
        cout<<n/4<<' ';
        in>>n;
        cout<<n/4<<'\n';

    }
    return 0;
    /**/}
    RenderWindow win(VideoMode::getDesktopMode(), "SFML window", Style::Fullscreen);
    //RenderWindow win(VideoMode(1000, 630), "SFML window");
    //RenderWindow win(VideoMode(800, 600), "SFML window");
    win.setFramerateLimit(60);
    {

        float width_of_view=600, height_of_view=width_of_view*3/4, coof1 = 3.0*win.getSize().x/win.getSize().y/4.0;
        win.setView(View(FloatRect(0, 0, width_of_view*coof1, height_of_view)));
        length_of_dark_rects = width_of_view*(coof1-1)/2.0;

        Image icon; icon.loadFromFile("input/images/icon.png");
        win.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        win.setMouseCursorVisible(0);

    }
    list <Entity*> es;
    vector <Particle> partic;
    Player p;

    Pause.anim = p.anim.anims[1];

    Clock cl;

    /// Map load
    {
        Main_Map.load_visited_rooms();
        load_map(win, p.load(), es, p, partic);
        p.check_y();
    }

    //Main_Map.under_spikes[0].pw = 1;
    /// Map load end


    /*for(Anim &i: BOSS1.anims){
        i.sp.setScale(2, 2);
        for(auto &j: i.shift){
            j.x *= 2;
            j.y *= 2;
        }
    }*/




    while(win.isOpen()){

        Event event;
        while(win.pollEvent(event))
            if(event.type == Event::Closed)
                win.close();
        float time = cl.restart().asSeconds();
        if(time > 0.5)continue;
        //win.clear(Color(0, 200, 200));

        /// Time experiments
        {
            //if(!Pause.is)
              //  Pause.setPause(win, 2, "0");
        }
        ///end
        if(!Pause.is){
            draw(win, es, p, partic);
            upd(win, time, es, p, partic);
            Pause.check_future(win);
        }else{
            if(Pause.k.press() && Pause.types != 2)Pause.is = 0;
            draw(win, es, p, partic);
            string pauseupdreturn = Pause.upd(win, time);
            if(pauseupdreturn == "NewGame"){
                audio.setMusic("level");
                NewGame(win, es, p, partic);
                Pause.is=0;
                continue;
            }
        }
        Pause.k.upd();
    }
    global_save(p);
    ///Main_Map.clear_map();
    return 0;
}
