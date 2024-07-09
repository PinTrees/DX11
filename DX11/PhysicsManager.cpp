#include "pch.h"
#include "PhysicsManager.h"
#include "Debug.h"
#include "CollisionDetector.h"
#include "CollisionResolver.h"

// SAT(Separating Axis Theorem)
// GJK / EPA(Gilbert - Johnson - Keerthi / Expanding Polytope Algorithm)

// �浹 �� ����Ǵ� �� ���.
// ħ�� ����(Penetration Depth) �� �浹 ���� ����(Contact Normal) ���.
// Impulse ��� : �������� ���.
// Constraint ��� : LCP Solver ���.
// ��ġ ���� : ħ�� ���̸�ŭ ��ü�� �о�� ��ġ ����.

SINGLE_BODY(PhysicsManager)

PhysicsManager::PhysicsManager()
	: m_GravityAcceleration(9.8f)
{

}

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Init()
{
	SetGravity(9.8f);

	m_Detector = new CollisionDetector; 
	m_Resolver = new CollisionResolver;
}

void PhysicsManager::Update(float deltaTime)
{
	/* ��ü���� �����Ѵ� */
	auto gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		rigidBody->Integrate(deltaTime);
	}

	/* �浹���� ó���Ѵ� */
	m_Detector->DetectCollision(m_Contacts, gameObjects);
	m_Resolver->ResolveCollision(m_Contacts, deltaTime);
	Safe_Delete_Vec(m_Contacts);
}
