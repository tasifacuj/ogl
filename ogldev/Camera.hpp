#pragma once
#include "math_3d.h"


class Camera{
    Vector3f pos_;
    Vector3f target_;
    Vector3f up_;

    int width_;
    int height_;

    float angleH_;
    float angleV_;

    bool onUpperEdge_;
    bool onLowerEdge_;
    bool onLeftEdge_;
    bool onRightEdge_;

    Vector2i mousePos_;
public:
    Camera( int w, int h );

    Camera( int w, int h, Vector3f const& pos, Vector3f const& target, Vector3f const& up );

    Vector3f const& getPos()const{
        return pos_;
    }

    Vector3f const& getTarget()const{
        return target_;
    }

    Vector3f const& getUp()const{
        return up_;
    }

    bool onKeyboard( int key );
    void onMouse( int x, int y );
    void onRender();
private:
    void init();
    void update();
};

