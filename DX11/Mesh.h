#pragma once
#include "MeshGeometry.h"
#include "TextureMgr.h"
#include "Vertex.h"
#include "LightHelper.h"

class Mesh
{
public:
	Mesh();
	Mesh(ComPtr<ID3D11Device> device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath);
	Mesh(ComPtr<ID3D11Device> device, const std::string& modelFilename);
	~Mesh();
	
	void Setup();

	uint32 SubsetCount;

	std::vector<Material> Mat;

	// ������� ���� - ���� ���� - UMaterial�� ����
	std::vector<ComPtr<ID3D11ShaderResourceView>> DiffuseMapSRV;
	std::vector<ComPtr<ID3D11ShaderResourceView>> NormalMapSRV;

	// Keep CPU copies of the mesh data to read from.  
	std::vector<Vertex::PosNormalTexTan2> Vertices;
	std::vector<USHORT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	MeshGeometry ModelMesh;
	BouncingBall Ball;

	string Name;
	wstring Path;

public:
	friend void from_json(const json& j, Mesh& m);
	friend void to_json(json& j, const Mesh& m);

	void from_byte(ifstream& inStream);
	void to_byte(ofstream& outStream);
};

// ������� ���� - ����
struct MeshInstance
{
	shared_ptr<class Mesh> Model;
	XMFLOAT4X4 World;
};