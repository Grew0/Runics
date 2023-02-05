#ifndef FORDRAW_H_INCLUDED
#define FORDRAW_H_INCLUDED

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

struct ParticleShaderClass{
    Shader sh;
    ParticleShaderClass(){
        sh.loadFromFile("input/images/Light.frag", Shader::Fragment);
    };
}part_sh;

class Particle{
public:
    Texture tx;
    RectangleShape sp;
    Color c;
    double dx, dy, t, ang, ddx=0, ddy=0;
    float r, g, b, a;
    bool SH=1;
    Particle(double x, double y, double Dx, double Dy, double side, double time, double angle, Color now, Color next, bool sh=0){
        SH=sh;
        sp.setSize(Vector2f(side, side));
        sp.setOrigin(side / 2, side / 2);
        sp.setPosition(x, y);
        r = now.r;
        g = now.g;
        b = now.b;
        a = now.a;
        c = next;
        ang = angle;
        t = time;
        dx = Dx; dy = Dy;
        if(sh){
            Image im;
            im.create(side, side);
            tx.loadFromImage(im);
            sp.setTexture(&tx);
        }
    }

    bool is_over(){
        return t <= 0;
    }

    void draw(RenderWindow & win){
        if(is_over())
            return;
        if(SH){
            part_sh.sh.setUniform("col", Glsl::Vec4(Color(int(r), int(g), int(b), int(a))));
            win.draw(sp, &part_sh.sh);
        }else{
            sp.setFillColor(Color(int(r), int(g), int(b), int(a)));
            win.draw(sp);
        }
    }

    void upd(float time){
        if(is_over())
            return;
        r += (c.r - r) * time / t;
        g += (c.g - g) * time / t;
        b += (c.b - b) * time / t;
        a += (c.a - a) * time / t;
        ///cout<<int(r)<<'\t'<<int(g)<<'\t'<<int(b)<<'\n';
        t -= time;
        sp.rotate(ang * time);
        sp.move(dx * time, dy * time);
        dx += ddx * time;
        dy += ddy * time;

    }
};

class Firefly{
public:
    Texture tx;
    Sprite sp;
    double dx=0, dy=0, ddx=0, ddy=0;
    double t=-1, spt=0.1, spxy=100;
    double bl;

    Firefly(double x, double y, int side = 10){
        {
            RenderTexture twin;
            twin.create(100, 100);
            twin.clear(Color(0,0,0,0));
            tx.create(100, 100);
            sp.setTexture(tx);
            bl = 0.8 + (rand() % 2001) / 10000;
            part_sh.sh.setUniform("col", Glsl::Vec4(1, 1, bl, 1));
            twin.draw(sp, &part_sh.sh);
            twin.display();
            tx = twin.getTexture();
        }
        sp.setScale(side/100.0, side/100.0);
        sp.setTexture(tx);
        sp.setPosition(x, y);
        sp.setOrigin(side/2, side/2);
        spt += (rand() % 1001) / 1000;
    }

    void upd(float time, IntRect v){
        ddx += (rand() % 3 - 1) * spxy * time;
        ddy += (rand() % 3 - 1) * spxy * time;
        dx += ddx * time;
        dy += ddy * time;
        limit(dx, 3);
        limit(dy, 3);
        limit(ddx, 2);
        limit(ddy, 2);
        sp.move(dx * time, dy * time);
        t += spt * time;

        while(sp.getPosition().y < v.top)sp.move(0, v.height);
        while(sp.getPosition().x < v.left)sp.move(v.width, 0);
        while(sp.getPosition().y > v.top + v.height)sp.move(0, -v.height);
        while(sp.getPosition().x > v.left + v.width)sp.move(-v.width, 0);

    }

    void draw(RenderWindow &win){
        sp.setTexture(tx);
        sp.setColor(Color(255,255,255,255*(1-t*t)));
        win.draw(sp);

    }

    void limit(double &val, double lim){
        val = min(lim, max(val, -lim));
    }

};

class indicator{
public:
    double x, y, w, h, t;
    Color c1, c2;
    indicator(double X,double  Y,double  W,double  H, Color C1, Color C2, double T){
        x=X;y=Y;w=W;h=H;c1=C1;c2=C2;t=T;
    }

    void draw(RenderWindow &win, double val, double mxval, bool use_view=1){
        RectangleShape sp(Vector2f(w, h));
        Vector2f cen = (use_view? win.getView().getCenter() : Vector2f(0, 0));
        if(use_view){
            cen.x -= win.getView().getSize().x / 2;
            cen.y -= win.getView().getSize().y / 2;
        }
        sp.setPosition(x+cen.x, y+cen.y);
        sp.setFillColor(c1);
        win.draw(sp);
        if(val > mxval) val = mxval;
        sp.setSize(Vector2f(w - t*2, h-t*2));
        sp.setPosition(x+cen.x+t, y+cen.y+t);
        sp.setFillColor(Color(c2.r/2, c2.g/2, c2.b/2));
        win.draw(sp);

        sp.setSize(Vector2f((w-t*2) * val / mxval, h-t*2));
        sp.setFillColor(c2);
        win.draw(sp);

    }

};

struct DarkShaderClass{
    Shader sh;
    DarkShaderClass(){
        sh.loadFromFile("input/dark.frag", Shader::Fragment);
    }
}DarkShader;
#endif // FORDRAW_H_INCLUDED
