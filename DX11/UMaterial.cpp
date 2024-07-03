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

	is.read(reinterpret_cast<char*>(&material->Ambient), sizeof(XMFLOAT4));
	is.read(reinterpret_cast<char*>(&material->Diffuse), sizeof(XMFLOAT4));
	is.read(reinterpret_cast<char*>(&material->Specular), sizeof(XMFLOAT4));
	is.read(reinterpret_cast<char*>(&material->Reflect), sizeof(XMFLOAT4));

	return material;
}

void UMaterial::Save(UMaterial* material)
{
	if (material == nullptr) 
		return;

	std::ofstream os(material->m_ResourcePath, std::ios::binary);
	if (!os)
	{
		return;
	}

	os.write(reinterpret_cast<const char*>(&material->Ambient), sizeof(XMFLOAT4));
	os.write(reinterpret_cast<const char*>(&material->Diffuse), sizeof(XMFLOAT4));
	os.write(reinterpret_cast<const char*>(&material->Specular), sizeof(XMFLOAT4));
	os.write(reinterpret_cast<const char*>(&material->Reflect), sizeof(XMFLOAT4));
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
	
	if (ImGui::Button("Load Base Map")) {
		std::string filePath = EditorUtility::OpenFileDialog();
		if (!filePath.empty()) 
		{
			BaseMapSRV = LoadTexture(device, filePath);
			//baseMapPath = filePath;  
			//changed = true;
		}
	}
	if(BaseMapSRV != nullptr)
		ImGui::Image((void*)BaseMapSRV.Get(), ImVec2(128, 128)); 
	if (NormalMapSRV != nullptr)
		ImGui::Image((void*)NormalMapSRV.Get(), ImVec2(128, 128));

	changed |= ImGui::ColorEdit4("Ambient", reinterpret_cast<float*>(&Ambient));
	changed |= ImGui::ColorEdit4("Diffuse", reinterpret_cast<float*>(&Diffuse));
	changed |= ImGui::ColorEdit4("Specular", reinterpret_cast<float*>(&Specular));
	changed |= ImGui::ColorEdit4("Reflect", reinterpret_cast<float*>(&Reflect));

	if (changed) 
	{
		UMaterial::Save(this);
	}
}
