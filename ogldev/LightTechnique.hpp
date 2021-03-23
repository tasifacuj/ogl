#ifndef LIGHTTECHNIQUE_HPP
#define LIGHTTECHNIQUE_HPP

#include "math_3d.h"
#include "Technique.hpp"
#include "common/LightsCommon.hpp"

class LightTechnique : public Technique{
public: // == CONSTANTS ==
    static constexpr int MAX_POINT_LIGHTS = 2;
    static constexpr int MAX_SPOT_LIGHTS = 2;
public: // == ctors ==
    LightTechnique() = default;
public: // == Technique ==
    virtual bool init() override;
public: //== LightTechnique ==
    void setWVP( const Matrix4f& wvp );
    void setWorldMatrix( const Matrix4f& wm );
    void setDirectionLight( DirectionalLight const& dl );

    void setEyeWorldPosition( Vector3f const& eyeWorldPos );
    void setMatSpecularIntensity( float intensity );
    void setMatSpecularPower( float power );

    void setPointLights( unsigned numLights, PointLight const* pLights );
    void setSpotLigts( unsigned nLigts, SpotLight const* pLights );

    void setLightWVP( Matrix4f const& lightWVP );

    void setColorTextureUnit( unsigned textureUnit );
    void setShadowMapTextureUnit( unsigned textureUnitId );
    void setNormalMapTextureUnit( unsigned tuid );
private:
    GLuint  wVPLoacation_{};
    GLuint  worldMatrixLocation_{};
    
    GLuint  colorMapLocation_{};// sampler2D gSampler
    GLuint  shadowMapLocation_{};
    GLuint  normalMapLocation_{};

    GLuint eyeWorldPosition_{};
    GLuint matSpecularIntensityLocation_{};
    GLuint matSpecularPowerLocation_{};
    GLuint numPointLightsLocation_{};
    GLuint numSpotLightsLocation_{};



    struct{
        GLuint  Color;
        GLuint  AmbientIntensity;
        GLuint  Direction;
        GLuint  DiffuseIntensity;
    } dirLightLocation_;

    struct {
        GLuint  Color;
        GLuint  AmbientIntensity;
        GLuint  DiffuseIntensity;
        GLuint  Position;

        struct{
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } pointLightsLocation_[ MAX_POINT_LIGHTS ];

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint CutOff;

        struct{
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } spotLightsLocation_[ MAX_SPOT_LIGHTS ];

    GLuint  lightWVPLocation_{};
    
};

#endif // LIGHTTECHNIQUE_HPP
