#include "pch.h"
#include "PhysicsManager.h"


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
}

void PhysicsManager::Update(float deltaTime)
{
	const vector<GameObject*> gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	mMapColInfo.clear();

	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		rigidBody->ApplyForce(Vec3(0.0f, -m_GravityAcceleration * rigidBody->GetMass(), 0.0f)); // 중력 적용 
		rigidBody->Integrate(deltaTime);
	}

	UpdateCollision();
}

void PhysicsManager::UpdateCollision()
{
	const std::vector<GameObject*> gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();

	for (size_t i = 0; i < gameObjects.size(); ++i)
	{
		for (size_t j = i + 1; j < gameObjects.size(); ++j)
		{
			GameObject* obj1 = gameObjects[i];
			GameObject* obj2 = gameObjects[j];

			// 자기 자신과의 충돌인 경우
			if (obj1->GetInstanceID() == obj2->GetInstanceID())
				continue;

			SphereCollider* sphere1 = obj1->GetComponent<SphereCollider>();
			SphereCollider* sphere2 = obj2->GetComponent<SphereCollider>();
			BoxCollider* box1 = obj1->GetComponent<BoxCollider>();
			BoxCollider* box2 = obj2->GetComponent<BoxCollider>();

			// 두 충돌체 조합 아이디 생성
			COLLIDER_ID colliderID;
			colliderID.Left_id = obj1->GetInstanceID();
			colliderID.RIght_id = obj2->GetInstanceID();

			Contact contact;

			// Sphere-Sphere collision
			if (sphere1 && sphere2)
			{
				if (CheckSphereSphereCollision(sphere1, sphere2, contact))
				{
					ResolveCollision(obj1, obj2, contact);
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Sphere-Box collision
			if (sphere1 && box2)
			{
				if (CheckSphereBoxCollision(sphere1, box2, contact))
				{
					ResolveCollision(obj1, obj2, contact);
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			if (box1 && sphere2)
			{
				if (CheckSphereBoxCollision(sphere2, box1, contact))
				{
					ResolveCollision(obj1, obj2, contact);
					HandleCollision(obj1, obj2, colliderID);
				}
			}
		}
	}
}

bool PhysicsManager::CheckSphereSphereCollision(SphereCollider* sphere1, SphereCollider* sphere2, Contact& contact)
{
	Transform* sphere1Tr = sphere1->GetGameObject()->GetTransform();
	Transform* sphere2Tr = sphere2->GetGameObject()->GetTransform();

	XMVECTOR sphere1Position = sphere1Tr->GetPosition();
	XMVECTOR sphere2Position = sphere2Tr->GetPosition();
	XMVECTOR diff = XMVectorSubtract(sphere1Position, sphere2Position);

	float distanceSquared = XMVectorGetX(XMVector3LengthSq(diff));
	float radius1 = sphere1->GetRadius() * XMVectorGetX(sphere1Tr->GetScale());
	float radius2 = sphere2->GetRadius() * XMVectorGetX(sphere2Tr->GetScale());
	float radiusSum = radius1 + radius2;

	if (distanceSquared <= radiusSum * radiusSum)
	{ 
		float distance = std::sqrt(distanceSquared);
		contact.point = XMVectorAdd(sphere1Position, XMVectorScale(diff, radius1 / radiusSum));
		contact.normal = XMVector3Normalize(diff);
		contact.penetrationDepth = radiusSum - distance;
		return true;
	}
	return false;
}

bool PhysicsManager::CheckSphereBoxCollision(SphereCollider* sphere, BoxCollider* box, Contact& contact)
{
	Transform* sphereTr = sphere->GetGameObject()->GetTransform();
	Transform* boxTr = box->GetGameObject()->GetTransform();

	XMVECTOR spherePosition = sphereTr->GetPosition();
	XMVECTOR boxPosition = boxTr->GetPosition();
	XMVECTOR boxScale = boxTr->GetScale();
	XMVECTOR boxHalfSize = XMVectorMultiply(box->GetSize(), XMVectorScale(boxScale, 0.5f));

	// 박스의 로컬 좌표로 구의 위치를 변환
	XMVECTOR boxRotationEuler = boxTr->GetLocalRotation(); // 오일러 각 (X, Y, Z 순서로 회전)
	XMMATRIX boxRotation = XMMatrixRotationRollPitchYawFromVector(boxRotationEuler);
	XMMATRIX boxWorld = XMMatrixScalingFromVector(boxScale) * boxRotation * XMMatrixTranslationFromVector(boxPosition);
	XMMATRIX boxWorldInverse = XMMatrixInverse(nullptr, boxWorld);
	XMVECTOR localSpherePosition = XMVector3TransformCoord(spherePosition, boxWorldInverse);

	// 박스의 로컬 좌표에서 구의 위치와 가장 가까운 점 찾기 
	XMVECTOR closestPoint = XMVectorClamp(localSpherePosition, XMVectorNegate(boxHalfSize), boxHalfSize);

	XMVECTOR diff = XMVectorSubtract(localSpherePosition, closestPoint);
	float distanceSquared = XMVectorGetX(XMVector3LengthSq(diff));
	float sphereRadius = sphere->GetRadius() * XMVectorGetX(sphereTr->GetScale());

	if (distanceSquared <= sphereRadius * sphereRadius)
	{
		float distance = std::sqrt(distanceSquared);
		// 충돌 지점을 월드 좌표로 변환
		contact.point = XMVector3TransformCoord(closestPoint, boxWorld);
		contact.normal = XMVector3Normalize(XMVector3TransformNormal(diff, boxRotation));
		contact.penetrationDepth = sphereRadius - distance;
		return true;
	}
	return false;
}

bool PhysicsManager::CheckBoxBoxCollision(BoxCollider* box1, BoxCollider* box2)
{
	return false;
}

void PhysicsManager::HandleCollision(GameObject* obj1, GameObject* obj2, COLLIDER_ID colliderID)
{
	if (mMapColInfo.find(colliderID.ID) == mMapColInfo.end())
	{
		// First time collision in this frame, handle collision
		// (collision response logic goes here)
		mMapColInfo[colliderID.ID] = true;
	}
}

void PhysicsManager::ResolveCollision(GameObject* obj1, GameObject* obj2, const Contact& contact)
{
	auto rb1 = obj1->GetComponent<RigidBody>();
	auto rb2 = obj2->GetComponent<RigidBody>();

	if (rb1 == nullptr && rb2 == nullptr)
		return;
	 
	XMVECTOR relativeVelocity = XMVectorZero();
	float invMass1 = 0.0f, invMass2 = 0.0f, restitution = 0.0f; 

	if (rb1)
	{
		relativeVelocity = rb2 ? rb2->GetVelocity() - rb1->GetVelocity() : -rb1->GetVelocity();
		invMass1 = rb1->GetInverseMass();
		restitution = rb2 ? min(rb1->GetRestitution(), rb2->GetRestitution()) : rb1->GetRestitution();
	}
	else
	{
		relativeVelocity = rb2->GetVelocity();
		invMass2 = rb2->GetInverseMass();
		restitution = rb2->GetRestitution();
	}
	 
	float velocityAlongNormal = XMVectorGetX(XMVector3Dot(relativeVelocity, contact.normal)); 
	if (velocityAlongNormal > 0) 
		return; 

	float j = -(1 + restitution) * velocityAlongNormal;
	j /= invMass1 + invMass2;

	XMVECTOR impulse = j * contact.normal;
	if (rb1)
	{
		rb1->ApplyImpulse(-impulse);
	}
	if (rb2)
	{
		rb2->ApplyImpulse(impulse);
	}
	
	// Positional correction to prevent sinking
	const float percent = 0.2f;
	const float slop = 0.01f;
	Vec3 correction = max(contact.penetrationDepth - slop, 0.0f) / (invMass1 + invMass2) * percent * contact.normal;
	if (rb1)
	{
		obj1->GetTransform()->SetPosition(obj1->GetTransform()->GetPosition() - invMass1 * correction); 
	}
	if (rb2)
	{
		obj2->GetTransform()->SetPosition(obj2->GetTransform()->GetPosition() + invMass2 * correction); 
	}
}