#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#include <SFML/Graphics.hpp>

class Key{
public:
    unsigned int key;
    bool was=0, is=0;
    bool can=1;
    Key(unsigned int KEY=0){
        key = KEY;
    }

    bool press(){
        return gtk() && !was;
    }

    bool release(){
        return was && !gtk();
    }

    bool gtk(){
        return is;
    }

    void upd(){
        was = is;
        is = Keyboard::isKeyPressed(Keyboard::Key(key)) && can;
    }
};


#endif // CONTROL_H_INCLUDED
