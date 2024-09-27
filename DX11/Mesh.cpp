#include "pch.h"
#include "Mesh.h"
#include "FBXLoader.h"
#include "LoadM3d.h"
#include "MathHelper.h"

std::string GetFileExtension(const std::string& filename)
{
	size_t dotPos = filename.find_last_of(L'.');
	if (dotPos != std::string::npos)
	{
		return filename.substr(dotPos + 1);
	}
	return "";
}

// 반지름 계산 함수
float Mesh::ComputeRadius(const Vec3& center, const std::vector<Vertex::PosNormalTexTan2>& vertices)
{
	float maxDistance = 0.0f;

	for (const auto& vertex : vertices)
	{
		float distance = std::sqrt((center.x - vertex.pos.x) * (center.x - vertex.pos.x) +
			(center.y - vertex.pos.y) * (center.y - vertex.pos.y) +
			(center.z - vertex.pos.z) * (center.z - vertex.pos.z));
		if (distance > maxDistance)
		{
			maxDistance = distance;
		}
	}

	return maxDistance;
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

		Vec3 minVertex = { +MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity };
		Vec3 maxVertex = { -MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity };
		for (int i = 0; i < Vertices.size(); i++)
		{
			if (Vertices[i].pos.x < minVertex.x) minVertex.x = Vertices[i].pos.x;
			if (Vertices[i].pos.y < minVertex.y) minVertex.y = Vertices[i].pos.y;
			if (Vertices[i].pos.z < minVertex.z) minVertex.z = Vertices[i].pos.z;

			if (Vertices[i].pos.x > maxVertex.x) maxVertex.x = Vertices[i].pos.x;
			if (Vertices[i].pos.y > maxVertex.y) maxVertex.y = Vertices[i].pos.y;
			if (Vertices[i].pos.z > maxVertex.z) maxVertex.z = Vertices[i].pos.z;
		}

		Ball.center.x = (minVertex.x + maxVertex.x) / 2;
		Ball.center.y = (minVertex.y + maxVertex.y) / 2;
		Ball.center.z = (minVertex.z + maxVertex.z) / 2;

		Ball.radius = ComputeRadius(Ball.center, Vertices);

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

		Vec3 minVertex = { +MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity };
		Vec3 maxVertex = { -MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity };
		for (int i = 0; i < Vertices.size(); i++)
		{
			if (Vertices[i].pos.x < minVertex.x) minVertex.x = Vertices[i].pos.x;
			if (Vertices[i].pos.y < minVertex.y) minVertex.y = Vertices[i].pos.y;
			if (Vertices[i].pos.z < minVertex.z) minVertex.z = Vertices[i].pos.z;

			if (Vertices[i].pos.x > maxVertex.x) maxVertex.x = Vertices[i].pos.x;
			if (Vertices[i].pos.y > maxVertex.y) maxVertex.y = Vertices[i].pos.y;
			if (Vertices[i].pos.z > maxVertex.z) maxVertex.z = Vertices[i].pos.z;
		}

		Ball.center.x = (minVertex.x + maxVertex.x) / 2;
		Ball.center.y = (minVertex.y + maxVertex.y) / 2;
		Ball.center.z = (minVertex.z + maxVertex.z) / 2;

		Ball.radius = ComputeRadius(Ball.center, Vertices);

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

		Vec3 minVertex = {+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity };
		Vec3 maxVertex = {-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity };
		for (int i = 0; i < Vertices.size(); i++)
		{
			if (Vertices[i].pos.x < minVertex.x) minVertex.x = Vertices[i].pos.x;
			if (Vertices[i].pos.y < minVertex.y) minVertex.y = Vertices[i].pos.y;
			if (Vertices[i].pos.z < minVertex.z) minVertex.z = Vertices[i].pos.z;

			if (Vertices[i].pos.x > maxVertex.x) maxVertex.x = Vertices[i].pos.x;
			if (Vertices[i].pos.y > maxVertex.y) maxVertex.y = Vertices[i].pos.y;
			if (Vertices[i].pos.z > maxVertex.z) maxVertex.z = Vertices[i].pos.z;
		}

		Ball.center.x = (minVertex.x + maxVertex.x) / 2;
		Ball.center.y = (minVertex.y + maxVertex.y) / 2;
		Ball.center.z = (minVertex.z + maxVertex.z) / 2;

		Ball.radius = ComputeRadius(Ball.center, Vertices);

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
