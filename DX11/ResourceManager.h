#pragma once

class UMaterial;
class Mesh;
class SkinnedMesh;

class ResourceManager
{
	SINGLE_HEADER(ResourceManager)

private:
	ComPtr<ID3D11Device> m_Device;
	map<wstring, ComPtr<ID3D11ShaderResourceView>>	m_TextureSRV;
	map<string, shared_ptr<UMaterial>>				m_Materials;
	map<wstring, shared_ptr<Mesh>>					m_Meshs;
	map<wstring, shared_ptr<SkinnedMesh>>			m_SkinnedMeshs;

	map<string, shared_ptr<MeshFile>>			m_FbxModels;

public:
	void Init(ComPtr<ID3D11Device> device);

	ComPtr<ID3D11ShaderResourceView> LoadTexture(wstring filename);
	shared_ptr<UMaterial>	LoadMaterial(string filename);
	shared_ptr<Mesh>		LoadMesh(wstring filename);
	shared_ptr<SkinnedMesh> LoadSkinnedMesh(wstring filename);

	shared_ptr<MeshFile> LoadFbxModel(string filename);
};

