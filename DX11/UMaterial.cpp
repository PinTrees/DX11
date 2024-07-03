#include "pch.h"
#include "UMaterial.h"

UMaterial::UMaterial()
{
}

UMaterial::~UMaterial()
{
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
	ImGui::ColorEdit4("Ambient", reinterpret_cast<float*>(&Ambient));
	ImGui::ColorEdit4("Diffuse", reinterpret_cast<float*>(&Diffuse));
	ImGui::ColorEdit4("Specular", reinterpret_cast<float*>(&Specular));
	ImGui::ColorEdit4("Reflect", reinterpret_cast<float*>(&Reflect));
}
