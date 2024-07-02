#include "pch.h"
#include "SkinnedMesh.h"
#include "FBXLoader.h"
#include "SkinnedData.h"

SkinnedMesh::SkinnedMesh(ComPtr<ID3D11Device> device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath)
{
	std::vector<FbxMaterial> mats;
	FBXLoader fbxLoader;
	fbxLoader.LoadFBX(modelFilename, Vertices, Indices, Subsets, mats, SkinnedData);

	ModelMesh.SetVertices(device, &Vertices[0], Vertices.size());
	ModelMesh.SetIndices(device, &Indices[0], Indices.size());
	ModelMesh.SetSubsetTable(Subsets);

	SubsetCount = mats.size();

	for (uint32 i = 0; i < SubsetCount; ++i)
	{
		Mat.push_back(mats[i].Mat);

		ComPtr<ID3D11ShaderResourceView> diffuseMapSRV = texMgr.CreateTexture(texturePath + mats[i].DiffuseMapName);
		DiffuseMapSRV.push_back(diffuseMapSRV);

		ComPtr<ID3D11ShaderResourceView> normalMapSRV = texMgr.CreateTexture(texturePath + mats[i].NormalMapName);
		NormalMapSRV.push_back(normalMapSRV);
	}
}

SkinnedMesh::~SkinnedMesh()
{
}


void SkinnedMeshlInstance::Update(float dt)
{
	TimePos += dt;
	Model->SkinnedData.GetFinalTransforms(ClipName, TimePos, FinalTransforms);

	// Loop animation
	if (TimePos > Model->SkinnedData.GetClipEndTime(ClipName))
		TimePos = 0.0f;
}
