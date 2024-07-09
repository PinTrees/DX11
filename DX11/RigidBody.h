#pragma once
#include "Component.h"

class Transform;

class RigidBody
	: public Component
{
private:
	float m_Mass;						// ����
	//float m_InverseMass;				// ������ ����. - ���нĿ� Ȱ��

	/* ���� ���Ʈ ���ټ� */
	Matrix m_InverseInertiaTensor;		// ��ȸ������ ���� ��ǥ�� ����
	Matrix m_InverseInertiaTensorWorld;	// ��ȸ������ ���� ��ǥ�� ����

	Vec3 m_Velocity;				// ���ӵ�
	Vec3 m_RotationVelocity;		// ���ӵ�

	Vec3 m_Acceleration;			// ���ӵ�
	Vec3 m_PrevAcceleration;		// ���� �����ӿ��� ��ü�� ���ӵ� - �߷� ���ӵ� + �ܺ� ���� ���� ���ӵ��� �����Ѵ�

	float m_LinearDamping;			// ���ӵ� ����
	float m_AngularDamping;			// ���ӵ� ����

	/* ��ü�� ������ �� & ��ũ */
	Vec3 m_Force;
	Vec3 m_Torque;
	
	bool m_IsKinematic;				// Ű��ƽ ����
	Matrix m_InertiaTensor;			// ȸ������		(Inertia Tensor)

	Vec3 m_CenterOfMass;			// ���� �߽�

public:
	RigidBody();
	~RigidBody();

public:
	/* getter */
	Matrix GetInverseInertiaTensor() const { return m_InverseInertiaTensor; };
	Matrix GetInverseInertiaTensorWorld() const { return m_InverseInertiaTensorWorld; };
	
	void SetKinematic(bool isKinematic) { m_IsKinematic = isKinematic; }
	bool IsKinematic() const { return m_IsKinematic; }

	void SetVelocity(const Vec3& velocity) { m_Velocity = velocity; }
	Vec3 GetVelocity() const { return m_Velocity; }

	void SetMass(float mass) { m_Mass = mass; }
	float GetMass() const { return m_Mass; }

	Vec3 GetCenterOfMass();

	float GetInverseMass() const { return (m_Mass == 0) ? 0 : 1.0f / m_Mass; } 
	//float GetRestitution() const { return m_Elasticity; } 
	//float GetFriction() const { return m_Friction; }

	void ApplyTorque(const Vec3& torque);
	void ApplyForce(const Vec3& force);
	void ApplyImpulse(const Vec3& impulse);
	
	// �������̵�
	Vec3 GetRotation();
	void SetRotation(const Vec3& rotation);
	void TransformInertiaTensor();

	void Integrate(float deltaTime);

public:
	virtual void OnDrawGizmos() override;
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(RigidBody) 
};

REGISTER_COMPONENT(RigidBody); 

