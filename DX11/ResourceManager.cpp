#include "pch.h"
#include "ResourceManager.h"
#include "Utils.h"

SINGLE_BODY(ResourceManager)

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	m_TextureSRV.clear();
}

void ResourceManager::Init(ComPtr<ID3D11Device> device)
{
	m_Device = device;
}

ComPtr<ID3D11ShaderResourceView> ResourceManager::LoadTexture(wstring filename)
{
	ComPtr<ID3D11ShaderResourceView> srv;

	if (m_TextureSRV.find(filename) != m_TextureSRV.end())
	{
		srv = m_TextureSRV[filename];
	}
	else
	{
		srv = Utils::LoadTexture(m_Device, filename.c_str());
		m_TextureSRV[filename] = srv;
	}

	return srv;
}

UMaterial* ResourceManager::LoadMaterial(string filename)
{
	UMaterial* material = nullptr;

	if (m_Materials.find(filename) != m_Materials.end())
	{
		material = m_Materials[filename];
	}
	else
	{
		material = UMaterial::Load(filename);
		m_Materials[filename] = material;
	}

	return material;
}

shared_ptr<Mesh> ResourceManager::LoadMesh(wstring filename)
{
	shared_ptr<Mesh> mesh = nullptr;

	if (m_Meshs.find(filename) != m_Meshs.end())
	{
		mesh = m_Meshs[filename];
	}
	else
	{
		mesh = make_shared<Mesh>(Application::GetI()->GetDevice(), wstring_to_string(filename));
		m_Meshs[filename] = mesh;
	}

	return mesh;
}
