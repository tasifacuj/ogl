#include "BasicMesh.hpp"
#include "../EngineCommon.hpp"
#include "../Utils.hpp"

#include <cstring>
#include <iostream>
#include <cassert>

static const int POSITION_LOCATION	= 0;
static const int TEX_COORD_LOCATION	= 1;
static const int NORMAL_LOCATION	= 2;
static const int WVP_LOCATION		= 3;
static const int WORLD_LOCATION		= 7;

BasicMesh::BasicMesh()
	: vao_(0) {
	memset(buffers_, 0, sizeof(buffers_));
}

BasicMesh::~BasicMesh() {
	clear();
	memset(buffers_, 0, sizeof(buffers_));
}

void BasicMesh::clear() {
	textures_.clear();

	if (buffers_[0]) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(buffers_), buffers_);
	}

	if (vao_) {
		glDeleteVertexArrays(1, &vao_);
		vao_ = 0;
	}
}

bool BasicMesh::loadMesh(std::string const& filename) {
	clear();
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(buffers_), buffers_);
	bool rval = false;
	Assimp::Importer importer;
	aiScene const* pScene = importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);

	if (pScene)
		rval = initFromScene(pScene, filename);
	else
		std::cerr << "Failed to load " << filename << ", err: " << importer.GetErrorString() << std::endl;

	glBindVertexArray(0);
	return rval;
}

bool BasicMesh::initFromScene(aiScene const* pScene, std::string const& filename) {
	entries_.resize(pScene->mNumMeshes);
	textures_.resize(pScene->mNumMaterials);

	std::vector< Vector3f > positions;
	std::vector< Vector3f > normals;
	std::vector< Vector2f > texCoords;
	std::vector< unsigned > indices;

	unsigned numVertices = 0, numIndices = 0;

	// count vertices and indices
	for(unsigned idx = 0; idx < entries_.size(); idx++) {
		entries_[idx].MaterialIndex = pScene->mMeshes[idx]->mMaterialIndex;
		entries_[idx].NumIndices = pScene->mMeshes[idx]->mNumFaces * 3;
		entries_[idx].BaseVertex = numVertices;
		entries_[idx].BaseIndex = numIndices;

		numVertices += pScene->mMeshes[idx]->mNumVertices;
		numIndices += entries_[idx].NumIndices;
	}

	positions.reserve(numVertices);
	normals.reserve(numVertices);
	texCoords.reserve(numVertices);
	indices.reserve(numIndices);

	for (size_t idx = 0; idx < entries_.size(); idx++) {
		aiMesh const* pMesh = pScene->mMeshes[idx];
		initMesh(pMesh, positions, normals, texCoords, indices);
	}

	if (!initMaterials(pScene, filename))
		return false;
// positions
	glBindBuffer(GL_ARRAY_BUFFER, buffers_[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
// textures
	glBindBuffer(GL_ARRAY_BUFFER, buffers_[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
// normals
	glBindBuffer(GL_ARRAY_BUFFER, buffers_[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers_[WVP_MAT_VB]);

	for (unsigned int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(WVP_LOCATION + i);
		glVertexAttribPointer(WVP_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4f), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(WVP_LOCATION + i, 1);
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffers_[WORLD_MAT_VB]);

	for (unsigned int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(WORLD_LOCATION + i);
		glVertexAttribPointer(WORLD_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4f), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(WORLD_LOCATION + i, 1);
	}


	return GLCheckError();
}

void BasicMesh::initMesh(aiMesh const* paiMesh
	, std::vector< Vector3f >& positions
	, std::vector< Vector3f >& normals
	, std::vector< Vector2f >& texCoords
	, std::vector< unsigned >& indices)
{
	const aiVector3D zero3d(0.0f, 0.0f, 0.0f);

	for (size_t idx = 0; idx < paiMesh->mNumVertices; idx++) {
		aiVector3D const* pPos = &(paiMesh->mVertices[idx]);
		aiVector3D const* pNormal = &(paiMesh->mNormals[idx]);
		aiVector3D const* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][idx]) : &zero3d;

		positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
		normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));
		texCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));
	}

	// populate index buffer
	for (size_t idx = 0; idx < paiMesh->mNumFaces; idx++) {
		aiFace const& face = paiMesh->mFaces[idx];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
}

bool BasicMesh::initMaterials(aiScene const* pScene, std::string const& filename) {
	std::string::size_type SlashIndex = filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = filename.substr(0, SlashIndex);
	}

	bool rval = true;

	// Initialize the materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p(Path.data);

				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				std::string FullPath = Dir + "/" + p;

				textures_[i] = std::make_shared< Texture >( GL_TEXTURE_2D, FullPath.c_str() );

				if (!textures_[i]->load()) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
					textures_[i].reset();
					rval = false;
				}
				else {
					printf("Loaded texture '%s'\n", FullPath.c_str());
				}
			}
		}
	}

	return rval;
}

void BasicMesh::render() {
	glBindVertexArray(vao_);

	for (size_t idx = 0; idx < entries_.size(); idx++) {
		const unsigned matIdx = entries_[idx].MaterialIndex;
		assert(matIdx < textures_.size());

		if (textures_[matIdx]) {
			textures_[matIdx]->bind(COLOR_TEXTURE_UNIT);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES
			, entries_[idx].NumIndices
			, GL_UNSIGNED_INT
			, (void*)(sizeof(unsigned) * entries_[idx].BaseIndex)
			, entries_[idx].BaseVertex
		);
	}

	glBindVertexArray(0);
}

void BasicMesh::render(unsigned NumInstances, Matrix4f const* wvpMats, Matrix4f const* worldMats) {
	glBindBuffer(GL_ARRAY_BUFFER, buffers_[WVP_MAT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, wvpMats, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers_[WORLD_MAT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, worldMats, GL_DYNAMIC_DRAW);

	glBindVertexArray(vao_);

	for (unsigned int i = 0; i < entries_.size(); i++) {
		const unsigned int MaterialIndex = entries_[i].MaterialIndex;

		assert(MaterialIndex < textures_.size());

		if (textures_[MaterialIndex]) {
			textures_[MaterialIndex]->bind(GL_TEXTURE0);
		}

		glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
			entries_[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * entries_[i].BaseIndex),
			NumInstances,
			entries_[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}
