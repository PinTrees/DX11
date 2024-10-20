#include "pch.h"
#include "SkinnedMesh.h"
#include "FBXLoader.h"
#include "SkinnedData.h"
#include "LoadM3d.h"
#include "MathHelper.h"
#include "File.h"
#include "MeshUtility.h"
#include "EditorGUI.h"

SkinnedMesh::SkinnedMesh()
{
}

SkinnedMesh::SkinnedMesh(ComPtr<ID3D11Device> device, const std::string& modelFilename)
{
	string modelfilepath = PathManager::GetI()->GetMovePathS(modelFilename);

	std::string fileExtension = File::GetExtension(modelfilepath); 

	if (fileExtension == "fbx" || fileExtension == "FBX")
	{
		std::vector<FbxMaterial> mats;

		FBXLoader fbxLoader;
		fbxLoader.LoadFBX(modelfilepath, Vertices, Indices, Subsets, mats, SkinnedData); 

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

		Ball.radius =  MeshUtility::ComputeBoundingRadius(Ball.center, Vertices);

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

SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::Setup()
{
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

	ModelMesh.SetVertices(Application::GetI()->GetDevice(), &Vertices[0], Vertices.size()); 
	ModelMesh.SetIndices(Application::GetI()->GetDevice(), &Indices[0], Indices.size()); 
	ModelMesh.SetSubsetTable(Subsets);

	SubsetCount = Subsets.size(); 
}

void SkinnedMesh::from_byte(ifstream& inStream)
{
	if (!inStream.is_open()) 
	{
		std::cerr << "File stream is not open!" << std::endl; 
		return;
	}

	// 1. �̸� �б� (���ڿ�)
	size_t nameLength = 0;
	inStream.read(reinterpret_cast<char*>(&nameLength), sizeof(size_t));

	Name.resize(nameLength);
	inStream.read(&Name[0], nameLength);

	// 2. Vertices �б� (PosNormalTexTanSkinned �迭)
	size_t vertexCount = 0;
	inStream.read(reinterpret_cast<char*>(&vertexCount), sizeof(size_t));

	Vertices.resize(vertexCount);
	inStream.read(reinterpret_cast<char*>(Vertices.data()), vertexCount * sizeof(Vertex::PosNormalTexTanSkinned));

	// 3. Indices �б� (USHORT �迭)
	size_t indexCount = 0;
	inStream.read(reinterpret_cast<char*>(&indexCount), sizeof(size_t));

	Indices.resize(indexCount);
	inStream.read(reinterpret_cast<char*>(Indices.data()), indexCount * sizeof(USHORT));

	// 4. Subsets �б�
	size_t subsetCount = 0;
	inStream.read(reinterpret_cast<char*>(&subsetCount), sizeof(size_t));

	Subsets.resize(subsetCount);
	for (auto& subset : Subsets)
	{
		// �̸� �б� (���ڿ�)
		size_t subsetNameLength = 0;
		inStream.read(reinterpret_cast<char*>(&subsetNameLength), sizeof(size_t));

		subset.Name.resize(subsetNameLength);
		inStream.read(&subset.Name[0], subsetNameLength);

		// �ٸ� �ʵ�� �б� (Id, MaterialIndex, VertexStart, VertexCount, FaceStart, FaceCount)
		inStream.read(reinterpret_cast<char*>(&subset.Id), sizeof(subset.Id));
		inStream.read(reinterpret_cast<char*>(&subset.MaterialIndex), sizeof(subset.MaterialIndex));
		inStream.read(reinterpret_cast<char*>(&subset.VertexStart), sizeof(subset.VertexStart));
		inStream.read(reinterpret_cast<char*>(&subset.VertexCount), sizeof(subset.VertexCount));
		inStream.read(reinterpret_cast<char*>(&subset.FaceStart), sizeof(subset.FaceStart));
		inStream.read(reinterpret_cast<char*>(&subset.FaceCount), sizeof(subset.FaceCount));
	}

	Setup(); 
}

void SkinnedMesh::to_byte(ofstream& outStream)
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
	outStream.write(reinterpret_cast<const char*>(Vertices.data()), vertexCount * sizeof(Vertex::PosNormalTexTanSkinned));

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


void SkinnedMeshlInstance::Update(float dt)
{
	TimePos += dt;
	Model->SkinnedData.GetFinalTransforms(ClipName, TimePos, FinalTransforms);

	// Loop animation
	if (TimePos > Model->SkinnedData.GetClipEndTime(ClipName))
		TimePos = 0.0f;
}



MeshFile::MeshFile()
{
}

MeshFile::MeshFile(string path)
{
	FullPath = PathManager::GetI()->GetMovePathS(path);
	Path = string_to_wstring(path);
	Name = PathManager::GetI()->GetFileName(path); 

	std::string fileExtension = File::GetExtension(FullPath);  

	if (fileExtension == "fbx" || fileExtension == "FBX")
	{
		FBXLoader fbxLoader;
		fbxLoader.LoadModelFbx(FullPath, this);  
	}
	if (fileExtension == "mesh" || fileExtension == "MESH")
	{
		std::ifstream inStream(FullPath, std::ios::binary); 
		try
		{
			from_byte(inStream);
		}
		catch (const std::exception&)
		{
		}
		inStream.close(); 
	}
}

MeshFile::~MeshFile()
{
}

void MeshFile::OnInspectorGUI()
{
	EditorGUI::LabelHeader(Name + " Fbx Import Setting");

	ImGui::Dummy(ImVec2(0, 4));

	if (EditorGUI::Button("Import Fbx Mesh"))
	{
		ImportFile(); 
	}
}

void MeshFile::ImportFile() 
{
	wstring savePath = Path + L".mesh";
	std::ofstream outStream(PathManager::GetI()->GetMovePathS(wstring_to_string(savePath)), std::ios::binary);

	if (!outStream)
	{
		std::cerr << "������ �� �� �����ϴ�: " << PathManager::GetI()->GetMovePathS(wstring_to_string(savePath)) << std::endl;
		return;
	}

	// FbxModel ���̳ʸ� ����
	to_byte(outStream);
	outStream.close();
}


void MeshFile::from_byte(ifstream& inStream)
{
	if (!inStream.is_open())
	{
		std::cerr << "File stream is not open!" << std::endl;
		return;
	}

	// �̸��� ���̸� �а�, �� ���̸�ŭ ���ڿ��� ����
// �̸��� ���̸� �а�, �� ���̸�ŭ ���ڿ��� ����
	size_t nameLength = 0;
	inStream.read(reinterpret_cast<char*>(&nameLength), sizeof(size_t));

	if (nameLength > 0) 
	{
		Name.resize(nameLength); 
		inStream.read(&Name[0], nameLength); 
	} 

	// Meshs ������ �а�, �� ������ŭ Mesh ��ü�� �����ϰ� �ε�
	size_t meshCount = 0;
	inStream.read(reinterpret_cast<char*>(&meshCount), sizeof(size_t));

	Meshs.resize(meshCount);
	for (size_t i = 0; i < meshCount; ++i)
	{
		Meshs[i] = new Mesh;
		Meshs[i]->from_byte(inStream);  // �� Mesh ��ü�� from_byte ȣ��
	}

	// SkinnedMeshs ������ �а�, �� ������ŭ SkinnedMesh ��ü�� �����ϰ� �ε�
	size_t skinnedMeshCount = 0;
	inStream.read(reinterpret_cast<char*>(&skinnedMeshCount), sizeof(size_t));

	SkinnedMeshs.resize(skinnedMeshCount);
	for (size_t i = 0; i < skinnedMeshCount; ++i)
	{
		SkinnedMeshs[i] = new SkinnedMesh; 
		SkinnedMeshs[i]->from_byte(inStream);  // �� SkinnedMesh ��ü�� from_byte ȣ��
	}
}

void MeshFile::to_byte(ofstream& outStream)
{
	if (!outStream.is_open())
	{
		std::cerr << "File stream is not open!" << std::endl;
		return;
	}

	size_t nameLength = Name.size();
	outStream.write(reinterpret_cast<const char*>(&nameLength), sizeof(size_t));
	outStream.write(Name.c_str(), nameLength);

	size_t meshCount = Meshs.size();
	outStream.write(reinterpret_cast<char*>(&meshCount), sizeof(size_t));

	for (const auto& mesh : Meshs)
	{
		mesh->to_byte(outStream);
	}

	size_t skinnedMeshCount = SkinnedMeshs.size();
	outStream.write(reinterpret_cast<char*>(&skinnedMeshCount), sizeof(size_t)); 

	for (const auto& skinnedMesh : SkinnedMeshs)
	{
		skinnedMesh->to_byte(outStream);
	}
}

void from_json(const json& j, MeshFile& m)
{

}

void to_json(json& j, const MeshFile& m)
{
	SERIALIZE_TYPE(j, MeshFile);
	SERIALIZE_STRING(j, m.Name, "name");

	json meshes_json = json::array();
	for (const auto& mesh : m.Meshs)
	{
		json mesh_json = *mesh;
		meshes_json.push_back(mesh_json); 
	}
	for (const auto& skinnedMesh : m.SkinnedMeshs)
	{

	}
	j["meshes"] = meshes_json; 
}
