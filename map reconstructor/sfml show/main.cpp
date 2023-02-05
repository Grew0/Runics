#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#define GTK(key) Keyboard::isKeyPressed(Keyboard:: key )

using namespace sf;
using namespace std;

int main(){
    RenderWindow win(VideoMode(800, 600), "SFML");
    Clock cl;
    View vvv = win.getView();
    vvv.zoom(1.0/2);
    vvv.setCenter(0,0);
    ifstream in;
    in.open("input.txt");
    vector <string> a;
    {
        string s;
        while(in>>s)a.push_back(s);
    }
    Texture tx;
    tx.loadFromFile("image.png");
    Sprite sp(tx);
    while(win.isOpen()){
        Event event;
        while(win.pollEvent(event))
            if(event.type == Event::Closed)
                win.close();

        float time = cl.restart().asSeconds();
        vvv.move((GTK(D)-GTK(A))*time*100, (GTK(S)-GTK(W))*time*100);
        win.clear(Color::White);
        for(int i=0;i<a.size();i++){
            for(int j=0;j<a[i].size();j++){
                sp.setPosition(16*j, 16*i);
                int qw = a[i][j] - 'a';

                sp.setTextureRect(IntRect((qw%4)*16, (qw/4)*16, 16, 16));
                win.draw(sp);
            }

        }
        win.setView(vvv);
        win.display();
    }
	return 0;
}
