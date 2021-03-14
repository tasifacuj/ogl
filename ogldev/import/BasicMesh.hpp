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
	void render(unsigned nInstances, Matrix4f const* wvpMats, Matrix4f const* worldMats);
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

	static constexpr int INDEX_BUFFER	= 0;
	static constexpr int POS_VB			= 1;
	static constexpr int NORMAL_VB		= 2;
	static constexpr int TEXCOORD_VB	= 3;
	static constexpr int WVP_MAT_VB		= 4;
	static constexpr int WORLD_MAT_VB	= 5;

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

	GLuint							vao_;
	GLuint							buffers_[6];

	std::vector< BasicMeshEntry >	entries_;
	std::vector< TexturePtr >		textures_;
};