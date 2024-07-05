#pragma once

class UMaterial;
class Mesh;

class ResourceManager
{
	SINGLE_HEADER(ResourceManager)

private:
	ComPtr<ID3D11Device> m_Device;
	std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_TextureSRV;
	std::map<std::string, UMaterial*> m_Materials;
	std::map<std::wstring, shared_ptr<Mesh>> m_Meshs;

public:
	void Init(ComPtr<ID3D11Device> device);

	ComPtr<ID3D11ShaderResourceView> LoadTexture(wstring filename);
	UMaterial* LoadMaterial(string filename);
	shared_ptr<Mesh> LoadMesh(wstring filename);
};

