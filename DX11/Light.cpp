#include "pch.h"
#include "App.h"
#include "Light.h"
#include "LightManager.h"
#include "Transform.h"
#include "SceneViewManager.h"
#include "SceneEditorWindow.h"
#include "EditorCamera.h"
#include "Camera.h"

Light::Light()
{
	m_InspectorTitleName = "Light";
	m_InspectorIconPath = L"directional_right.png"; 

	m_directionalDesc.Init();
	m_pointDesc.Init();
	m_spotDesc.Init();

	m_LightView.resize(6);
	m_editorLightView.resize(6);
	ProjUpdate();
}

Light::~Light()
{
}

void Light::OnDestroy()
{
	// Manager 삭제
	LightManager::GetI()->DeleteLight(this->GetInstanceID());
}

void Light::SetDirLight(DirectionalLight light)
{
	m_directionalDesc = light;
	m_pGameObject->GetTransform()->SetLocalEulerRadians(light.Direction);
}

void Light::SetPointLight(PointLight light)
{
	m_pointDesc = light;
	m_pGameObject->GetTransform()->SetLocalPosition(light.Position);
}

void Light::SetSpotLight(SpotLight light)
{
	m_spotDesc = light;
	m_pGameObject->GetTransform()->SetLocalEulerRadians(light.Direction);
	m_pGameObject->GetTransform()->SetLocalPosition(light.Position);
}

void Light::Awake()
{
	// Manager 추가
	//LightManager::GetI()->SetLight(shared_from_this());
}

float Light::GetRange()
{
	switch (m_lightType)
	{
	case LightType::Point:
		return m_pointDesc.Range;
	case LightType::Spot:
		return m_spotDesc.Range;
	}
}

void Light::Update()
{
}

void Light::LateUpdate()
{
}

void Light::FixedUpdate()
{
}

void Light::LastUpdate()
{
	XMVECTOR pos = m_pGameObject->GetTransform()->GetPosition();
	XMVECTOR lookDir = m_pGameObject->GetTransform()->GetLook();
	XMVECTOR target = pos + lookDir;
	XMVECTOR upDir = m_pGameObject->GetTransform()->GetUp();
	XMMATRIX V;

	Vec3 r;

	XMVECTOR backwardDir;
	XMVECTOR leftDir;
	XMVECTOR rightDir;
	XMVECTOR downDir;

	XMVECTOR gamePos;
	XMVECTOR editorPos;
	float editorFarZ;

	XMFLOAT3 tempPos = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetPosition();

	switch (m_lightType)
	{
	case LightType::Directional:
		r = m_pGameObject->GetTransform()->GetLocalEulerRadians();

		// gameCamera도 나중에 GameCameraManager에서 메인카메라를 가져오게 하기, projUpdate에도 변경 부분 있음
		// gamePos

		editorPos = XMLoadFloat3(&tempPos);
		editorFarZ = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetFarZ();
		pos = XMVectorSet
		(
			{ editorFarZ * sinf(r.x) * cosf(r.y) },
			{ editorFarZ * sinf(r.x) * sinf(r.y) },
			{ editorFarZ * cosf(r.x) },
			{ 0 }
		);

		// light pos = camera position + ((camerafarZ * 2) * light.dir)
		// light pos는 camera position을 중심으로 CameraFarZ(반지름) 끝에 위치해야함

		m_editorLightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);
		m_LightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);

		XMStoreFloat3(&m_directionalDesc.Direction, r);
		break;
	case LightType::Point:
		// 전방향이므로 6개의 매트릭스 필요, proj는 하나만이여도 상관없(범위)

		backwardDir = m_pGameObject->GetTransform()->GetBackward();
		leftDir = m_pGameObject->GetTransform()->GetLeft();
		rightDir = m_pGameObject->GetTransform()->GetRight();
		downDir = m_pGameObject->GetTransform()->GetDown();

		m_editorLightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);
		m_editorLightView[1] = ::XMMatrixLookAtLH(pos, pos + backwardDir, upDir);
		m_editorLightView[2] = ::XMMatrixLookAtLH(pos, pos + leftDir, upDir);
		m_editorLightView[3] = ::XMMatrixLookAtLH(pos, pos + rightDir, upDir);
		m_editorLightView[4] = ::XMMatrixLookAtLH(pos, pos + upDir, backwardDir);
		m_editorLightView[5] = ::XMMatrixLookAtLH(pos, pos + downDir, lookDir);

		m_LightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);
		m_LightView[1] = ::XMMatrixLookAtLH(pos, pos + backwardDir, upDir);
		m_LightView[2] = ::XMMatrixLookAtLH(pos, pos + leftDir, upDir);
		m_LightView[3] = ::XMMatrixLookAtLH(pos, pos + rightDir, upDir);
		m_LightView[4] = ::XMMatrixLookAtLH(pos, pos + upDir, backwardDir);
		m_LightView[5] = ::XMMatrixLookAtLH(pos, pos + downDir, lookDir);

		XMStoreFloat3(&m_pointDesc.Position, pos);
		break;
	case LightType::Spot:
		r = m_pGameObject->GetTransform()->GetLocalEulerRadians();

		m_editorLightView[0] = ::XMMatrixLookAtLH(pos, target, upDir);
		m_LightView[0] = ::XMMatrixLookAtLH(pos, target, upDir);

		XMStoreFloat3(&m_spotDesc.Position, pos);
		XMStoreFloat3(&m_spotDesc.Direction, r);
		break;
	default:
		break;
	}

	ProjUpdate();
}

