#pragma once
#include"Mesh.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
class ModelReader {
public: 

 static	void ReadModule(const std::string& pFile, Mesh* mesh);

 static	void ReadNode(aiNode* node, const aiScene* scene, Mesh* mesh);

 static	void ReadMesh(Mesh* result, aiMesh* mesh, const aiScene* scene);

};