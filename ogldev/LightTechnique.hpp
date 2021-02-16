#ifndef LIGHTTECHNIQUE_HPP
#define LIGHTTECHNIQUE_HPP

#include "math_3d.h"
#include "Technique.hpp"

struct BaseLight{
    Vector3f    Color;
    float       AmbientIntensity;
    float       DiffuseIntensity;

    BaseLight()
    : Color( 0.0f, 0.0f, 0.0f )
    , AmbientIntensity( 0.0f )
    , DiffuseIntensity( 0.0f )
    {}
};



struct DirectionLight : BaseLight{
    Vector3f    Direction;

    DirectionLight()
    : Direction( 0.0f, 0.0f, 0.0f )
    {}
};

struct PointLight : BaseLight{
    Vector3f    Position;

    struct{
        float Constatnt;
        float Linear;
        float Exp;
    }Attenuation;

    PointLight()
    : Position( 0.0f, 0.0f, 0.0f ){
        Attenuation.Constatnt = 1.0f;
        Attenuation.Linear = 0.0f;
        Attenuation.Exp = 0.0f;
    }
};


struct SpotLight : PointLight{
    Vector3f    Direction;
    float       CutOff;//angle

    SpotLight()
    : Direction( 0.0f, 0.0f, 0.0f )
    , CutOff( 0.0f )
    {}
};

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
    void setTextureUnit( unsigned textureUnit );
    void setDirectionLight( DirectionLight const& dl );

    void setEyeWorldPosition( Vector3f const& eyeWorldPos );
    void setMatSpecularIntensity( float intensity );
    void setMatSpecularPower( float power );

    void setPointLights( unsigned numLights, PointLight const* pLights );
    void setSpotLigts( unsigned nLigts, SpotLight const* pLights );

    void setLightWVP( Matrix4f const& lightWVP );
    void setShadowMapTextureUnit( unsigned textureUnitId );
private:
    GLuint  wVPLoacation_{};
    GLuint  worldMatrixLocation_{};
    GLuint  samplerLocation_{};// sampler2D gSampler

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
    GLuint  shadowMapLocation_{};
};

#endif // LIGHTTECHNIQUE_HPP
