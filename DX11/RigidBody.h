#pragma once
#include "Component.h"

class Transform;

class RigidBody
	: public Component
{
private:
	Vec3 m_Velocity;
	Vec3 m_Acceleration;	// ���ӵ�
	float m_Mass;			// ����
	float m_Elasticity;		// �ݹ� ���
	bool m_IsKinematic;     // Ű��ƽ ���� (true�� ���� ��� ����)

public:
	RigidBody();
	~RigidBody();

public:
	void SetKinematic(bool isKinematic) { m_IsKinematic = isKinematic; }
	bool IsKinematic() const { return m_IsKinematic; }

	void SetVelocity(const Vec3& velocity) { m_Velocity = velocity; }
	Vec3 GetVelocity() const { return m_Velocity; }

	void SetMass(float mass) { m_Mass = mass; }
	float GetMass() const { return m_Mass; }

	void SetElasticity(float elasticity) { m_Elasticity = elasticity; }
	float GetInverseMass() const { return (m_Mass == 0) ? 0 : 1.0f / m_Mass; }
	float GetRestitution() const { return m_Elasticity; }

	void ApplyForce(const Vec3& force);
	void ApplyImpulse(const Vec3& impulse);
	

	void Integrate(float deltaTime);

public:
	virtual void OnDrawGizmos() override;
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(RigidBody) 
};

REGISTER_COMPONENT(RigidBody); 

