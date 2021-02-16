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
    Matrix4f cameraTranslate, cameraRotate, projection;

    cameraTranslate.InitTranslationTransform( -camera_.Pos.x, -camera_.Pos.y, -camera_.Pos.z );
    cameraRotate.InitCameraTransform( camera_.Target, camera_.Up );
    projection.InitPersProjTransform( perspectiveProjection_.fow, perspectiveProjection_.width, perspectiveProjection_.height, perspectiveProjection_.znear, perspectiveProjection_.zfar );

    Matrix4f view = cameraRotate * cameraTranslate;
    wvpTransformation_ = projection * view * worldTransformation_;

    return wvpTransformation_;
}
