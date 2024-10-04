#pragma once
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Converter
{
public:
	Converter();
	~Converter();

public:
	void ReadAssetFile(wstring file);
	void ExportModelData(wstring savePath);
	void ExportMaterialData(wstring savePath);

private:
	void ReadModelData(aiNode* node, int32 index, int32 parent);
	void ReadMeshData(aiNode* node, int32 bone);
	void WriteModelFile(wstring finalPath);

private:
	void ReadMaterialData();
	void WriteMaterialData(wstring finalPath);
	string WriteTexture(string saveFolder, string file);

private:
	wstring _assetPath = L"../Resources/Assets/";
	wstring _modelPath = L"../Resources/Models/";
	wstring _texturePath = L"../Resources/Textures/";

private:
	shared_ptr<Assimp::Importer> _importer;
	const aiScene* _scene;

private:
	vector<shared_ptr<asBone>> _bones;
	vector<shared_ptr<asMesh>> _meshes;
	vector<shared_ptr<asMaterial>> _materials;
};

