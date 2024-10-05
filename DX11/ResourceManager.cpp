#include "pch.h"
#include "ResourceManager.h"
#include "Utils.h"
#include "SkinnedMesh.h"

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

	wstring path = PathManager::GetI()->GetMovePathW(filename);

	if (m_TextureSRV.find(filename) != m_TextureSRV.end())
	{
		srv = m_TextureSRV[filename];
	}
	else
	{
		srv = Utils::LoadTexture(m_Device, path.c_str());
		m_TextureSRV[filename] = srv;
	}

	return srv;
}

shared_ptr<UMaterial> ResourceManager::LoadMaterial(string filename)
{
	shared_ptr<UMaterial> material = nullptr;

	if (m_Materials.find(filename) != m_Materials.end())
	{
		material = m_Materials[filename];
	}
	else
	{
		UMaterial* umat = UMaterial::Load(filename);
		if (umat != nullptr)
		{
			shared_ptr<UMaterial> material(umat);
			m_Materials[filename] = material;
			return material;
		}
		else
		{
			return nullptr;
		}
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
		mesh = make_shared<Mesh>(Application::GetI()->GetDevice(), Utils::wstring_to_string(filename));
		m_Meshs[filename] = mesh;
	}

	return mesh;
}

shared_ptr<SkinnedMesh> ResourceManager::LoadSkinnedMesh(wstring filename)
{
	shared_ptr<SkinnedMesh> mesh = nullptr;

	if (m_SkinnedMeshs.find(filename) != m_SkinnedMeshs.end())
	{
		mesh = m_SkinnedMeshs[filename];
	}
	else
	{
		mesh = make_shared<SkinnedMesh>(Application::GetI()->GetDevice(), Utils::wstring_to_string(filename));
		m_SkinnedMeshs[filename] = mesh;
	}

	return mesh;
}
