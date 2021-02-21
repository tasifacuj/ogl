#ifndef MESH_HPP
#define MESH_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <vector>
#include <GL/glew.h>
#include <memory>

#include "Utils.hpp"
#include "math_3d.h"
#include "Texture.hpp"

struct Vertex{
    Vector3f Pos;
    Vector2f Tex;
    Vector3f Normal;
    Vector3f Tangent;

    Vertex( Vector3f const& pos, Vector2f const& tex, Vector3f const& normal, Vector3f const& tangent )
    : Pos( pos )
    , Tex( tex )
    , Normal( normal )
    , Tangent( tangent )
    {}
};

class Mesh final{
public: // == TYPEs ==
    struct MeshEntry{
        GLuint VBO;
        GLuint IBO;
        unsigned NumIndices;
        unsigned MaterialIndex;

        MeshEntry();
        ~MeshEntry();

        bool init( std::vector< Vertex > const& vertices, std::vector< unsigned > const& indices );
    };

    using TextuePtr = std::shared_ptr< Texture >;
public: // == CONSTANTS ==
    static constexpr int INVALID_MATERIAL =  0xFFFFFFFF;
private: // == MEMBERs ==
    std::vector< MeshEntry > entries_;
    std::vector< TextuePtr > textures_;
public: // == CTORs ==
    Mesh() = default;
    ~Mesh(){
        clear();
    }
public: // == Mesh ==
    bool loadMesh( std::string const& filename );
    void render();
private:
    bool initFromScene( aiScene const* pScene, std::string const& filename );
    void initMesh( unsigned index, aiMesh const* paiMesh );
    bool initMaterials( aiScene const* pScene, std::string const& filename );
    void clear();
};

#endif // MESH_HPP
