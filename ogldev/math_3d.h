#pragma once

#include <cmath>

#define ToRadian( x ) ( ( x ) * M_PI / 180.0f )
#define ToDegree( x ) ( ( x )  * 180.0f * M_PI )

struct Vector2i
{
    int x;
    int y;
};

struct Vector2f{
    float x_{};
    float y_{};

    Vector2f( float x, float y )
    : x_( x )
    , y_( y ){}
};

struct Vector3f
{
    float x{};
    float y{};
    float z{};

    Vector3f() = default;

    Vector3f( const Vector3f& rhs )
    : x( rhs.x )
    , y( rhs.y )
    , z( rhs.z )
    {}

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f& operator += ( Vector3f const& rhs ){
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vector3f& operator -= ( Vector3f const& rhs ){
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vector3f& operator *= ( float f ){
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    Vector3f cross( Vector3f const& rhs );
    Vector3f& normalize( );
    void rotate( float angle, Vector3f const& axis );

};

inline Vector3f operator -( Vector3f const& l, Vector3f const& r ){
    return Vector3f( l.x - r.x, l.y - r.y, l.z - r.z );
}

inline Vector3f operator *( Vector3f const& l, float f ){
    return Vector3f( l.x * f, l.y * f, l.z * f );
}

class Matrix4f{
public:
    float m[ 4 ][ 4 ];
public:
    Matrix4f() = default;

    void identity(){
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    Matrix4f operator*( Matrix4f const& rhs )const{
        Matrix4f rval;

        for (unsigned int i = 0 ; i < 4 ; i++) {
            for (unsigned int j = 0 ; j < 4 ; j++) {
                rval.m[i][j] = m[i][0] * rhs.m[0][j] +
                              m[i][1] * rhs.m[1][j] +
                              m[i][2] * rhs.m[2][j] +
                              m[i][3] * rhs.m[3][j];
            }
        }

        return rval;
    }

    void initScaleTransform( float scaleX, float scaleY, float scaleZ );
    void initRotateTransform( float rotateX, float rotateY, float rotateZ );
    void initTranslateTranaform( float x, float y, float z );
    void initCameraTransform(const Vector3f& target, const Vector3f& up);
    void initPerspectiveProjection( float fow, float width, float height, float zNear, float zFar );

};

struct Quaternion{
    float x_, y_, z_, w_;

    Quaternion( float x, float y, float z, float w );

    void normalize();

    Quaternion conjugate();
};

Quaternion operator* ( Quaternion const& l, Quaternion const& r );
Quaternion operator* ( Quaternion const& l, Vector3f const& rv );
