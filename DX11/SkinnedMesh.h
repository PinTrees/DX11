#pragma once
#include "TextureMgr.h"
#include "Vertex.h"
#include "MeshGeometry.h"
#include "SkinnedData.h"
#include "Vertex.h"
#include "LightHelper.h"

class SkinnedMesh;

// 사용하지 않음 - 제거
struct SkinnedMeshlInstance
{
	shared_ptr<SkinnedMesh> Model;
	float TimePos;
	std::string ClipName;
	XMFLOAT4X4 World;
	std::vector<XMFLOAT4X4> FinalTransforms;

	void Update(float dt);
};

class SkinnedMesh
{
public: 
	SkinnedMesh();
	~SkinnedMesh();

public:
	void Setup();
	void from_byte(ifstream& inStream);
	void to_byte(ofstream& outStream);

	uint32					SubsetCount;
	std::vector<Material>	Mat;

	// Keep CPU copies of the mesh data to read from.  
	std::vector<Vertex::PosNormalTexTanSkinned> Vertices;
	std::vector<USHORT>							Indices;
	std::vector<MeshGeometry::Subset>			Subsets;

	MeshGeometry	ModelMesh;
	SkinnedData		SkinnedData;

	BouncingBall	Ball;
	wstring			Path;
	string			Name;
};

class MeshFile
{
public:
	MeshFile();
	MeshFile(string path);  
	~MeshFile();

	vector<SkinnedMesh*> SkinnedMeshs;
	vector<Mesh*>		Meshs; 
	SkinnedData			SkinnedData;

	wstring				Path;  
	string				FullPath;
	string				Name;

public:
	void OnInspectorGUI();

public:
	void ImportFile(); 
	void ImportAnimation();

	void from_byte(ifstream& inStream);
	void to_byte(ofstream& outStream);
};