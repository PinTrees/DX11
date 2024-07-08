#include "pch.h"
#include "RigidBody.h"
#include "Debug.h"

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

void RigidBody::ApplyImpulse(const Vec3& impulse)
{
	m_Velocity += impulse / m_Mass;
}

void RigidBody::Integrate(float deltaTime)
{
	// 속도 업데이트: v = u + at
	m_Velocity += m_Acceleration * deltaTime; 

	Debug::Log("Acceleration: " + ToString(m_Acceleration)); 
	Debug::Log("m_Velocity: " + ToString(m_Velocity)); 
	Debug::Log("deltaTile: " + to_string(deltaTime)); 

	// 위치 업데이트: s = s + vt
	auto position = GetGameObject()->GetTransform()->GetPosition();
	position += m_Velocity * deltaTime;
	GetGameObject()->GetTransform()->SetPosition(position); 

	// 가속도 초기화 (한 프레임 동안만 적용)
	m_Acceleration = Vec3(0.0f, 0.0f, 0.0f); 

	int a = 0;
} 

void RigidBody::OnInspectorGUI()
{
	ImGui::Text("RigidBody");

	ImGui::DragFloat("Mass", &m_Mass);
	ImGui::DragFloat("Elasticity", &m_Elasticity);
}

GENERATE_COMPONENT_FUNC_TOJSON(RigidBody)
{
	json j = {};
	j["type"] = "RigidBody";
	j["mass"] = m_Mass;
	j["elasticity"] = m_Elasticity;

	// Dont save velocity
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(RigidBody)
{
	m_Mass = j["mass"];
	m_Elasticity = j["elasticity"];
}
