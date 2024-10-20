#include "pch.h"
#include "Mesh.h"
#include "FBXLoader.h"
#include "LoadM3d.h"
#include "MathHelper.h"
#include "File.h"
#include "MeshUtility.h"


Mesh::Mesh()
{
}

Mesh::Mesh(ComPtr<ID3D11Device> device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath)
{
	string modelfilepath = PathManager::GetI()->GetMovePathS(modelFilename);

	std::string fileExtension = File::GetExtension(modelfilepath);

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

		Ball.radius = MeshUtility::ComputeBoundingRadius(Ball.center, Vertices);

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

		Ball.radius = MeshUtility::ComputeBoundingRadius(Ball.center, Vertices);

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

	std::string fileExtension = File::GetExtension(modelfilepath);

	if (fileExtension == "fbx" || fileExtension == "FBX")
	{
		std::vector<FbxMaterial> mats;

		FBXLoader fbxLoader;
		fbxLoader.LoadFBX(modelfilepath, Vertices, Indices, Subsets, mats);

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

		Ball.radius = MeshUtility::ComputeBoundingRadius(Ball.center, Vertices);

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

void Mesh::to_byte(ofstream& outStream)
{
	if (!outStream.is_open())
	{
		std::cerr << "File stream is not open!" << std::endl;
		return;
	}

	// 1. �̸� ���� (���ڿ�)
	size_t nameLength = Name.size();
	outStream.write(reinterpret_cast<const char*>(&nameLength), sizeof(size_t));
	outStream.write(Name.c_str(), nameLength);

	// 2. Vertices ���� (PosNormalTexTan2 �迭)
	size_t vertexCount = Vertices.size();
	outStream.write(reinterpret_cast<const char*>(&vertexCount), sizeof(size_t));
	outStream.write(reinterpret_cast<const char*>(Vertices.data()), vertexCount * sizeof(Vertex::PosNormalTexTan2));

	// 3. Indices ���� (USHORT �迭)
	size_t indexCount = Indices.size();
	outStream.write(reinterpret_cast<const char*>(&indexCount), sizeof(size_t));
	outStream.write(reinterpret_cast<const char*>(Indices.data()), indexCount * sizeof(USHORT));

	// 4. Subsets ����
	size_t subsetCount = Subsets.size();
	outStream.write(reinterpret_cast<const char*>(&subsetCount), sizeof(size_t));

	for (const auto& subset : Subsets)
	{
		// �̸� ���� (���ڿ�)
		size_t subsetNameLength = subset.Name.size();
		outStream.write(reinterpret_cast<const char*>(&subsetNameLength), sizeof(size_t));
		outStream.write(subset.Name.c_str(), subsetNameLength);

		// �ٸ� �ʵ�� ���� (Id, MaterialIndex, VertexStart, VertexCount, FaceStart, FaceCount)
		outStream.write(reinterpret_cast<const char*>(&subset.Id), sizeof(subset.Id));
		outStream.write(reinterpret_cast<const char*>(&subset.MaterialIndex), sizeof(subset.MaterialIndex));
		outStream.write(reinterpret_cast<const char*>(&subset.VertexStart), sizeof(subset.VertexStart));
		outStream.write(reinterpret_cast<const char*>(&subset.VertexCount), sizeof(subset.VertexCount));
		outStream.write(reinterpret_cast<const char*>(&subset.FaceStart), sizeof(subset.FaceStart));
		outStream.write(reinterpret_cast<const char*>(&subset.FaceCount), sizeof(subset.FaceCount));
	}
}

void from_json(const json& j, Mesh& m)
{
}

void to_json(json& j, const Mesh& m)
{
	SERIALIZE_TYPE(j, Mesh);
	SERIALIZE_STRING(j, m.Name, "name");
	SERIALIZE_PosNormalTexTan2_ARRAY(j, m.Vertices, "vertices");
	SERIALIZE_USHORT_ARRAY(j, m.Indices, "indices");

	json array_json = json::array();
	for (MeshGeometry::Subset subset : m.Subsets)
	{
		json subset_json;
		subset_json["name"] = subset.Name;
		subset_json["id"] = subset.Id;
		subset_json["materialIndex"] = subset.MaterialIndex;
		subset_json["vertexStart"] = subset.VertexStart;
		subset_json["vertexCount"] = subset.VertexCount;
		subset_json["faceStart"] = subset.FaceStart;
		subset_json["faceCount"] = subset.FaceCount;
		array_json.push_back(subset_json);
	}
	j["subsets"] = array_json;
}
