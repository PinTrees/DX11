#include "pch.h"
#include "MeshRenderer.h"
#include "Effects.h"
#include "ShaderSetting.h"
#include "MathHelper.h"
#include "InstancingBuffer.h"
#include "EditorGUI.h"

MeshRenderer::MeshRenderer()
	: m_pMaterials({}),
	m_MeshSubsetIndex(0)
{
	m_InspectorTitleName = "MeshRenderer";
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render()
{
	if (m_Mesh == nullptr)
		return;

	auto deviceContext = Application::GetI()->GetDeviceContext();
	ComPtr<ID3DX11EffectTechnique> tech = Effects::InstancedBasicFX->Tech;

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		if (m_MeshSubsetIndex >= m_Mesh->Subsets.size())
			break;

		Transform* transform = m_pGameObject->GetComponent<Transform>();
		
		XMMATRIX world = transform->GetWorldMatrix();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX vi = RenderManager::GetI()->cameraViewMatrix;
		XMMATRIX pr = RenderManager::GetI()->cameraProjectionMatrix;
		XMMATRIX worldViewProj = world * RenderManager::GetI()->cameraViewProjectionMatrix;

		Effects::InstancedBasicFX->SetWorld(world);
		Effects::InstancedBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicFX->SetViewProj(RenderManager::GetI()->cameraViewProjectionMatrix);
		Effects::InstancedBasicFX->SetWorldViewProj(worldViewProj);
		Effects::InstancedBasicFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
		Effects::InstancedBasicFX->SetShadowTransform(world * RenderManager::GetI()->shadowTransform);
		Effects::InstancedBasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		for (auto mat : m_pMaterials)
		{
			if (mat != nullptr)
			{
				Effects::InstancedBasicFX->SetMaterial(mat->Mat);
				Effects::InstancedBasicFX->SetDiffuseMap(mat->GetBaseMapSRV());
				Effects::InstancedBasicFX->SetNormalMap(mat->GetNormalMapSRV());
				Effects::InstancedBasicFX->SetShaderSetting(mat->GetShaderSetting());
			}
			else
			{
				ShaderSetting shaderSetting;
				Effects::InstancedBasicFX->SetMaterial(m_Mesh->Mat[m_MeshSubsetIndex]);
				Effects::InstancedBasicFX->SetShaderSetting(shaderSetting);
			}
		}

		tech->GetPassByIndex(p)->Apply(0, deviceContext);
		m_Mesh->ModelMesh.Draw(deviceContext, m_MeshSubsetIndex);
	}
}

void MeshRenderer::RenderInstancing(shared_ptr<class InstancingBuffer>& buffer)
{
	if (m_Mesh == nullptr)
		return;

	auto deviceContext = Application::GetI()->GetDeviceContext();
	ComPtr<ID3DX11EffectTechnique> tech = Effects::InstancedBasicFX->InstancingTech;
	//Light3TexTech;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(InputLayouts::InstancedBasic.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		if (m_MeshSubsetIndex >= m_Mesh->Subsets.size())
			break;

		Transform* transform = m_pGameObject->GetComponent<Transform>();

		XMMATRIX world = transform->GetWorldMatrix();
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * RenderManager::GetI()->cameraViewProjectionMatrix;
		XMMATRIX View = RenderManager::GetI()->cameraViewMatrix;
		XMMATRIX Proj = RenderManager::GetI()->cameraProjectionMatrix;
		XMMATRIX ViewProj = RenderManager::GetI()->cameraViewProjectionMatrix;

		Effects::InstancedBasicFX->SetWorld(world);
		Effects::InstancedBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicFX->SetWorldViewProj(worldViewProj);
		Effects::InstancedBasicFX->SetWorldViewProjTex(worldViewProj * toTexSpace);

		Effects::InstancedBasicFX->SetView(View);
		Effects::InstancedBasicFX->SetProj(Proj);
		Effects::InstancedBasicFX->SetViewProj(View* Proj);
		Effects::InstancedBasicFX->SetViewProjTex(ViewProj * toTexSpace);
		Effects::InstancedBasicFX->SetShadowTransform(RenderManager::GetI()->shadowTransform);
		Effects::InstancedBasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		for (auto mat : m_pMaterials)
		{
			if (mat != nullptr)
			{
				Effects::InstancedBasicFX->SetMaterial(mat->Mat);
				Effects::InstancedBasicFX->SetDiffuseMap(mat->GetBaseMapSRV());
				Effects::InstancedBasicFX->SetNormalMap(mat->GetNormalMapSRV());
				Effects::InstancedBasicFX->SetShaderSetting(mat->GetShaderSetting());
			}
			else
			{
				ShaderSetting shaderSetting;
				Effects::InstancedBasicFX->SetMaterial(m_Mesh->Mat[m_MeshSubsetIndex]);
				Effects::InstancedBasicFX->SetShaderSetting(shaderSetting);
			}
		}

		tech->GetPassByIndex(p)->Apply(0, deviceContext);

		buffer->PushData(deviceContext);

		// 인스턴싱, ModelMesh 클래스에 InstancingDraw함수 적용
		m_Mesh->ModelMesh.InstancingDraw(deviceContext, m_MeshSubsetIndex, buffer->GetCount());
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

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
	deviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan.Get()); 

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		if (m_MeshSubsetIndex >= m_Mesh->Subsets.size())
			break;

		world = transform->GetWorldMatrix();
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * RenderManager::GetI()->directinalLightViewProjection;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(::XMMatrixScaling(1.0f, 1.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, deviceContext);
		m_Mesh->ModelMesh.Draw(deviceContext, m_MeshSubsetIndex);
	}
}

