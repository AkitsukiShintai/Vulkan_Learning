#include"ModelReader.h"
#include<string>

void ModelReader::ReadModule(const std::string& pFile, Mesh* mesh) {
	// Create an instance of the Importer class
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	if (!scene)
	{
		std::exception(importer.GetErrorString());
	}
	aiNode* root = scene->mRootNode;
	
	ReadNode(root, scene, mesh);
}

void ModelReader::ReadNode(aiNode* node, const aiScene* scene, Mesh* mesh) {


	for (int i = 0; i < node->mNumMeshes; i++)
	{
		if (i == 0)
		{
			ReadMesh(mesh->mSubmeshes[0], scene->mMeshes[node->mMeshes[i]], scene);
		}
		else
		{
			Mesh* newMesh = new Mesh();
			ReadMesh(newMesh, scene->mMeshes[node->mMeshes[i]], scene);
			mesh->AddSubmesh(newMesh);
		}
		
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ReadNode(node->mChildren[i], scene, mesh);
	}
}

void ModelReader::ReadMesh(Mesh* result, aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex_Aki> vertices(mesh->mNumVertices);
	std::vector<uint32_t> indices;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex_Aki vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.texCoord = glm::vec2(0.0f, 0.0f);
			vertex.texCoord2 = glm::vec3(0.0f, 0.0f,0.0f);
			vertex.texCoord3 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		vertices[i] = (vertex) ;
	}

	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}



	result->SetVertices(vertices);
	result->SetIndices(indices, 0);

}