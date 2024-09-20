#include "pch.h"
#include "Light.h"
#include "LightManager.h"

Light::Light()
{
	m_InspectorTitleName = "Light";
	m_directionalDesc.Init();
	m_pointDesc.Init();
	m_spotDesc.Init();
	ProjUpdate();
	// Manager 추가
	LightManager::GetI()->SetLight(shared_from_this());
}

Light::~Light()
{
}

void Light::OnDestroy()
{
	// Manager 삭제
	LightManager::GetI()->DeleteLight(this->GetInstanceID());
}

void Light::Update()
{
}

void Light::LateUpdate()
{
	XMVECTOR pos = m_pGameObject->GetTransform()->GetPosition();
	XMVECTOR dir = m_pGameObject->GetTransform()->GetLook();
	XMVECTOR target = pos + dir;
	XMVECTOR up = m_pGameObject->GetTransform()->GetUp();
	XMMATRIX V;

	switch (m_lightType)
	{
	case LightType::Directional:
		XMStoreFloat3(&m_directionalDesc.Direction, dir);
		break;
	case LightType::Point:
		XMStoreFloat3(&m_pointDesc.Position, pos);
		break;
	case LightType::Spot:
		XMStoreFloat3(&m_spotDesc.Position, pos);
		XMStoreFloat3(&m_spotDesc.Direction, dir);
		break;
	default:
		break;
	}

	V = ::XMMatrixLookAtLH(pos, target, up);
	::XMStoreFloat4x4(&m_lightView, V);
}

void Light::FixedUpdate()
{
}

void Light::Render()
{
}

