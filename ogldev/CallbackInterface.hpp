#ifndef CALLBACKINTERFACE_HPP
#define CALLBACKINTERFACE_HPP

class CallbackInterface{
public:
    virtual ~CallbackInterface(){}

    virtual void specialKeyboardCB( int key, int x, int y ) = 0;
    virtual void keyboardCB( unsigned char key, int x, int y ) = 0;
    virtual void passiveMouseCB( int x, int y ) = 0;
    virtual void renderSceneCB() = 0;
    virtual void idleCB()  = 0;
};

#endif // CALLBACKINTERFACE_HPP
