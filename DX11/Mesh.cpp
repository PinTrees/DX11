#include "pch.h"
#include "Mesh.h"
#include "FBXLoader.h"
#include "LoadM3d.h"


std::string GetFileExtension(const std::string& filename)
{
	size_t dotPos = filename.find_last_of(L'.');
	if (dotPos != std::string::npos)
	{
		return filename.substr(dotPos + 1);
	}
	return "";
}


Mesh::Mesh(ComPtr<ID3D11Device> device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath)
{
	string modelfilepath = PathManager::GetI()->GetMovePathS(modelFilename);

	std::string fileExtension = GetFileExtension(modelfilepath);

	if (fileExtension == "fbx")
	{
		std::vector<FbxMaterial> mats;

		FBXLoader m3dLoader;
		m3dLoader.LoadFBX(modelfilepath, Vertices, Indices, Subsets, mats);

		ModelMesh.SetVertices(device, &Vertices[0], Vertices.size());
		ModelMesh.SetIndices(device, &Indices[0], Indices.size());
		ModelMesh.SetSubsetTable(Subsets);

		SubsetCount = mats.size();

		for (uint32 i = 0; i < SubsetCount; ++i)
		{
			Mat.push_back(mats[i].Mat);

			ComPtr<ID3D11ShaderResourceView> diffuseMapSRV = texMgr.CreateTexture(texturePath + L"FishingBoat_Wood_ALB.png");
			DiffuseMapSRV.push_back(diffuseMapSRV);

			ComPtr<ID3D11ShaderResourceView> normalMapSRV = texMgr.CreateTexture(texturePath + L"FishingBoat_Wood_ALB.png");
			NormalMapSRV.push_back(normalMapSRV);
		}
	}
	else if(fileExtension == "m3d")
	{
		std::vector<M3dMaterial> mats;

		M3DLoader m3dLoader;
		m3dLoader.LoadM3d(modelfilepath, Vertices, Indices, Subsets, mats);

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
}

Mesh::Mesh(ComPtr<ID3D11Device> device, const std::string& modelFilename)
{
	string modelfilepath = PathManager::GetI()->GetMovePathS(modelFilename);

	std::string fileExtension = GetFileExtension(modelfilepath);

	if (fileExtension == "fbx" || fileExtension == "FBX")
	{
		std::vector<FbxMaterial> mats;

		FBXLoader m3dLoader;
		m3dLoader.LoadFBX(modelfilepath, Vertices, Indices, Subsets, mats);

		ModelMesh.SetVertices(device, &Vertices[0], Vertices.size());
		ModelMesh.SetIndices(device, &Indices[0], Indices.size());
		ModelMesh.SetSubsetTable(Subsets);

		SubsetCount = mats.size();

		for (uint32 i = 0; i < SubsetCount; ++i)
		{
			Mat.push_back(mats[i].Mat);
		}
	}
}

Mesh::~Mesh()
{
}