// private function
void Light::ProjUpdate()
{
	XMMATRIX P;
	switch (m_lightType)
	{
	case LightType::Directional:
		// X, Y 범위 한정, FarZ 이론상 무한이지만 리소스 효율을 위해 빛의 위치와 씬 위치 사이의 거리 + 씬의 크기로
		P = ::XMMatrixOrthographicLH(m_dirLightLen.x, m_dirLightLen.y, 0.001f, 100.f);
		break;
	case LightType::Point:
		// X, Y 의미 없으므로 임의의 값으로, FarZ는 범위 한정
		P = ::XMMatrixOrthographicLH(10.0f, 10.0f, 0.001f, m_pointDesc.Range);
		break;
	case LightType::Spot:
		// X, Y, FarZ 범위 한정
		P = ::XMMatrixOrthographicLH(m_spotLightLen.x, m_spotLightLen.y, 0.001f, m_spotDesc.Range);
		break;
	default: 
		break;
	}

	::XMStoreFloat4x4(&m_lightProj, P);
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
	// LightType 선택GUI
	if (ImGui::BeginCombo("LightType", GetStringLightType(m_lightType).c_str())) // The second parameter is the previewed value
	{
		for (int n = 0; n < (int)LightType::End; n++)
		{
			bool is_selected = ((int)m_lightType == n); // You can store your selection somewhere
			if (ImGui::Selectable(GetStringLightType((LightType)n).c_str(), is_selected))
			{
				m_lightType = (LightType)n;
				ProjUpdate();
			}
				
			if (is_selected)
				ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	bool ProjectionChanged = false; // ProjectionMatrix를 조정하는 변수값이 변경 시

	// LightType의 값을 조정하는 GUI
	switch (m_lightType)
	{
	case LightType::Directional:
		ImGui::Text("LightLengthX");
		if (ImGui::DragFloat4("##LightLengthX", reinterpret_cast<float*>(&m_dirLightLen.x), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("LightLengthY");
		if (ImGui::DragFloat4("##LightLengthY", reinterpret_cast<float*>(&m_dirLightLen.y), 0.1f))
		{
			ProjectionChanged = true;
		}

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

		break;
	case LightType::Spot:
		ImGui::Text("LightLengthX");
		if (ImGui::DragFloat4("##LightLengthX", reinterpret_cast<float*>(&m_spotLightLen.x), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("LightLengthY");
		if (ImGui::DragFloat4("##LightLengthY", reinterpret_cast<float*>(&m_spotLightLen.y), 0.1f))
		{
			ProjectionChanged = true;
		}

		ImGui::Text("Ambient");
		ImGui::DragFloat4("##Ambient", reinterpret_cast<float*>(&m_spotDesc.Ambient), 0.1f);
		ImGui::Text("Diffuse");
		ImGui::DragFloat4("##Diffuse", reinterpret_cast<float*>(&m_spotDesc.Diffuse), 0.1f);
		ImGui::Text("Specular");
		ImGui::DragFloat4("##Specular", reinterpret_cast<float*>(&m_spotDesc.Specular), 0.1f);
		ImGui::Text("Range");
		if(ImGui::DragFloat("##Range", reinterpret_cast<float*>(&m_spotDesc.Range), 0.1f))
		{
			ProjectionChanged = true;
		}
		ImGui::Text("Spot");
		ImGui::DragFloat("##Spot", reinterpret_cast<float*>(&m_spotDesc.Spot), 0.1f);
		break;
	default:
		break;
	}

	if(ProjectionChanged)
		ProjUpdate();
}

GENERATE_COMPONENT_FUNC_TOJSON(Light)
{
	json j = {};
	j["type"] = "Light";
	j["lightType"] = m_lightType;

	j["directionalLightAmbient"] = { m_directionalDesc.Ambient.x, m_directionalDesc.Ambient.y, m_directionalDesc.Ambient.z, m_directionalDesc.Ambient.w };
	j["directionalLightDiffuse"] = { m_directionalDesc.Diffuse.x, m_directionalDesc.Diffuse.y, m_directionalDesc.Diffuse.z, m_directionalDesc.Diffuse.w };
	j["directionalLightSpecular"] = { m_directionalDesc.Specular.x, m_directionalDesc.Specular.y, m_directionalDesc.Specular.z, m_directionalDesc.Specular.w };
	j["directionalLightDirection"] = { m_directionalDesc.Direction.x, m_directionalDesc.Direction.y, m_directionalDesc.Direction.z };
	j["directionalLightLength"] = { m_dirLightLen.x,m_dirLightLen.y };

	j["pointLightAmbient"] = { m_pointDesc.Ambient.x, m_pointDesc.Ambient.y, m_pointDesc.Ambient.z, m_pointDesc.Ambient.w };
	j["pointLightDiffuse"] = { m_pointDesc.Diffuse.x, m_pointDesc.Diffuse.y, m_pointDesc.Diffuse.z, m_pointDesc.Diffuse.w };
	j["pointLightSpecular"] = { m_pointDesc.Specular.x, m_pointDesc.Specular.y, m_pointDesc.Specular.z, m_pointDesc.Specular.w };
	j["pointLightPosition"] = { m_pointDesc.Position.x, m_pointDesc.Position.y, m_pointDesc.Position.z };
	j["pointLightRange"] = m_pointDesc.Range;

	j["spotLightAmbient"] = { m_spotDesc.Ambient.x, m_spotDesc.Ambient.y, m_spotDesc.Ambient.z, m_spotDesc.Ambient.w };
	j["spotLightDiffuse"] = { m_spotDesc.Diffuse.x, m_spotDesc.Diffuse.y, m_spotDesc.Diffuse.z, m_spotDesc.Diffuse.w };
	j["spotLightSpecular"] = { m_spotDesc.Specular.x, m_spotDesc.Specular.y, m_spotDesc.Specular.z, m_spotDesc.Specular.w };
	j["spotLightPosition"] = { m_spotDesc.Position.x, m_spotDesc.Position.y, m_spotDesc.Position.z };
	j["spotLightRange"] = m_spotDesc.Range;
	j["spotLightDirection"] = { m_spotDesc.Direction.x, m_spotDesc.Direction.y, m_spotDesc.Direction.z };
	j["spotLightSpot"] = m_spotDesc.Spot;
	j["spotLightLength"] = { m_spotLightLen.x,m_spotLightLen.y };

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
		if (j.contains("directionalLightAmbient"))
		{
			auto ambient = j.at("directionalLightAmbient").get<std::vector<float>>();
			m_directionalDesc.Ambient = XMFLOAT4{ ambient[0], ambient[1], ambient[2], ambient[3] };
		}
		if (j.contains("directionalLightDiffuse"))
		{
			auto diffuse = j.at("directionalLightDiffuse").get<std::vector<float>>();
			m_directionalDesc.Diffuse = XMFLOAT4{ diffuse[0], diffuse[1], diffuse[2], diffuse[3] };
		}
		if (j.contains("directionalLightSpecular"))
		{
			auto specular = j.at("directionalLightSpecular").get<std::vector<float>>();
			m_directionalDesc.Specular = XMFLOAT4{ specular[0], specular[1], specular[2], specular[3] };
		}
		if (j.contains("directionalLightDirection"))
		{
			auto direction = j.at("directionalLightDirection").get<std::vector<float>>();
			m_directionalDesc.Direction = XMFLOAT3{ direction[0], direction[1], direction[2] };
		}
		if (j.contains("directionalLightLength"))
		{
			auto length = j.at("directionalLightLength").get<std::vector<float>>();
			m_dirLightLen = XMFLOAT2{ length[0], length[1] };
		}
	}
	
	// Point
	{
		if (j.contains("pointLightAmbient"))
		{
			auto ambient = j.at("pointLightAmbient").get<std::vector<float>>();
			m_pointDesc.Ambient = XMFLOAT4{ ambient[0], ambient[1], ambient[2], ambient[3] };
		}
		if (j.contains("pointLightDiffuse"))
		{
			auto diffuse = j.at("pointLightDiffuse").get<std::vector<float>>();
			m_pointDesc.Diffuse = XMFLOAT4{ diffuse[0], diffuse[1], diffuse[2], diffuse[3] };
		}
		if (j.contains("pointLightSpecular"))
		{
			auto specular = j.at("pointLightSpecular").get<std::vector<float>>();
			m_pointDesc.Specular = XMFLOAT4{ specular[0], specular[1], specular[2], specular[3] };
		}
		if (j.contains("pointLightPosition"))
		{
			auto position = j.at("pointLightPosition").get<std::vector<float>>();
			m_pointDesc.Position = XMFLOAT3{ position[0], position[1], position[2] };
		}
		if (j.contains("pointLightRange"))
		{
			auto range = j.at("pointLightPosition").get<float>();
			m_pointDesc.Range = range;
		}
	}

	// Spot
	{
		if (j.contains("spotLightAmbient"))
		{
			auto ambient = j.at("spotLightAmbient").get<std::vector<float>>();
			m_spotDesc.Ambient = XMFLOAT4{ ambient[0], ambient[1], ambient[2], ambient[3] };
		}
		if (j.contains("spotLightDiffuse"))
		{
			auto diffuse = j.at("spotLightDiffuse").get<std::vector<float>>();
			m_spotDesc.Diffuse = XMFLOAT4{ diffuse[0], diffuse[1], diffuse[2], diffuse[3] };
		}
		if (j.contains("spotLightSpecular"))
		{
			auto specular = j.at("spotLightSpecular").get<std::vector<float>>();
			m_spotDesc.Specular = XMFLOAT4{ specular[0], specular[1], specular[2], specular[3] };
		}
		if (j.contains("spotLightPosition"))
		{
			auto position = j.at("spotLightPosition").get<std::vector<float>>();
			m_spotDesc.Position = XMFLOAT3{ position[0], position[1], position[2] };
		}
		if (j.contains("spotLightRange"))
		{
			auto range = j.at("spotLightRange").get<float>();
			m_spotDesc.Range = range;
		}
		if (j.contains("spotLightDirection"))
		{
			auto direction = j.at("spotLightDirection").get<std::vector<float>>();
			m_spotDesc.Direction = XMFLOAT3{ direction[0], direction[1], direction[2] };
		}
		if (j.contains("spotLightSpot"))
		{
			auto spot = j.at("spotLightSpot").get<float>();
			m_spotDesc.Spot = spot;
		}
		if (j.contains("spotLightLength"))
		{
			auto length = j.at("spotLightLength").get<std::vector<float>>();
			m_spotLightLen = XMFLOAT2{ length[0], length[1] };
		}
	}

	ProjUpdate();
}