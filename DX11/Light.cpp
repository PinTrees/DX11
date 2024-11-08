#include "pch.h"
#include "App.h"
#include "Light.h"
#include "LightManager.h"
#include "Transform.h"
#include "SceneViewManager.h"
#include "SceneEditorWindow.h"
#include "EditorCamera.h"
#include "Camera.h"
#include "DisplayManager.h"

Light::Light()
{
	m_InspectorTitleName = "Light";
	m_InspectorIconPath = L"directional_right.png"; 

	m_DirectionalDesc.Init();
	m_PointDesc.Init();
	m_SpotDesc.Init();

	m_LightView.resize(6);
	m_EditorLightView.resize(6);
	ProjUpdate();
}

Light::~Light()
{
}

void Light::OnDestroy()
{
	// Manager ����
	LightManager::GetI()->DeleteLight(this->GetInstanceID());
}

void Light::SetDirLight(DirectionalLight light)
{
	m_DirectionalDesc = light;
	m_pGameObject->GetTransform()->SetLocalEulerRadians(light.Direction);
}

void Light::SetPointLight(PointLight light)
{
	m_PointDesc = light;
	m_pGameObject->GetTransform()->SetLocalPosition(light.Position);
}

void Light::SetSpotLight(SpotLight light)
{
	m_SpotDesc = light;
	m_pGameObject->GetTransform()->SetLocalEulerRadians(light.Direction);
	m_pGameObject->GetTransform()->SetLocalPosition(light.Position);
}

void Light::Awake()
{
	// Manager �߰�
	//LightManager::GetI()->SetLight(shared_from_this());
}

float Light::GetRange()
{
	switch (m_LightType)
	{
	case LightType::Point:
		return m_PointDesc.Range;
	case LightType::Spot:
		return m_SpotDesc.Range;
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
}

void Light::ViewUpdate()
{
	XMVECTOR pos = m_pGameObject->GetTransform()->GetPosition();
	XMVECTOR lookDir = m_pGameObject->GetTransform()->GetLook();
	XMVECTOR target = pos + lookDir;
	XMVECTOR upDir = m_pGameObject->GetTransform()->GetUp();

	Vec3 r;

	XMVECTOR backwardDir;
	XMVECTOR leftDir;
	XMVECTOR rightDir;
	XMVECTOR downDir;

	XMVECTOR tempPos;
	float cameraFarZ;

	XMFLOAT3 gameCameraPos = DisplayManager::GetI()->GetActiveCamera()->GetPosition();

	switch (m_LightType)
	{
	case LightType::Directional:
		// Pos = GameCameraPosition + LightRotation�������� Camera ���� ��輱�� ��ġ
		// light pos = camera position + ((camerafarZ * 2) * light.dir)
		// light pos�� camera position�� �߽����� CameraFarZ(������) ���� ��ġ�ؾ���

		r = m_pGameObject->GetTransform()->GetLocalEulerRadians();

		tempPos = XMLoadFloat3(&gameCameraPos);
		cameraFarZ = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetFarZ();
		pos = XMVectorSet
		(
			{ cameraFarZ * sinf(r.x) * cosf(r.y) },
			{ cameraFarZ * sinf(r.x) * sinf(r.y) },
			{ cameraFarZ * cosf(r.x) },
			{ 0 }
		);

		pos += tempPos;

		m_LightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);

		XMStoreFloat3(&m_DirectionalDesc.Direction, r);
		break;
	case LightType::Point:
		// �������̹Ƿ� 6���� ��Ʈ���� �ʿ�, proj�� �ϳ����̿��� �����(����)

		backwardDir = m_pGameObject->GetTransform()->GetBackward();
		leftDir = m_pGameObject->GetTransform()->GetLeft();
		rightDir = m_pGameObject->GetTransform()->GetRight();
		downDir = m_pGameObject->GetTransform()->GetDown();

		m_LightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);
		m_LightView[1] = ::XMMatrixLookAtLH(pos, pos + backwardDir, upDir);
		m_LightView[2] = ::XMMatrixLookAtLH(pos, pos + leftDir, upDir);
		m_LightView[3] = ::XMMatrixLookAtLH(pos, pos + rightDir, upDir);
		m_LightView[4] = ::XMMatrixLookAtLH(pos, pos + upDir, backwardDir);
		m_LightView[5] = ::XMMatrixLookAtLH(pos, pos + downDir, lookDir);

		XMStoreFloat3(&m_PointDesc.Position, pos);
		break;
	case LightType::Spot:
		r = m_pGameObject->GetTransform()->GetLocalEulerRadians();

		m_LightView[0] = ::XMMatrixLookAtLH(pos, target, upDir);

		XMStoreFloat3(&m_SpotDesc.Position, pos);
		XMStoreFloat3(&m_SpotDesc.Direction, r);
		break;
	default:
		break;
	}

	ProjUpdate();
}

