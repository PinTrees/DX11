#include "pch.h"
#include "UMaterial.h"

UMaterial::UMaterial()
	: m_ResourcePath("")
{
}

UMaterial::~UMaterial()
{
}

void UMaterial::Create(string fullPath)
{
	std::string filePath = fullPath + "/New Material.mat";
	std::ofstream ofs(filePath);
	if (ofs)
	{
		ofs << "Material data"; 
		ofs.close();
	}
}

UMaterial* UMaterial::Load(string fullPath)
{
	std::ifstream is(fullPath);
	if (!is)
	{
		return nullptr;
	}

	UMaterial* material = new UMaterial;
	material->m_ResourcePath = fullPath;

	

	return material;
}

void UMaterial::Save(UMaterial* material)
{
	if (material == nullptr)
		return;

	// JSON 객체 생성
	json j = *material;

	// JSON 파일로 저장
	std::ofstream os(material->m_ResourcePath);
	if (!os)
	{
		std::cerr << "파일을 열 수 없습니다: " << material->m_ResourcePath << std::endl;
		return;
	}

	os << j.dump(4); 
	os.close();
}

void UMaterial::SetBaseMap(TextureMgr texMgr, wstring fullPath)
{
	ComPtr<ID3D11ShaderResourceView> baseMapSRV = texMgr.CreateTexture(fullPath);
	BaseMapSRV = baseMapSRV;
}

void UMaterial::SetNormalMap(TextureMgr texMgr, wstring fullPath)
{
	ComPtr<ID3D11ShaderResourceView> normalMapSRV = texMgr.CreateTexture(fullPath);
	NormalMapSRV = normalMapSRV;
}

void UMaterial::OnInspectorGUI()
{
	bool changed = false;
	
	if (BaseMapSRV != nullptr) 
	{
		if (ImGui::ImageButton((void*)BaseMapSRV.Get(), ImVec2(64, 64)))
		{
			std::wstring filePath = EditorUtility::OpenFileDialog(m_ResourcePath);
			if (!filePath.empty())
			{
				BaseMapSRV = ResourceManager::GetI()->LoadTexture(filePath);
				m_BaseMapPath = filePath;
				changed = true;
			}
		}
	}
	else 
	{
		if (ImGui::Button("Load Base Map##image", ImVec2(64, 64)))
		{
			std::wstring filePath = EditorUtility::OpenFileDialog(m_ResourcePath);
			if (!filePath.empty()) 
			{
				BaseMapSRV = ResourceManager::GetI()->LoadTexture(filePath);
				m_BaseMapPath = filePath;
				changed = true;
			}
		}
	}

	if (NormalMapSRV != nullptr)
	{
		if (ImGui::ImageButton((void*)NormalMapSRV.Get(), ImVec2(64, 64)))
		{
			std::wstring filePath = EditorUtility::OpenFileDialog(m_ResourcePath);
			if (!filePath.empty())
			{
				NormalMapSRV = ResourceManager::GetI()->LoadTexture(filePath);
				m_NormalMapPath = filePath;
				changed = true;
			}
		}
	}
	else
	{
		if (ImGui::Button("Load Normal Map##image", ImVec2(64, 64)))
		{
			std::wstring filePath = EditorUtility::OpenFileDialog(m_ResourcePath);
			if (!filePath.empty())
			{
				NormalMapSRV = ResourceManager::GetI()->LoadTexture(filePath);
				m_BaseMapPath = filePath;
				changed = true;
			}
		}
	}

	changed |= ImGui::ColorEdit4("Ambient", reinterpret_cast<float*>(&Ambient));
	changed |= ImGui::ColorEdit4("Diffuse", reinterpret_cast<float*>(&Diffuse));
	changed |= ImGui::ColorEdit4("Specular", reinterpret_cast<float*>(&Specular));
	changed |= ImGui::ColorEdit4("Reflect", reinterpret_cast<float*>(&Reflect));

	if (changed) 
	{
		UMaterial::Save(this);
	}
}

void to_json(json& j, const UMaterial& m)
{
	j = json
	{
		{ "BaseMapPath", wstring_to_string(m.m_BaseMapPath) },
		{ "NormalMapPath", wstring_to_string(m.m_NormalMapPath) },
		{ "ResourcePath", m.m_ResourcePath },
		{ "Ambient", { m.Ambient.x, m.Ambient.y, m.Ambient.z, m.Ambient.w } },
		{ "Diffuse", { m.Diffuse.x, m.Diffuse.y, m.Diffuse.z, m.Diffuse.w } },
		{ "Specular", { m.Specular.x, m.Specular.y, m.Specular.z, m.Specular.w } },
		{ "Reflect", { m.Reflect.x, m.Reflect.y, m.Reflect.z, m.Reflect.w } }
	};
}
