#pragma once

class TextureMgr;

class UMaterial
{
private:
	string m_ResourcePath;

	string m_BaseMapPath;
	string m_NormalMapPath;
	ComPtr<ID3D11ShaderResourceView> BaseMapSRV;
	ComPtr<ID3D11ShaderResourceView> NormalMapSRV;
	
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;

public:
	UMaterial();
	~UMaterial();

public:
	static void Create(string fullPath);
	static UMaterial* Load(string fullPath);
	static void Save(UMaterial* material);

public:
	void SetBaseMap(TextureMgr texMgr, wstring fullPath);
	void SetNormalMap(TextureMgr texMgr, wstring fullPath);

	ID3D11ShaderResourceView* GetBaseMapSRV() { return BaseMapSRV.Get(); }
	ID3D11ShaderResourceView* GetNormalMapSRV() { return NormalMapSRV.Get(); }

public:
	void OnInspectorGUI();
};

