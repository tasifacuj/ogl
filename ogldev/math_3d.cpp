#include "math_3d.h"

Vector3f Vector3f::cross(const Vector3f &v){
    const float _x = y * v.z - z * v.y;
    const float _y = z * v.x - x * v.z;
    const float _z = x * v.y - y * v.x;

    return Vector3f( _x, _y, _z );
}

Vector3f &Vector3f::normalize( ){
    float len = std::sqrt( x * x + y * y + z * z );
    x /= len;
    y /= len;
    z /= len;

    return *this;
}
void Vector3f::rotate(float angle, const Vector3f &axis){
    const float sinHlaphAngle = sin( ToRadian( angle / 2 ) );
    const float cosHalpfAngle = cos( ToRadian( angle / 2 ) );

    const float vx = axis.x * sinHlaphAngle;
    const float vy = axis.y * sinHlaphAngle;
    const float vz = axis.z * sinHlaphAngle;
    const float vw = cosHalpfAngle;
    Quaternion rotationQ( vx, vy, vz, vw );
    Quaternion conjugateQ = rotationQ.conjugate();
    Quaternion W = rotationQ * ( *this ) * conjugateQ;

    x = W.x_;
    y = W.y_;
    z = W.z_;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix4f::initScaleTransform(float scaleX, float scaleY, float scaleZ){
    m[0][0] = scaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
    m[1][0] = 0.0f;   m[1][1] = scaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
    m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = scaleZ; m[2][3] = 0.0f;
    m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
}

void Matrix4f::initRotateTransform(float rotateX, float rotateY, float rotateZ){
    Matrix4f rx, ry, rz;
    float x = ToRadian( rotateX );
    float y = ToRadian( rotateY );
    float z = ToRadian( rotateZ );

    rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f   ; rx.m[0][2] = 0.0f    ; rx.m[0][3] = 0.0f;
    rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
    rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x) ; rx.m[2][3] = 0.0f;
    rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f   ; rx.m[3][2] = 0.0f    ; rx.m[3][3] = 1.0f;

    ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
    ry.m[1][0] = 0.0f   ; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f    ; ry.m[1][3] = 0.0f;
    ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y) ; ry.m[2][3] = 0.0f;
    ry.m[3][0] = 0.0f   ; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f    ; ry.m[3][3] = 1.0f;

    rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
    rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z) ; rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
    rz.m[2][0] = 0.0f   ; rz.m[2][1] = 0.0f    ; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
    rz.m[3][0] = 0.0f   ; rz.m[3][1] = 0.0f    ; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

    *this = rz * ry * rx;
}


void Matrix4f::initTranslateTranaform(float x, float y, float z){
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


void Matrix4f::initCameraTransform(const Vector3f &target, const Vector3f &up){
    Vector3f N = target;// z-axis
    N.normalize();

    Vector3f U = up;
    U = U.cross( N );
    U.normalize();

    Vector3f V = N.cross( U );

    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
}

void Matrix4f::initPerspectiveProjection(float fow, float width, float height, float zNear, float zFar){
    float ar = width / height;
    float zRange = zFar - zNear;
    float tanHalfFOV = std::tan( ToRadian( fow / 2.0f ) );

    m[0][0] = 1.0f/(tanHalfFOV * ar); m[0][1] = 0.0f;            m[0][2] = 0.0f;                    m[0][3] = 0.0;
    m[1][0] = 0.0f;                   m[1][1] = 1.0f/tanHalfFOV; m[1][2] = 0.0f;                    m[1][3] = 0.0;
    m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-zNear -zFar)/zRange ;  m[2][3] = 2.0f * zFar*zNear/zRange;
    m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;                    m[3][3] = 0.0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Quaternion::Quaternion(float x, float y, float z, float w)
: x_( x )
, y_( y )
, z_( z )
, w_( w )
{}

void Quaternion::normalize(){
    float length = sqrt( x_ * x_ + y_ * y_ + z_ * z_ );

    x_ /= length;
    y_ /= length;
    z_ /= length;
    w_ /= length;
}

Quaternion Quaternion::conjugate(){
    return Quaternion( -x_, -y_, -z_, w_ );
}

Quaternion operator* ( Quaternion const& l, Quaternion const& r ){
    const float w = (l.w_ * r.w_) - (l.x_ * r.x_) - (l.y_ * r.y_) - (l.z_ * r.z_);
    const float x = (l.x_ * r.w_) + (l.w_ * r.x_) + (l.y_ * r.z_) - (l.z_ * r.y_);
    const float y = (l.y_ * r.w_) + (l.w_ * r.y_) + (l.z_ * r.x_) - (l.x_ * r.z_);
    const float z = (l.z_ * r.w_) + (l.w_ * r.z_) + (l.x_ * r.y_) - (l.y_ * r.x_);

    Quaternion ret(x, y, z, w);

    return ret;
}

Quaternion operator* ( Quaternion const& q, Vector3f const& v ){
    const float w = - (q.x_ * v.x) - (q.y_ * v.y) - (q.z_ * v.z);
    const float x =   (q.w_ * v.x) + (q.y_ * v.z) - (q.z_ * v.y);
    const float y =   (q.w_ * v.y) + (q.z_ * v.x) - (q.x_ * v.z);
    const float z =   (q.w_ * v.z) + (q.x_ * v.y) - (q.y_ * v.x);

    Quaternion ret(x, y, z, w);

    return ret;
}
