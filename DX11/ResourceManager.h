#pragma once

class UMaterial;
class Mesh;
class SkinnedMesh;

class ResourceManager
{
	SINGLE_HEADER(ResourceManager)

private:
	ComPtr<ID3D11Device> m_Device;
	std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_TextureSRV;
	std::map<std::string, UMaterial*> m_Materials;
	std::map<std::wstring, shared_ptr<Mesh>> m_Meshs;
	std::map<std::wstring, shared_ptr<SkinnedMesh>> m_SkinnedMeshs;

public:
	void Init(ComPtr<ID3D11Device> device);

	ComPtr<ID3D11ShaderResourceView> LoadTexture(wstring filename);
	UMaterial* LoadMaterial(string filename);
	shared_ptr<Mesh> LoadMesh(wstring filename);
	shared_ptr<SkinnedMesh> LoadSkinnedMesh(wstring filename);
};

