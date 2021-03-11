#pragma once

#include "../math_3d.h"

#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "../Texture.hpp"

class BasicMesh {
public: // == CTORs --
	BasicMesh();
	~BasicMesh();
public: // == BasicMesh ==
	bool loadMesh(std::string const& filename);
	void render();
private:
	bool initFromScene(aiScene const* pScene, std::string const& filename);
	
	void initMesh(aiMesh const* paiMesh
		, std::vector< Vector3f >& positions
		, std::vector< Vector3f >& normals
		, std::vector< Vector2f >& texCoords
		, std::vector< unsigned >& indices
	);

	bool initMaterials(aiScene const* pScene, std::string const& filename);
	void clear();
private:
#define INVALID_MATERIAL 0xFFFFFFFF

#define INDEX_BUFFER 0    
#define POS_VB       1
#define NORMAL_VB    2
#define TEXCOORD_VB  3  
	struct BasicMeshEntry {
		BasicMeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	using TexturePtr = std::shared_ptr< Texture >;

	GLuint	vao_;
	GLuint	buffers_[4];

	std::vector< BasicMeshEntry >	entries_;
	std::vector< TexturePtr >		textures_;
};