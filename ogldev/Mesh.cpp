#include "Mesh.hpp"

#include <cassert>
#include <iostream>

Mesh::MeshEntry::MeshEntry()
: VBO( INVALID_OGL_VALUE )
, IBO( INVALID_OGL_VALUE )
, NumIndices( 0 )
, MaterialIndex( INVALID_MATERIAL )
{}

Mesh::MeshEntry::~MeshEntry(){
    if( VBO != INVALID_OGL_VALUE )
        glDeleteBuffers( 1, &VBO );

    if( IBO != INVALID_OGL_VALUE )
        glDeleteBuffers( 1, &IBO );
}

bool Mesh::MeshEntry::init(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices){
    NumIndices = indices.size();

    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * vertices.size(), &vertices[ 0 ], GL_STATIC_DRAW );

    glGenBuffers( 1, &IBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned ) * NumIndices, &indices[ 0 ], GL_STATIC_DRAW );

    return true;
}

void Mesh::clear(){
    textures_.clear();
}

bool Mesh::loadMesh(const std::string &filename){
    clear();
    Assimp::Importer importer;
    aiScene const* pScene = importer.ReadFile( filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );

    if( pScene ){
        return initFromScene( pScene, filename );
    }else{
        std::cerr << "Error parsing " << filename << ": " << importer.GetErrorString() << std::endl;
        return false;
    }
}

bool Mesh::initFromScene(const aiScene *pScene, const std::string &filename){
    std::cout << "mNumMeshes = " << pScene->mNumMeshes << ", mNumMaterials = " << pScene->mNumMaterials << std::endl;
    entries_.resize( pScene->mNumMeshes );
    textures_.resize( pScene->mNumMaterials );

    for( unsigned idx = 0; idx < entries_.size(); idx++ ){
        aiMesh const* pMesh = pScene->mMeshes[ idx ];
        initMesh( idx, pMesh );
    }

    return initMaterials( pScene, filename );
}

void Mesh::initMesh(unsigned index, const aiMesh *paiMesh){
    entries_[ index ].MaterialIndex = paiMesh->mMaterialIndex;
    std::vector< Vertex > vertices;
    std::vector< unsigned > indices;
    const aiVector3D zero3D( 0.0f, 0.0f, 0.0f );

    for( size_t idx = 0; idx < paiMesh->mNumVertices; idx++ ){
        aiVector3D const* pPos = &( paiMesh->mVertices[ idx ] );
        aiVector3D const* pNormal = &( paiMesh->mNormals[ idx ] );
        aiVector3D const* pTexCoord = paiMesh->HasTextureCoords( 0 ) ? &( paiMesh->mTextureCoords[ 0 ][ idx ] ) : &zero3D;
        aiVector3D const* pTangent = &( paiMesh->mTangents[ idx ] );
        Vertex v( Vector3f( pPos->x, pPos->y, pPos->z )
            , Vector2f( pTexCoord->x, pTexCoord->y )
            , Vector3f( pNormal->x, pNormal->y, pNormal->z )
			, Vector3f(pTangent->x, pTangent->y, pTangent->z)
            );
        vertices.emplace_back( v );
    }

    for( size_t idx = 0; idx < paiMesh->mNumFaces; idx++ ){
        aiFace const& face = paiMesh->mFaces[ idx ];
        assert( face.mNumIndices == 3 );
        indices.push_back( face.mIndices[ 0 ] );
        indices.push_back( face.mIndices[ 1 ] );
        indices.push_back( face.mIndices[ 2 ] );
    }

    entries_[ index ].init( vertices, indices );
}

bool Mesh::initMaterials(const aiScene *pScene, const std::string &filename){
    std::string::size_type slashIdx = filename.find_last_of( "/" );
    std::string dir;

    if( slashIdx == std::string::npos )
        dir = ".";
    else if( slashIdx == 0 )
        dir = "/";
    else
        dir = filename.substr( 0, slashIdx );

    bool rval = true;

    for( size_t idx = 0; idx < pScene->mNumMaterials; idx++ ){
        aiMaterial const* pMaterial = pScene->mMaterials[ idx ];

        if( pMaterial->GetTextureCount( aiTextureType_DIFFUSE ) > 0 ){
            aiString path;

            if( pMaterial->GetTexture( aiTextureType_DIFFUSE, 0, &path ) == AI_SUCCESS ){
                std::cout << "path = " << path.data << std::endl;
                std::string fullPath = dir + "/" + path.data;
                 std::replace( fullPath.begin(), fullPath.end(), '\\', '/');


                textures_[ idx ] = std::make_shared< Texture >( GL_TEXTURE_2D, fullPath );

                if( !textures_[ idx ]->load() ){
                    std::cerr << "Failed to load texture " << fullPath << std::endl;
                    textures_[ idx ].reset();
                    rval = false;
                }else{
                    std::cout << "Loaded texture for idx " << idx << " and path " << fullPath << std::endl;
                }
            }
        }
    }

    return rval;
}

void Mesh::render(RenderCallbackInterface* rcb){
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );
	glEnableVertexAttribArray(3);

    for( size_t idx = 0; idx < entries_.size(); idx++ ){
        glBindBuffer( GL_ARRAY_BUFFER, entries_[ idx ].VBO );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), 0 );                     // pos
        glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( const GLvoid* )12 );   // texture
        glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( const GLvoid* )20 );   // normal
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)32);   // tangent

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, entries_[ idx ].IBO );
        unsigned materialIdx = entries_[ idx ].MaterialIndex;

        if( materialIdx < textures_.size() && textures_[ materialIdx ] )
            textures_[ materialIdx ]->bind( GL_TEXTURE0 );

		if (rcb)
			rcb->drawStartedCB(idx);

        glDrawElements( GL_TRIANGLES, entries_[ idx ].NumIndices, GL_UNSIGNED_INT, 0 );
    }

	glDisableVertexAttribArray(3);
    glDisableVertexAttribArray( 2 );
    glDisableVertexAttribArray( 1 );
    glDisableVertexAttribArray( 0 );
}

void Mesh::render(unsigned drawIdx, unsigned primID) {
	assert(drawIdx < entries_.size());

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, entries_[drawIdx].VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries_[ drawIdx ].IBO);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(primID * 3 * sizeof(GLuint)));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}