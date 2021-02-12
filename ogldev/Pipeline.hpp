#pragma once

#include "math_3d.h"

class Pipeline{
    Vector3f scale_;
    Vector3f worldPos_;
    Vector3f rotateInfo_;

    struct{
        float fow;
        float width;
        float height;
        float znear;
        float zfar;
    }perspectiveProjection_;

    struct {
        Vector3f Pos;
        Vector3f Target;
        Vector3f Up;
    } camera_;

    Matrix4f wvpTransformation_;
    Matrix4f worldTransformation_;
public: // == CTORS ==
    Pipeline()
    : scale_( 1.0f, 1.0f, 1.0f )
    , worldPos_( 0.0f, 0.0f, 0.0f )
    , rotateInfo_( 0.0f, 0.0f, 0.0f )
    {}

public: // == Pipeline ==
    void scale( float x, float y, float z ){
        scale_.x = x;
        scale_.y = y;
        scale_.z = z;
    }

    void worldPos( float x, float y, float z ){
        worldPos_.x = x;
        worldPos_.y = y;
        worldPos_.z = z;
    }

    void rotate( float x, float y, float z ){
        rotateInfo_.x = x;
        rotateInfo_.y = y;
        rotateInfo_.z = z;
    }

    const Matrix4f& getWVPTrans();
    const Matrix4f& getWorldTrans();


    void setPerspectiveProjection( float fow, float width, float height, float zNear, float zFar){
        perspectiveProjection_.fow = fow;
        perspectiveProjection_.width = width;
        perspectiveProjection_.height = height;
        perspectiveProjection_.znear = zNear;
        perspectiveProjection_.zfar = zFar;
    }

    void setCamera( Vector3f const& pos, Vector3f const& target, Vector3f const& up ){
        camera_.Pos = pos;
        camera_.Target = target;
        camera_.Up = up;
    }
};


