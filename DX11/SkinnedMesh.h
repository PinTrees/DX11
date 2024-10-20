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
	SkinnedMesh(ComPtr<ID3D11Device> device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath);
	SkinnedMesh(ComPtr<ID3D11Device> device, const std::string& modelFilename);
	~SkinnedMesh();

	void to_byte(ofstream& outStream);

	uint32					SubsetCount;
	std::vector<Material>	Mat;

	std::vector<ComPtr<ID3D11ShaderResourceView>> DiffuseMapSRV;
	std::vector<ComPtr<ID3D11ShaderResourceView>> NormalMapSRV;

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
	wstring				Path;  
	string				Name;

public:
	void OnInspectorGUI();

public:
	void Save(); 
	friend void from_json(const json& j, MeshFile& m);
	friend void to_json(json& j, const MeshFile& m);

	void to_byte(ofstream& outStream);
};