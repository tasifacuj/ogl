#include "Pipeline.hpp"

const Matrix4f& Pipeline::getWorldTransformation(){
    Matrix4f scale, rotate, translate;
    scale.InitScaleTransform( scale_.x, scale_.y, scale_.z );
    rotate.InitRotateTransform( rotateInfo_.x, rotateInfo_.y, rotateInfo_.z );
    translate.InitTranslationTransform( worldPos_.x, worldPos_.y, worldPos_.z );
    worldTransformation_ = translate * rotate * scale;
    return worldTransformation_;
}

const Matrix4f &Pipeline::getWVPTransformation(){
    getWorldTransformation();
    getViewProjectionTransformation();
    
    wvpTransformation_ = viewProjectionTransformation_ * worldTransformation_;
    return wvpTransformation_;
}

Matrix4f const& Pipeline::getViewProjectionTransformation(){
    Matrix4f cameraTranslationTransform, cameraRotationTransform, perspectiveProjectionTransform;
    cameraTranslationTransform.InitTranslationTransform( -camera_.Pos.x, -camera_.Pos.y, -camera_.Pos.z );
    cameraRotationTransform.InitCameraTransform( camera_.Target, camera_.Up );
    // void Matrix4f::InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar)
    perspectiveProjectionTransform.InitPersProjTransform( perspectiveProjection_.fow, perspectiveProjection_.width, perspectiveProjection_.height, perspectiveProjection_.znear, perspectiveProjection_.zfar );
    Matrix4f view = cameraRotationTransform * cameraTranslationTransform;
    viewProjectionTransformation_ = perspectiveProjectionTransform * view;
    return viewProjectionTransformation_;
}
