#include "pch.h"
#include "Light.h"

Light::Light()
{
	m_InspectorTitleName = "Light";
}

Light::~Light()
{
}

void Light::Update()
{
}

void Light::LateUpdate()
{
	XMVECTOR pos = m_pGameObject->GetTransform()->GetPosition();
	XMVECTOR dir = XMLoadFloat3(&m_directionalDesc.Direction);
	XMVECTOR target = pos + dir;
	//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX V = ::XMMatrixLookAtLH(pos,target,up);
	::XMStoreFloat4x4(&m_lightView, V);

	XMMATRIX P = ::XMMatrixOrthographicLH(m_lightLengthX, m_lightLengthY, 0.001f, m_lightFarZ);
	::XMStoreFloat4x4(&m_lightProj, P);

}

void Light::FixedUpdate()
{
}

void Light::Render()
{
}

void Light::OnInspectorGUI()
{
}

GENERATE_COMPONENT_FUNC_TOJSON(Light)
{
	json j = {};
	j["type"] = "Light";
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(Light)
{

}