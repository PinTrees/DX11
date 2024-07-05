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
