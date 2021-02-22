#include "BillboardList.hpp"

#include "EngineCommon.hpp"

#define NUM_ROWS  10
#define NUM_COLS 10

BillboardList::~BillboardList(){
    glDeleteBuffers( 1, &vbo_ );
}

bool BillboardList::init(const std::string &pathToTexture){
    pTexture_ = std::make_unique< Texture >( GL_TEXTURE_2D, pathToTexture );

    if( not pTexture_->load() )
        return false;

    createPositionBuffer();

    if( !shader_.init() )
        return false;

    return true;
}

void BillboardList::createPositionBuffer(){
    Vector3f positions[ NUM_ROWS * NUM_COLS ];

    for( size_t j = 0; j < NUM_ROWS; j++ ){
        for( size_t i = 0; i < NUM_COLS; i++ ){
            Vector3f p( float( i ), float( j ) );
            positions[ j * NUM_COLS + i ] = p;
        }
    }

    glGenBuffers( 1, &vbo_ );
    glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
    glBufferData( GL_ARRAY_BUFFER, sizeof( positions ), &positions[ 0 ], GL_STATIC_DRAW );
}

void BillboardList::render( Matrix4f const& vp, Vector3f const& cameraPos ){
    shader_.useProgram();
    shader_.setVP( vp );
    shader_.setCameraPosition( cameraPos );
    pTexture_->bind( COLOR_TEXTURE_UNIT );

    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 ); // position
    glDrawArrays( GL_POINTS, 0, NUM_ROWS * NUM_COLS );
    glDisableVertexAttribArray( 0 );
}