void Light::EditorViewUpdate()
{
	XMVECTOR pos = m_pGameObject->GetTransform()->GetPosition();
	XMVECTOR lookDir = m_pGameObject->GetTransform()->GetLook();
	XMVECTOR target = pos + lookDir;
	XMVECTOR upDir = m_pGameObject->GetTransform()->GetUp();

	Vec3 r;

	XMVECTOR backwardDir;
	XMVECTOR leftDir;
	XMVECTOR rightDir;
	XMVECTOR downDir;

	XMVECTOR tempPos;
	float cameraFarZ;

	XMFLOAT3 editorCameraPos = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetPosition();


	switch (m_LightType)
	{
	case LightType::Directional:
		// Pos = GameCameraPosition + LightRotation�������� Camera ���� ��輱�� ��ġ
		// light pos = camera position + ((camerafarZ * 2) * light.dir)
		// light pos�� camera position�� �߽����� CameraFarZ(������) ���� ��ġ�ؾ���

		r = m_pGameObject->GetTransform()->GetLocalEulerRadians();

		tempPos = XMLoadFloat3(&editorCameraPos);
		cameraFarZ = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetFarZ();
		pos = XMVectorSet
		(
			{ cameraFarZ * sinf(r.x) * cosf(r.y) },
			{ cameraFarZ * sinf(r.x) * sinf(r.y) },
			{ cameraFarZ * cosf(r.x) },
			{ 0 }
		);

		pos += tempPos;

		m_EditorLightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);

		XMStoreFloat3(&m_DirectionalDesc.Direction, r);
		break;
	case LightType::Point:
		// �������̹Ƿ� 6���� ��Ʈ���� �ʿ�, proj�� �ϳ����̿��� �����(����)

		backwardDir = m_pGameObject->GetTransform()->GetBackward();
		leftDir = m_pGameObject->GetTransform()->GetLeft();
		rightDir = m_pGameObject->GetTransform()->GetRight();
		downDir = m_pGameObject->GetTransform()->GetDown();

		m_EditorLightView[0] = ::XMMatrixLookAtLH(pos, pos + lookDir, upDir);
		m_EditorLightView[1] = ::XMMatrixLookAtLH(pos, pos + backwardDir, upDir);
		m_EditorLightView[2] = ::XMMatrixLookAtLH(pos, pos + leftDir, upDir);
		m_EditorLightView[3] = ::XMMatrixLookAtLH(pos, pos + rightDir, upDir);
		m_EditorLightView[4] = ::XMMatrixLookAtLH(pos, pos + upDir, backwardDir);
		m_EditorLightView[5] = ::XMMatrixLookAtLH(pos, pos + downDir, lookDir);

		XMStoreFloat3(&m_PointDesc.Position, pos);
		break;
	case LightType::Spot:
		r = m_pGameObject->GetTransform()->GetLocalEulerRadians();

		m_EditorLightView[0] = ::XMMatrixLookAtLH(pos, target, upDir);

		XMStoreFloat3(&m_SpotDesc.Position, pos);
		XMStoreFloat3(&m_SpotDesc.Direction, r);
		break;
	default:
		break;
	}

	EditorProjUpdate();
}

void Light::Render()
{
	
}

// private function
void Light::ProjUpdate()
{
	if (!DisplayManager::GetI()->GetActiveCamera())
		return;

	float gameAspect = Application::GetI()->GetApp()->AspectRatio();
	float gameFarZ = DisplayManager::GetI()->GetActiveCamera()->GetFarZ();
	switch (m_LightType)
	{
	case LightType::Directional:
		// ����
		// X, Y ���� ����, FarZ �̷л� ���������� ���ҽ� ȿ���� ���� ���� ��ġ�� �� ��ġ ������ �Ÿ� + ���� ũ���
		m_LightProj = ::XMMatrixOrthographicLH(gameFarZ * 2, gameFarZ * 2, 0.001f, gameFarZ * 2);
		break;
	case LightType::Point:
		// ����
		// float fov = XMConvertToRadians(90.0f);
		// XMMatrixOrthographicLH(D3DXToRadian(90), 1, near, range)
		// X, Y �ǹ� �����Ƿ� ������ ������, FarZ�� ���� ����
		//m_lightProj = ::XMMatrixOrthographicLH(10.0f, 10.0f, 0.001f, m_pointDesc.Range);
		m_LightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(90), gameAspect, 0.001f, m_PointDesc.Range);
		break;
	case LightType::Spot:
		// ����
		// XMMatrixOrthographicLH(D3DXToRadian(angle) <- fov, aspectRatio <- camera ����, near, range)
		// 1920x1080�� ��� : aspectRatio = 1920/1080 = 1.78
		// X, Y, FarZ ���� ����
		m_LightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(m_SpotDesc.Spot), gameAspect, 0.001f, m_SpotDesc.Range);
		break;
	default:
		break;
	}
}

