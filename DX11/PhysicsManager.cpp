#include "pch.h"
#include "PhysicsManager.h"
#include "Debug.h"
#include "CollisionDetector.h"
#include "CollisionResolver.h"

// SAT(Separating Axis Theorem)
// GJK / EPA(Gilbert - Johnson - Keerthi / Expanding Polytope Algorithm)

// 충돌 시 적용되는 힘 계산.
// 침투 깊이(Penetration Depth) 및 충돌 법선 벡터(Contact Normal) 계산.
// Impulse 기반 : 직관적인 방식.
// Constraint 기반 : LCP Solver 사용.
// 위치 보정 : 침투 깊이만큼 물체를 밀어내는 위치 보정.

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
	/* 물체들을 적분한다 */
	auto gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		rigidBody->Integrate(deltaTime);
	}

	/* 충돌들을 처리한다 */
	m_Detector->DetectCollision(m_Contacts, gameObjects);
	m_Resolver->ResolveCollision(m_Contacts, deltaTime);
	Safe_Delete_Vec(m_Contacts);
}
