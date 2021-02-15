#include "Technique.hpp"

#include <cstdio>
#include <cstring>

Technique::~Technique(){
    for( auto &s : shadersObjList_ ){
        glDeleteShader( s );
    }

    if( shaderProgram_ != 0 ){
        glDeleteProgram( shaderProgram_ );
        shaderProgram_ = 0;
    }
}

bool Technique::init(){
    shaderProgram_ = glCreateProgram();

    if( shaderProgram_ == 0 ){
        fprintf( stderr, "Failed to create program" );
        return false;
    }

    return true;
}

bool Technique::addShader(GLenum shaderType, const char *shaderText){
    GLuint shaderObj = glCreateShader( shaderType );

    if( 0 == shaderObj ){
        fprintf( stderr, "Failed to create shader of type %d\n", shaderType );
        return false;
    }

    shadersObjList_.push_back( shaderObj );
    const GLchar* p[ 1 ];
    p[ 0 ] = shaderText;
    GLint lengths[ 1 ];
    lengths[ 0 ] = strlen( shaderText );
    glShaderSource( shaderObj, 1, p, lengths );
    glCompileShader( shaderObj );

    GLint succ;
    glGetShaderiv( shaderObj, GL_COMPILE_STATUS, &succ );

    if( !succ ){
        GLchar log[ 1024 ];
        glGetShaderInfoLog( shaderObj, 1024, NULL, log );
        fprintf( stderr, "Failed to compile shader type %i, err: %s\n", shaderType, log );
        return false;
    }

    glAttachShader( shaderProgram_, shaderObj );
    return true;
}

bool Technique::finalize(){
    GLint success = 0;
    GLchar errLog[ 1024 ] = { 0 };
// link program
    glLinkProgram( shaderProgram_ );
    glGetProgramiv( shaderProgram_, GL_LINK_STATUS, &success );

    if( success == 0 ){
        glGetProgramInfoLog( shaderProgram_, 1024, NULL, errLog );
        fprintf( stderr, "Invalid program: %s\n", errLog );
        return false;
    }

// validate program
    glValidateProgram( shaderProgram_ );
    glGetProgramiv( shaderProgram_, GL_VALIDATE_STATUS, &success );

    if( success == 0 ){
        glGetProgramInfoLog( shaderProgram_, 1024, NULL, errLog );
        fprintf( stderr, "Invalid program: %s\n", errLog );
        return false;
    }

    for( auto sh : shadersObjList_ ){
        glDeleteShader( sh );
    }

    shadersObjList_.clear();
    return true;
}

void Technique::enable(){
    glUseProgram( shaderProgram_ );
}

void Technique::useProgram(){
    enable();
}

GLint Technique::getUniformLocation( const char* name ){
    GLint loc = glGetUniformLocation( shaderProgram_, name );

    if( loc == 0xFFFFFFFF )
        fprintf( stderr, "Unable to get location for %s\n", name );

    return loc;
}


