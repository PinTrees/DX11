#include "pch.h"
#include "MeshRenderer.h"
#include "Effects.h"
#include "MathHelper.h"

MeshRenderer::MeshRenderer()
	: m_pMaterial(nullptr)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render()
{
	if (m_Mesh == nullptr)
		return;

	auto deviceContext = Application::GetI()->GetDeviceContext();
	ComPtr<ID3DX11EffectTechnique> tech = Effects::NormalMapFX->Light3TexTech;
	//Light3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		Transform* transform = m_pGameObject->GetComponent<Transform>();
		
		XMMATRIX world = transform->GetWorldMatrix();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * RenderManager::GetI()->cameraViewProjectionMatrix;

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world * RenderManager::GetI()->shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		for (uint32 subset = 0; subset < m_Mesh->SubsetCount; ++subset)
		{
			if (m_pMaterial == nullptr)
			{
				Effects::NormalMapFX->SetMaterial(m_Mesh->Mat[subset]);
				//Effects::NormalMapFX->SetDiffuseMap(m_Mesh->DiffuseMapSRV[subset].Get());
				//Effects::NormalMapFX->SetNormalMap(m_Mesh->NormalMapSRV[subset].Get());
			}
			else
			{
				Effects::NormalMapFX->SetMaterial(m_pMaterial->Mat);
				Effects::NormalMapFX->SetDiffuseMap(m_pMaterial->GetBaseMapSRV());
				Effects::NormalMapFX->SetNormalMap(m_pMaterial->GetNormalMapSRV());
			}

			tech->GetPassByIndex(p)->Apply(0, deviceContext);
			m_Mesh->ModelMesh.Draw(deviceContext, subset);
		}
	}
}

void MeshRenderer::RenderShadow()
{
	if (m_Mesh == nullptr)
		return;

	Transform* transform = m_pGameObject->GetComponent<Transform>();
	auto deviceContext = Application::GetI()->GetDeviceContext();

	ComPtr<ID3DX11EffectTechnique> tech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ComPtr<ID3DX11EffectTechnique> alphaClippedTech = Effects::BuildShadowMapFX->BuildShadowMapAlphaClipTech;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		world = transform->GetWorldMatrix();
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * RenderManager::GetI()->directinalLightViewProjection;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(::XMMatrixScaling(1.0f, 1.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, deviceContext);

		for (uint32 subset = 0; subset < m_Mesh->SubsetCount; ++subset)
		{
			m_Mesh->ModelMesh.Draw(deviceContext, subset);
		}
	}
}

void MeshRenderer::RenderShadowNormal()
{
	if (m_Mesh == nullptr)
		return;

	auto deviceContext = Application::GetI()->GetDeviceContext();
	Transform* transform = m_pGameObject->GetComponent<Transform>();
	ComPtr<ID3DX11EffectTechnique> tech = Effects::SsaoNormalDepthFX->NormalDepthTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		world = transform->GetWorldMatrix();
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world * RenderManager::GetI()->cameraViewMatrix;
		worldInvTransposeView = worldInvTranspose * RenderManager::GetI()->cameraViewMatrix;
		worldViewProj = world * RenderManager::GetI()->cameraViewProjectionMatrix;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, deviceContext);
		for (uint32 subset = 0; subset < m_Mesh->SubsetCount; ++subset)
		{
			m_Mesh->ModelMesh.Draw(deviceContext, subset);
		}
	}
}

void MeshRenderer::OnInspectorGUI()
{
	ImGui::Text("Mesh Renderer");

	// 같은 라인에 배치
	ImGui::SameLine();
	ImGui::PushItemWidth(-100);
	ImGui::Text("%s", m_Mesh ? "Meshs" : "None");
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Button("Select##Mesh"))
	{
		std::wstring filePath = EditorUtility::OpenFileDialog(Application::GetDataPath(), L"Mesh", { L"fbx" });
		if (!filePath.empty())
		{
			m_Mesh = ResourceManager::GetI()->LoadMesh(filePath);
			if (m_Mesh)
			{
				m_MeshPath = filePath;
			}
		}
	}



	if (m_pMaterial) {
		ImGui::Text("Selected Material: %s", m_MaterialPath);
	}
	else {
		ImGui::Text("Selected Material: None");
	}

	if (ImGui::Button("Select Material")) {

		std::wstring filePath = EditorUtility::OpenFileDialog(L"", "Material", {"mat"});
		if (!filePath.empty())
		{
			m_pMaterial = ResourceManager::GetI()->LoadMaterial(wstring_to_string(filePath));
			if (m_pMaterial) 
			{
				m_MaterialPath = filePath;
			}
		}
	}
}


GENERATE_COMPONENT_FUNC_TOJSON(MeshRenderer)
{
	json j;
	j["type"] = "MeshRenderer";
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(MeshRenderer)
{
}