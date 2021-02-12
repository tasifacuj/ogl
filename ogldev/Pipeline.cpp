#include "Pipeline.hpp"

const Matrix4f& Pipeline::getWorldTrans(){
    Matrix4f scale, rotate, translate;
    scale.initScaleTransform( scale_.x, scale_.y, scale_.z );
    rotate.initRotateTransform( rotateInfo_.x, rotateInfo_.y, rotateInfo_.z );
    translate.initTranslateTranaform( worldPos_.x, worldPos_.y, worldPos_.z );
    worldTransformation_ = translate * rotate * scale;
    return worldTransformation_;
}

const Matrix4f &Pipeline::getWVPTrans(){
    getWorldTrans();
    Matrix4f cameraTranslate, cameraRotate, projection;

    cameraTranslate.initTranslateTranaform( -camera_.Pos.x, -camera_.Pos.y, -camera_.Pos.z );
    cameraRotate.initCameraTransform( camera_.Target, camera_.Up );
    projection.initPerspectiveProjection( perspectiveProjection_.fow, perspectiveProjection_.width, perspectiveProjection_.height, perspectiveProjection_.znear, perspectiveProjection_.zfar );

    Matrix4f view = cameraRotate * cameraTranslate;
    wvpTransformation_ = projection * view * worldTransformation_;

    return wvpTransformation_;
}