void Light::Render()
{
}

// private function
void Light::ProjUpdate()
{
	XMMATRIX P;
	float gameAspect = Application::GetI()->GetApp()->AspectRatio();
	float editorAspect = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetAspect();
	float editorFarZ = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetFarZ();
	switch (m_lightType)
	{
	case LightType::Directional:
		// 직교
		// X, Y 범위 한정, FarZ 이론상 무한이지만 리소스 효율을 위해 빛의 위치와 씬 위치 사이의 거리 + 씬의 크기로
		//m_gameLightProj = ::XMMatrixOrthographicLH(m_dirLightLen.x, m_dirLightLen.y, 0.001f, 100.f);
		m_editorLightProj = ::XMMatrixOrthographicLH(editorFarZ * 2, editorFarZ * 2, 0.001f, editorFarZ * 2);
		break;
	case LightType::Point:
		// 원근
		// float fov = XMConvertToRadians(90.0f);
		// XMMatrixOrthographicLH(D3DXToRadian(90), 1, near, range)
		// X, Y 의미 없으므로 임의의 값으로, FarZ는 범위 한정
		//m_lightProj = ::XMMatrixOrthographicLH(10.0f, 10.0f, 0.001f, m_pointDesc.Range);
		m_LightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(90), gameAspect, 0.001f, m_pointDesc.Range);
		m_editorLightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(90), editorAspect, 0.001f, m_pointDesc.Range);
		break;
	case LightType::Spot:
		// 원근
		// XMMatrixOrthographicLH(D3DXToRadian(angle) <- fov, aspectRatio <- camera 비율, near, range)
		// 1920x1080인 경우 : aspectRatio = 1920/1080 = 1.78
		// X, Y, FarZ 범위 한정

		m_LightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(m_spotDesc.Spot), gameAspect, 0.001f, m_spotDesc.Range);
		m_editorLightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(m_spotDesc.Spot), editorAspect, 0.001f, m_spotDesc.Range);
		break;
	default:
		break;
	}
}
string Light::GetStringLightType(LightType type)
{
	switch (type)
	{
	case LightType::Directional: return "Directional";
	case LightType::Point: return "Point";
	case LightType::Spot: return "Spot";
	default: return "Unknown";
	}
}

