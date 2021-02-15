#ifndef TECHNIQUE_HPP
#define TECHNIQUE_HPP

#include <GL/glew.h>
#include <list>

class Technique{
public: // == TYPES ==
    using Shaders = std::list< GLuint >;
public:
    static constexpr int INVALID_UNIFORM_LOCATION = 0xFFFFFFFF;
public: // == CTORS ==
    Technique() = default;
    virtual ~Technique();
public: // == Technique ==
    virtual bool init();
public:
    void enable();
    void useProgram();
protected:
    bool addShader( GLenum shaderType, const char* shaderText );
    bool finalize();
    GLint getUniformLocation( const char* uniformName );
private:
    GLuint  shaderProgram_{ 0 };
    Shaders shadersObjList_;
};

#endif // TECHNIQUE_HPP