void Light::EditorProjUpdate()
{
	float editorAspect = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetAspect();
	float editorFarZ = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera()->GetFarZ();
	switch (m_LightType)
	{
	case LightType::Directional:
		m_EditorLightProj = ::XMMatrixOrthographicLH(editorFarZ * 2, editorFarZ * 2, 0.001f, editorFarZ * 2);
		break;
	case LightType::Point:
		m_EditorLightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(90), editorAspect, 0.001f, m_PointDesc.Range);
		break;
	case LightType::Spot:
		m_EditorLightProj = ::XMMatrixPerspectiveFovLH(XMConvertToRadians(m_SpotDesc.Spot), editorAspect, 0.001f, m_SpotDesc.Range);
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
	bool ProjectionChanged = false; // ProjectionMatrix�� �����ϴ� �������� ���� ��

	// LightType ����GUI
	if (ImGui::BeginCombo("LightType", GetStringLightType(m_LightType).c_str())) // The second parameter is the previewed value
	{
		for (int n = 0; n < (int)LightType::End; n++)
		{
			bool is_selected = ((int)m_LightType == n); // You can store your selection somewhere
			if (ImGui::Selectable(GetStringLightType((LightType)n).c_str(), is_selected))
			{
				m_LightType = (LightType)n;
				ProjectionChanged = true;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	// LightType�� ���� �����ϴ� GUI
	switch (m_LightType)
	{
	case LightType::Directional:
		ImGui::Text("Ambient");
		ImGui::DragFloat4("##Ambient", reinterpret_cast<float*>(&m_DirectionalDesc.Ambient), 0.1f);
		ImGui::Text("Diffuse");
		ImGui::DragFloat4("##Diffuse", reinterpret_cast<float*>(&m_DirectionalDesc.Diffuse), 0.1f);
		ImGui::Text("Specular");
		ImGui::DragFloat4("##Specular", reinterpret_cast<float*>(&m_DirectionalDesc.Specular), 0.1f);
		break;
	case LightType::Point:
		ImGui::Text("Ambient");
		ImGui::DragFloat4("##Ambient", reinterpret_cast<float*>(&m_PointDesc.Ambient), 0.1f);
		ImGui::Text("Diffuse");
		ImGui::DragFloat4("##Diffuse", reinterpret_cast<float*>(&m_PointDesc.Diffuse), 0.1f);
		ImGui::Text("Specular");
		ImGui::DragFloat4("##Specular", reinterpret_cast<float*>(&m_PointDesc.Specular), 0.1f);
		ImGui::Text("Range");
		if (ImGui::DragFloat("##Range", reinterpret_cast<float*>(&m_PointDesc.Range), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("Att");
		ImGui::DragFloat3("##Att", reinterpret_cast<float*>(&m_PointDesc.Att), 0.1f);
		break;
	case LightType::Spot:
		ImGui::Text("Ambient");
		ImGui::DragFloat4("##Ambient", reinterpret_cast<float*>(&m_SpotDesc.Ambient), 0.1f);
		ImGui::Text("Diffuse");
		ImGui::DragFloat4("##Diffuse", reinterpret_cast<float*>(&m_SpotDesc.Diffuse), 0.1f);
		ImGui::Text("Specular");
		ImGui::DragFloat4("##Specular", reinterpret_cast<float*>(&m_SpotDesc.Specular), 0.1f);
		ImGui::Text("Range");
		if (ImGui::DragFloat("##Range", reinterpret_cast<float*>(&m_SpotDesc.Range), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("Spot");
		if (ImGui::DragFloat("##Spot", reinterpret_cast<float*>(&m_SpotDesc.Spot), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("Att");
		ImGui::DragFloat3("##Att", reinterpret_cast<float*>(&m_SpotDesc.Att), 0.1f);
		break;
	default:
		break;
	}

	if (ProjectionChanged)
	{
		ProjUpdate();
		EditorProjUpdate();
	}
}

void Light::OnDrawGizmos()
{
}



GENERATE_COMPONENT_FUNC_TOJSON(Light)
{
	json j = {};
	j["type"] = "Light";
	j["lightType"] = m_LightType;

	j["directionalLightAmbient"] = { m_DirectionalDesc.Ambient.x, m_DirectionalDesc.Ambient.y, m_DirectionalDesc.Ambient.z, m_DirectionalDesc.Ambient.w };
	j["directionalLightDiffuse"] = { m_DirectionalDesc.Diffuse.x, m_DirectionalDesc.Diffuse.y, m_DirectionalDesc.Diffuse.z, m_DirectionalDesc.Diffuse.w };
	j["directionalLightSpecular"] = { m_DirectionalDesc.Specular.x, m_DirectionalDesc.Specular.y, m_DirectionalDesc.Specular.z, m_DirectionalDesc.Specular.w };

	j["pointLightAmbient"] = { m_PointDesc.Ambient.x, m_PointDesc.Ambient.y, m_PointDesc.Ambient.z, m_PointDesc.Ambient.w };
	j["pointLightDiffuse"] = { m_PointDesc.Diffuse.x, m_PointDesc.Diffuse.y, m_PointDesc.Diffuse.z, m_PointDesc.Diffuse.w };
	j["pointLightSpecular"] = { m_PointDesc.Specular.x, m_PointDesc.Specular.y, m_PointDesc.Specular.z, m_PointDesc.Specular.w };
	j["pointLightRange"] = m_PointDesc.Range;
	j["pointLightAtt"] = { m_PointDesc.Att.x,m_PointDesc.Att.y,m_PointDesc.Att.z };

	j["spotLightAmbient"] = { m_SpotDesc.Ambient.x, m_SpotDesc.Ambient.y, m_SpotDesc.Ambient.z, m_SpotDesc.Ambient.w };
	j["spotLightDiffuse"] = { m_SpotDesc.Diffuse.x, m_SpotDesc.Diffuse.y, m_SpotDesc.Diffuse.z, m_SpotDesc.Diffuse.w };
	j["spotLightSpecular"] = { m_SpotDesc.Specular.x, m_SpotDesc.Specular.y, m_SpotDesc.Specular.z, m_SpotDesc.Specular.w };
	j["spotLightRange"] = m_SpotDesc.Range;
	j["spotLightSpot"] = m_SpotDesc.Spot;
	j["spotLightAtt"] = { m_SpotDesc.Att.x,m_SpotDesc.Att.y,m_SpotDesc.Att.z };

	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(Light)
{
	if (j.contains("lightType"))
	{
		m_LightType = j.at("lightType").get<LightType>();
	}

	// Directional
	{
		DE_SERIALIZE_FLOAT4(j, m_DirectionalDesc.Ambient, "directionalLightAmbient");
		DE_SERIALIZE_FLOAT4(j, m_DirectionalDesc.Diffuse, "directionalLightDiffuse");
		DE_SERIALIZE_FLOAT4(j, m_DirectionalDesc.Specular, "directionalLightSpecular");

	}

	// Point
	{
		DE_SERIALIZE_FLOAT4(j, m_PointDesc.Ambient, "pointLightAmbient");
		DE_SERIALIZE_FLOAT4(j, m_PointDesc.Diffuse, "pointLightDiffuse");
		DE_SERIALIZE_FLOAT4(j, m_PointDesc.Specular, "pointLightSpecular");
		DE_SERIALIZE_FLOAT(j, m_PointDesc.Range, "pointLightRange");
		DE_SERIALIZE_FLOAT3(j, m_PointDesc.Att, "pointLightAtt");
	}

	// Spot
	{
		DE_SERIALIZE_FLOAT4(j, m_SpotDesc.Ambient, "spotLightAmbient");
		DE_SERIALIZE_FLOAT4(j, m_SpotDesc.Diffuse, "spotLightDiffuse");
		DE_SERIALIZE_FLOAT4(j, m_SpotDesc.Specular, "spotLightSpecular");
		DE_SERIALIZE_FLOAT(j, m_SpotDesc.Range, "spotLightRange");
		DE_SERIALIZE_FLOAT(j, m_SpotDesc.Spot, "spotLightSpot");
		DE_SERIALIZE_FLOAT3(j, m_SpotDesc.Att, "spotLightAtt");

	}

	ProjUpdate();
	EditorProjUpdate();
}