void Light::OnInspectorGUI()
{
	bool ProjectionChanged = false; // ProjectionMatrix를 조정하는 변수값이 변경 시

	// LightType 선택GUI
	if (ImGui::BeginCombo("LightType", GetStringLightType(m_lightType).c_str())) // The second parameter is the previewed value
	{
		for (int n = 0; n < (int)LightType::End; n++)
		{
			bool is_selected = ((int)m_lightType == n); // You can store your selection somewhere
			if (ImGui::Selectable(GetStringLightType((LightType)n).c_str(), is_selected))
			{
				m_lightType = (LightType)n;
				ProjectionChanged = true;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	// LightType의 값을 조정하는 GUI
	switch (m_lightType)
	{
	case LightType::Directional:
		ImGui::Text("Ambient");
		ImGui::DragFloat4("##Ambient", reinterpret_cast<float*>(&m_directionalDesc.Ambient), 0.1f);
		ImGui::Text("Diffuse");
		ImGui::DragFloat4("##Diffuse", reinterpret_cast<float*>(&m_directionalDesc.Diffuse), 0.1f);
		ImGui::Text("Specular");
		ImGui::DragFloat4("##Specular", reinterpret_cast<float*>(&m_directionalDesc.Specular), 0.1f);
		break;
	case LightType::Point:
		ImGui::Text("Ambient");
		ImGui::DragFloat4("##Ambient", reinterpret_cast<float*>(&m_pointDesc.Ambient), 0.1f);
		ImGui::Text("Diffuse");
		ImGui::DragFloat4("##Diffuse", reinterpret_cast<float*>(&m_pointDesc.Diffuse), 0.1f);
		ImGui::Text("Specular");
		ImGui::DragFloat4("##Specular", reinterpret_cast<float*>(&m_pointDesc.Specular), 0.1f);
		ImGui::Text("Range");
		if (ImGui::DragFloat("##Range", reinterpret_cast<float*>(&m_pointDesc.Range), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("Att");
		ImGui::DragFloat3("##Att", reinterpret_cast<float*>(&m_pointDesc.Att), 0.1f);
		break;
	case LightType::Spot:
		ImGui::Text("Ambient");
		ImGui::DragFloat4("##Ambient", reinterpret_cast<float*>(&m_spotDesc.Ambient), 0.1f);
		ImGui::Text("Diffuse");
		ImGui::DragFloat4("##Diffuse", reinterpret_cast<float*>(&m_spotDesc.Diffuse), 0.1f);
		ImGui::Text("Specular");
		ImGui::DragFloat4("##Specular", reinterpret_cast<float*>(&m_spotDesc.Specular), 0.1f);
		ImGui::Text("Range");
		if (ImGui::DragFloat("##Range", reinterpret_cast<float*>(&m_spotDesc.Range), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("Spot");
		if (ImGui::DragFloat("##Spot", reinterpret_cast<float*>(&m_spotDesc.Spot), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("Att");
		ImGui::DragFloat3("##Att", reinterpret_cast<float*>(&m_spotDesc.Att), 0.1f);
		break;
	default:
		break;
	}

	if (ProjectionChanged)
		ProjUpdate();
}

void Light::OnDrawGizmos()
{
}

GENERATE_COMPONENT_FUNC_TOJSON(Light)
{
	json j = {};
	j["type"] = "Light";
	j["lightType"] = m_lightType;

	j["directionalLightAmbient"] = { m_directionalDesc.Ambient.x, m_directionalDesc.Ambient.y, m_directionalDesc.Ambient.z, m_directionalDesc.Ambient.w };
	j["directionalLightDiffuse"] = { m_directionalDesc.Diffuse.x, m_directionalDesc.Diffuse.y, m_directionalDesc.Diffuse.z, m_directionalDesc.Diffuse.w };
	j["directionalLightSpecular"] = { m_directionalDesc.Specular.x, m_directionalDesc.Specular.y, m_directionalDesc.Specular.z, m_directionalDesc.Specular.w };

	j["pointLightAmbient"] = { m_pointDesc.Ambient.x, m_pointDesc.Ambient.y, m_pointDesc.Ambient.z, m_pointDesc.Ambient.w };
	j["pointLightDiffuse"] = { m_pointDesc.Diffuse.x, m_pointDesc.Diffuse.y, m_pointDesc.Diffuse.z, m_pointDesc.Diffuse.w };
	j["pointLightSpecular"] = { m_pointDesc.Specular.x, m_pointDesc.Specular.y, m_pointDesc.Specular.z, m_pointDesc.Specular.w };
	j["pointLightRange"] = m_pointDesc.Range;
	j["pointLightAtt"] = { m_pointDesc.Att.x,m_pointDesc.Att.y,m_pointDesc.Att.z };

	j["spotLightAmbient"] = { m_spotDesc.Ambient.x, m_spotDesc.Ambient.y, m_spotDesc.Ambient.z, m_spotDesc.Ambient.w };
	j["spotLightDiffuse"] = { m_spotDesc.Diffuse.x, m_spotDesc.Diffuse.y, m_spotDesc.Diffuse.z, m_spotDesc.Diffuse.w };
	j["spotLightSpecular"] = { m_spotDesc.Specular.x, m_spotDesc.Specular.y, m_spotDesc.Specular.z, m_spotDesc.Specular.w };
	j["spotLightRange"] = m_spotDesc.Range;
	j["spotLightSpot"] = m_spotDesc.Spot;
	j["spotLightAtt"] = { m_spotDesc.Att.x,m_spotDesc.Att.y,m_spotDesc.Att.z };

	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(Light)
{
	if (j.contains("lightType"))
	{
		m_lightType = j.at("lightType").get<LightType>();
	}

	// Directional
	{
		DE_SERIALIZE_FLOAT4(j, m_directionalDesc.Ambient, "directionalLightAmbient");
		DE_SERIALIZE_FLOAT4(j, m_directionalDesc.Diffuse, "directionalLightDiffuse");
		DE_SERIALIZE_FLOAT4(j, m_directionalDesc.Specular, "directionalLightSpecular");

	}

	// Point
	{
		DE_SERIALIZE_FLOAT4(j, m_pointDesc.Ambient, "pointLightAmbient");
		DE_SERIALIZE_FLOAT4(j, m_pointDesc.Diffuse, "pointLightDiffuse");
		DE_SERIALIZE_FLOAT4(j, m_pointDesc.Specular, "pointLightSpecular");
		DE_SERIALIZE_FLOAT(j, m_pointDesc.Range, "pointLightRange");
		DE_SERIALIZE_FLOAT3(j, m_pointDesc.Att, "pointLightAtt");
	}

	// Spot
	{
		DE_SERIALIZE_FLOAT4(j, m_spotDesc.Ambient, "spotLightAmbient");
		DE_SERIALIZE_FLOAT4(j, m_spotDesc.Diffuse, "spotLightDiffuse");
		DE_SERIALIZE_FLOAT4(j, m_spotDesc.Specular, "spotLightSpecular");
		DE_SERIALIZE_FLOAT(j, m_spotDesc.Range, "spotLightRange");
		DE_SERIALIZE_FLOAT(j, m_spotDesc.Spot, "spotLightSpot");
		DE_SERIALIZE_FLOAT3(j, m_spotDesc.Att, "spotLightAtt");

	}

	ProjUpdate();
}