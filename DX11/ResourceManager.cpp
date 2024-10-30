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

shared_ptr<Mesh> ResourceManager::LoadMesh(wstring filename, int index)
{
	tuple<wstring, int> key = make_tuple(filename, index); 
	shared_ptr<Mesh> mesh = nullptr;

	if (m_Meshs.find(key) != m_Meshs.end()) 
	{
		mesh = m_Meshs[key]; 
	}
	else
	{
		const auto& meshFile = LoadMeshFile(wstring_to_string(filename));
		if (meshFile == nullptr)
			return nullptr;

		if (meshFile->Meshs.size() > index) 
		{
			mesh = shared_ptr<Mesh>(meshFile->Meshs[index]); 
			m_Meshs[key] = mesh; 
		}
	}

	return mesh;
}

shared_ptr<SkinnedMesh> ResourceManager::LoadSkinnedMesh(wstring filename, int index)
{
	shared_ptr<SkinnedMesh> mesh = nullptr;

	if (m_SkinnedMeshs.find(filename) != m_SkinnedMeshs.end())
	{
		mesh = m_SkinnedMeshs[filename];
	} 
	else
	{
		auto meshFile = LoadMeshFile(wstring_to_string(filename));  
		if (meshFile)
		{
			if (meshFile->SkinnedMeshs.size() > index)
			{
				mesh = std::shared_ptr<SkinnedMesh>(meshFile->SkinnedMeshs[index]);
				m_SkinnedMeshs[filename] = mesh;    
			}
		}
	}

	return mesh;
}

shared_ptr<MeshFile> ResourceManager::LoadFbxModel(string filename)
{
	shared_ptr<MeshFile> model = nullptr;

	if (m_FbxModels.find(filename) != m_FbxModels.end())
	{
		model = m_FbxModels[filename];
	}
	else
	{
		MeshFile* meshFile = MeshFile::LoadFromFbxFile(filename);
		model.reset(meshFile);
		m_FbxModels[filename] = model;
	}

	return model;
}

shared_ptr<MeshFile> ResourceManager::LoadMeshFile(string filename)
{
	shared_ptr<MeshFile> model = nullptr;

	if (m_FbxModels.find(filename) != m_FbxModels.end())
	{
		model = m_FbxModels[filename];
	}
	else
	{
		MeshFile* meshFile = MeshFile::LoadFromMetaFile(filename); 
		// 메타파일 읽기 오류
		if (meshFile == nullptr)
			return nullptr;

		model.reset(meshFile);
		m_FbxModels[filename] = model;  
	}

	return model;
}

shared_ptr<AnimationClip> ResourceManager::LoadAnimationClip(string filename, int index)
{
	tuple<string, int> key = make_tuple(filename, index);
	shared_ptr<AnimationClip> animationClip;

	if (m_AnimationClips.find(key) != m_AnimationClips.end())
	{
		animationClip = m_AnimationClips[key];
	}
	else
	{
		// Load MeshFile;
		shared_ptr<MeshFile> meshFile = ResourceManager::GetI()->LoadMeshFile(filename);
		if (meshFile == nullptr)
			return nullptr;

		m_AnimationClips[key] = meshFile->SkinnedData.AnimationClips[index]; 
	}

	return animationClip;
}
