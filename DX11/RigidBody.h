#pragma once
#include "Component.h"

class Transform;

// ����: F = ma���� ���ӵ��� ���ϴ� �� �ʿ�.
// ���ӵ� : ���ӵ��� �ð����� ������Ʈ.
// ȸ������(Inertia Tensor) : ȸ�� �����п��� ���.
// ���ӵ� : �����ӵ��� �ð����� ������Ʈ.
// ���� �߽�(Center of Mass) : ��ü�� ��ġ ǥ��.
// ���� ��ǥ�� : x, y, z ��(��ü�� ȸ�� ���¸� ��Ÿ��).
// �浹 üũ�� ���� ��� ����(AABB, OBB, Sphere ��).
// ���� ��� : ��ü�� ��ġ �� ȸ���� ����.
// �߰� ��� : ��������, ������, ź�����, ������� ��.	

class RigidBody
	: public Component
{
private:
	Vec3 m_Velocity;				// ���ӵ�
	Vec3 m_Acceleration;			// ���ӵ�
	Vec3 m_AngularVelocity;			// ���ӵ�
	Vec3 m_AngularAcceleration;		// �����ӵ�
	float m_Mass;					// ����
	float m_Elasticity;				// �ݹ� ���
	float m_AirResistance;			// ��������
	float m_Friction;				// ������
	bool m_IsKinematic;				// Ű��ƽ ����
	Matrix m_InertiaTensor;			// ȸ������		(Inertia Tensor)
	Matrix m_InverseInertiaTensor;	// ��ȸ������

	Vec3 m_CenterOfMass;			// ���� �߽�

	bool m_IsMovable;				// ������ �� �ִ°�
	float m_Restitution;			// ź�����
	bool m_Sleep;					// Sleep ����

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
	float GetFriction() const { return m_Friction; }

	void ApplyTorque(const Vec3& torque);
	void ApplyForce(const Vec3& force);
	void ApplyImpulse(const Vec3& impulse);
	

	void Integrate(float deltaTime);

public:
	virtual void OnDrawGizmos() override;
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(RigidBody) 
};

REGISTER_COMPONENT(RigidBody); 

