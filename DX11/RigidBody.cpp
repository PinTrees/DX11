#include "pch.h"
#include "RigidBody.h"

RigidBody::RigidBody()
	: m_Velocity(0.0f, 0.0f, 0.0f),
	m_Mass(1.0f),
	m_Elasticity(0.5f)
{
	m_InspectorTitleName = "RigidBody";
}

RigidBody::~RigidBody()
{
}

void RigidBody::ApplyForce(const Vec3& force)
{
	Vec3 acceleration = force / m_Mass; 
	m_Acceleration += acceleration; 
}

void RigidBody::Integrate(float deltaTime)
{
	// �ӵ� ������Ʈ: v = u + at
	m_Velocity += m_Acceleration * deltaTime;

	// ��ġ ������Ʈ: s = s + vt
	auto position = GetGameObject()->GetTransform()->GetPosition();
	position += m_Velocity * deltaTime;
	GetGameObject()->GetTransform()->SetPosition(position); 

	// ���ӵ� �ʱ�ȭ (�� ������ ���ȸ� ����)
	m_Acceleration = Vec3(0.0f, 0.0f, 0.0f); 
} 

void RigidBody::OnInspectorGUI()
{
	ImGui::Text("RigidBody");

	ImGui::InputFloat("Mass", &m_Mass);
	ImGui::InputFloat("Elasticity", &m_Elasticity);
}


GENERATE_COMPONENT_FUNC_TOJSON(RigidBody)
{
	json j = {};
	j["type"] = "RigidBody";
	j["velocity"] = { m_Velocity.x, m_Velocity.y, m_Velocity.z };
	j["mass"] = m_Mass;
	j["elasticity"] = m_Elasticity;
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(RigidBody)
{
	m_Velocity = Vec3(j["velocity"][0], j["velocity"][1], j["velocity"][2]);
	m_Mass = j["mass"];
	m_Elasticity = j["elasticity"];
}