void MeshRenderer::RenderShadowInstancing(shared_ptr<class InstancingBuffer>& buffer)
{
	if (m_Mesh == nullptr)
		return;

	auto deviceContext = Application::GetI()->GetDeviceContext();

	ComPtr<ID3DX11EffectTechnique> tech = Effects::BuildShadowMapFX->BuildShadowMapInstancingTech;
	ComPtr<ID3DX11EffectTechnique> alphaClippedTech = Effects::BuildShadowMapFX->BuildShadowMapAlphaClipInstancingTech;

	XMMATRIX ViewProj;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(InputLayouts::InstancedBasic.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		if (m_MeshSubsetIndex >= m_Mesh->Subsets.size())
			break;

		ViewProj = RenderManager::GetI()->directinalLightViewProjection;

		Effects::BuildShadowMapFX->SetViewProj(ViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(::XMMatrixScaling(1.0f, 1.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, deviceContext);

		buffer->PushData(deviceContext);

		m_Mesh->ModelMesh.InstancingDraw(deviceContext, m_MeshSubsetIndex, buffer->GetCount());
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
		if (m_MeshSubsetIndex >= m_Mesh->Subsets.size())
			break;

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
		m_Mesh->ModelMesh.Draw(deviceContext, m_MeshSubsetIndex);
	}
}

void MeshRenderer::RenderShadowNormalInstancing(shared_ptr<class InstancingBuffer>& buffer)
{
	if (m_Mesh == nullptr)
		return;

	auto deviceContext = Application::GetI()->GetDeviceContext();
	ComPtr<ID3DX11EffectTechnique> tech = Effects::SsaoNormalDepthFX->NormalDepthInstancingTech;

	XMMATRIX View;
	XMMATRIX Proj;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(InputLayouts::InstancedBasic.Get());

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		if (m_MeshSubsetIndex >= m_Mesh->Subsets.size())
			break;

		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldView;
		XMMATRIX worldInvTransposeView;
		XMMATRIX worldViewProj;
		Transform* transform = m_pGameObject->GetComponent<Transform>();

		world = transform->GetWorldMatrix();
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView = world * RenderManager::GetI()->cameraViewMatrix;
		worldInvTransposeView = worldInvTranspose * RenderManager::GetI()->cameraViewMatrix;
		worldViewProj = world * RenderManager::GetI()->cameraViewProjectionMatrix;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		View = RenderManager::GetI()->cameraViewMatrix;
		Proj = RenderManager::GetI()->cameraProjectionMatrix;

		Effects::SsaoNormalDepthFX->SetView(View);
		Effects::SsaoNormalDepthFX->SetProj(Proj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, deviceContext);

		buffer->PushData(deviceContext);

		m_Mesh->ModelMesh.InstancingDraw(deviceContext, m_MeshSubsetIndex, buffer->GetCount());
	}
}

void MeshRenderer::OnInspectorGUI()
{
	auto changed = EditorGUI::MeshField("Mesh", m_Mesh, m_MeshSubsetIndex);

	EditorGUI::LabelHeader("Materials");

	// 배열의 각 요소의 높이를 계산
	float itemHeight = 24;
	float totalHeight = itemHeight * m_pMaterials.size(); // 자식 요소들의 총 높이 

	ImGui::Dummy(ImVec2(4, 0));
	ImGui::SameLine();

	if (ImGui::BeginChild("Materials List Box",
		ImVec2(ImGui::GetContentRegionAvail().x - 8, 4 + totalHeight), true,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0)); // X, Y 간격을 2로 설정
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // 프레임 패딩도 2로 설정

		for (int i = 0; i < m_pMaterials.size(); ++i)
		{
			ImGui::PushID(i);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			bool isDelete = EditorGUI::Button("-", Vec2(20, 20), Color(0.20f, 0.20f, 0.20f, 1.0f));
			ImGui::PopStyleVar();

			if (isDelete)
			{
				m_pMaterials.erase(m_pMaterials.begin() + i);
				m_MaterialPaths.erase(m_MaterialPaths.begin() + i);
				--i;

				ImGui::PopID();
				continue;
			}

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(0, 24));
			ImGui::SameLine();
			EditorGUI::MaterialField("Element " + to_string(i), m_pMaterials[i], m_MaterialPaths[i]);

			ImGui::PopID();
		}

		ImGui::PopStyleVar(2);
	}
	ImGui::EndChild();

	ImGui::Dummy(ImVec2(4, 0));
	ImGui::SameLine();
	if (EditorGUI::Button("Add"))
	{
		m_pMaterials.push_back(nullptr);
		m_MaterialPaths.push_back(L"");
	}
}

GENERATE_COMPONENT_FUNC_TOJSON(MeshRenderer)
{
	json j;

	SERIALIZE_TYPE(j, MeshRenderer);
	j["shaderPath"] = wstring_to_string(m_ShaderPath);
	j["meshPath"] = wstring_to_string(m_MeshPath);
	//j["materialPath"] = wstring_to_string(m_MaterialPath);
	j["subsetIndex"] = m_MeshSubsetIndex; 
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(MeshRenderer)
{
	if (j.contains("shaderPath"))
	{
		m_ShaderPath = string_to_wstring(j.at("shaderPath").get<string>());
	}
	if (j.contains("meshPath"))
	{
		m_MeshPath = string_to_wstring(j.at("meshPath").get<string>());
		m_Mesh = ResourceManager::GetI()->LoadMesh(m_MeshPath);
	}
	if (j.contains("materialPath"))
	{
		//m_MaterialPath = string_to_wstring(j.at("materialPath").get<string>());
		//m_pMaterial = ResourceManager::GetI()->LoadMaterial(wstring_to_string(m_MaterialPath));
	}
	if (j.contains("subsetIndex"))
	{
		m_MeshSubsetIndex = j.at("subsetIndex").get<int>(); 
	}
}