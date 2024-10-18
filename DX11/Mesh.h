#pragma once
#include "MeshGeometry.h"
#include "TextureMgr.h"
#include "Vertex.h"
#include "LightHelper.h"

class Mesh
{
private:
	float ComputeRadius(const Vec3& center, const std::vector<Vertex::PosNormalTexTan2>& vertices);
public:
	Mesh(ComPtr<ID3D11Device> device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath);
	Mesh(ComPtr<ID3D11Device> device, const std::string& modelFilename);
	~Mesh();

	uint32 SubsetCount;

	std::vector<Material> Mat;

	// 사용하지 않음 - 제거 예정 - UMaterial로 이전
	std::vector<ComPtr<ID3D11ShaderResourceView>> DiffuseMapSRV;
	std::vector<ComPtr<ID3D11ShaderResourceView>> NormalMapSRV;

	// Keep CPU copies of the mesh data to read from.  
	std::vector<Vertex::PosNormalTexTan2> Vertices;
	std::vector<USHORT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	MeshGeometry ModelMesh;
	BouncingBall Ball;

	wstring Path;
};

// 사용하지 않음 - 제거
struct MeshInstance
{
	shared_ptr<class Mesh> Model;
	XMFLOAT4X4 World;
};