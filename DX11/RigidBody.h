#pragma once
#include "Component.h"

class Transform;

class RigidBody
	: public Component
{
private:
	float m_Mass;						// ����
	float m_InverseMass;				// ������ ���� - ���нĿ� Ȱ��

	// Fixed
	/* ���� ���Ʈ ���ټ� */
	Matrix3 m_InverseInertiaTensor;			// ���� ��ǥ�� ����
	Matrix3 m_InverseInertiaTensorWorld;	// ���� ��ǥ�� ����

	Vec3 m_Velocity;				// ���ӵ�
	Vec3 m_RotationVelocity;		// ���ӵ�

	Vec3 m_Acceleration;			// ���ӵ� - �߷�
	Vec3 m_PrevAcceleration;		// ���� �����ӿ��� ��ü�� ���ӵ� - �߷� ���ӵ� + �ܺ� ���� ���� ���ӵ��� �����Ѵ�

	float m_LinearDamping;			// ���ӵ� ����
	float m_AngularDamping;			// ���ӵ� ����

	/* ��ü�� ������ �� & ��ũ */
	Vec3 m_Force;
	Vec3 m_Torque;
	
	bool m_IsKinematic;				// Ű��ƽ ����

public:
	RigidBody();
	~RigidBody();

public:
	void SetInertiaTensor(const Matrix3 mat);
	void SetInverseInertiaTensor(const Matrix3& mat); 
	Matrix3 GetInverseInertiaTensor() const { return m_InverseInertiaTensor; }   
	Matrix3 GetInverseInertiaTensorWorld() const { return m_InverseInertiaTensorWorld; }  

	void SetKinematic(bool isKinematic) { m_IsKinematic = isKinematic; }
	bool IsKinematic() const { return m_IsKinematic; }

	void SetAcceleration(Vec3 vec) { m_Acceleration = vec; }

	void SetVelocity(const Vec3& velocity) { m_Velocity = velocity; }
	Vec3 GetVelocity() const { return m_Velocity; }

	Vec3 GetRotationVelocity();
	void SetRotationVelocity(const Vec3& rotation);

	void SetMass(float mass);
	float GetMass() const { return m_Mass; }

	float GetInverseMass() const { return m_InverseMass; }

	void ApplyTorque(const Vec3& torque);
	void ApplyForce(const Vec3& force);
	void ApplyImpulse(const Vec3& impulse);
	
	void TransformInertiaTensor();


public:
	virtual void Awake() override;
	void Integrate(float deltaTime);


public:
	virtual void OnDrawGizmos() override;
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(RigidBody) 
};

REGISTER_COMPONENT(RigidBody); 

