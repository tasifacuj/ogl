#include "Camera.hpp"

#include <iostream>
#include <GL/freeglut.h>

namespace{

const float StepScale = 0.1f;
const int MARGIN = 10;
}



Camera::Camera( int w, int h )
: pos_( 0.0f, 0.0f, 0.0f )
, target_( 0.0f, 0.0f, 1.0f )
, up_( 0.0f, 1.0f, 0.0f )
, width_( w )
, height_( h )
, angleH_( 0.0f )
, angleV_( 0.0f )
, onUpperEdge_( false )
, onLowerEdge_( false )
, onLeftEdge_( false )
, onRightEdge_( false ){
    target_.Normalize();
    init();
}

Camera::Camera(int w, int h, const Vector3f &pos, const Vector3f &target, const Vector3f &up)
: pos_( pos )
, target_( target )
, up_( up )
, width_( w )
, height_( h )
, angleH_( 0.0f )
, angleV_( 0.0f )
, onUpperEdge_( false )
, onLowerEdge_( false )
, onLeftEdge_( false )
, onRightEdge_( false ){
    target_.Normalize();
    up_.Normalize();
    init();
}

void Camera::init(){
    Vector3f hTarget( target_.x, 0.0f, target_.z );
    hTarget.Normalize();

    if( hTarget.z >= 0.0f ){
        if( hTarget.x >= 0.0f )
            angleH_ = 360.0f - ToDegree( asin( hTarget.z ) );
        else
            angleH_ = 180.0f + ToDegree( asin( hTarget.z ) );
    }else{
        if( hTarget.x >= 0.0f )
            angleH_ = ToDegree( asin( -hTarget.z ) );
        else
            angleH_ = 90.0f + ToDegree( asin( -hTarget.z ) );
    }

    angleV_ = -ToDegree( asin( target_.y ) );
    onUpperEdge_ = false;
    onLowerEdge_ = false;
    onLeftEdge_ = false;
    onRightEdge_ = false;
    mousePos_.x = width_ / 2;
    mousePos_.y = height_ / 2;

    glutWarpPointer( mousePos_.x, mousePos_.y );
}

bool Camera::onKeyboard(int key){
    bool ret = false;

    switch ( key ) {
    case GLUT_KEY_UP:
        pos_ += ( target_ * StepScale );
        ret = true;
        break;
    case GLUT_KEY_DOWN:
        pos_ -= ( target_ * StepScale );
        ret = true;
    break;
    case GLUT_KEY_LEFT:
    {
        Vector3f left = target_.Cross( up_ );
        left.Normalize();
        left *= StepScale;
        pos_ += left;
        ret = true;
    }
    break;
    case GLUT_KEY_RIGHT:
    {
        Vector3f right = up_.Cross( target_ );
        right.Normalize();
        right *= StepScale;
        pos_ += right;
        ret = true;
    }
    break;
    default:
        break;
    }

    return ret;
}

void Camera::onMouse(int x, int y){
    const int deltaX = x - mousePos_.x;
    const int deltaY = y - mousePos_.y;

//    std::cout << "deltaX = " << deltaX << ", deltaY = " << deltaY << std::endl;

    mousePos_.x = x;
    mousePos_.y = y;

    angleH_ += ( float )deltaX / 10.0f;
    angleV_ += ( float )deltaY / 10.0f;

    if( 0 == deltaX ){
        if( x <= MARGIN )
            onLeftEdge_ = true;
        else if( x >= ( width_ - MARGIN ) )
            onRightEdge_ = true;
    }else{
        onLeftEdge_ = false;
        onRightEdge_ = false;
    }

    if( 0 == deltaY ){
        if( y <= MARGIN )
            onUpperEdge_ = true;
        else if( y >= ( height_ - MARGIN ) )
            onLowerEdge_ = true;
    }else{
        onUpperEdge_ = false;
        onLowerEdge_ = false;
    }

    update();
}

void Camera::onRender(){
    bool shouldUpdate = false;

    if( onLeftEdge_ ){
        angleH_ -= 2.0f;
        shouldUpdate = true;
    }else if( onRightEdge_ ){
        angleH_ += 2.0f;
        shouldUpdate = true;
    }

    if( onUpperEdge_ ){
        if( angleV_ > -90.0f ){
            angleV_ -= 0.1f;
            shouldUpdate = true;
        }
    }else if( onLowerEdge_ ){
        if( angleV_ < 90.0f ){
            angleV_ += 0.1f;
            shouldUpdate = true;
        }
    }

    if( shouldUpdate )
        update();
}

void Camera::update(){
    Vector3f vaxis( 0.0f, 1.0f, 0.0f );
    // rotate view vector by orizontal angle around y.
    Vector3f view( 1.0f, 0.0f, 0.0f );
    view.Rotate( angleH_, vaxis );
    view.Normalize();

    // rotate view vector by the vertical angle around horizontal axis.
    Vector3f haxis = vaxis.Cross( view );
    haxis.Normalize();
    view.Rotate( angleV_, haxis );

    target_ = view;
    target_.Normalize();

    up_ = target_.Cross( haxis );
    up_.Normalize();
}
