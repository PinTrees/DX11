#include "pch.h"
#include "PhysicsManager.h"
#include "Debug.h"

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
}

void PhysicsManager::Update(float deltaTime)
{
	auto gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
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
	mMapColInfo.clear();
	map<ULONGLONG, bool>::iterator iter;

	const std::vector<GameObject*> gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();

	for (size_t i = 0; i < gameObjects.size(); ++i)
	{
		for (size_t j = 0; j < gameObjects.size(); ++j)
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

			iter = mMapColInfo.find(colliderID.ID);

			if (mMapColInfo.end() == iter)
			{
				mMapColInfo.insert(make_pair(colliderID.ID, false)); 
				iter = mMapColInfo.find(colliderID.ID); 
			}

			Contact contact;

			// Sphere-Sphere collision
			//if (sphere1 && sphere2)
			//{
			//	if (CheckSphereSphereCollision(sphere1, sphere2, contact))
			//	{
			//		ResolveCollision(obj1, obj2, contact);
			//		HandleCollision(obj1, obj2, colliderID);
			//	}
			//}

			// Sphere-Box collision
			if (sphere1 && box2)
			{
				if (CheckSphereBoxCollision(sphere1, box2, contact))
				{
					ResolveCollision(obj1, obj2, contact);
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Sphere-Box collision
			if (box1 && sphere2)
			{
				if (CheckSphereBoxCollision(sphere2, box1, contact))
				{
					ResolveCollision(obj1, obj2, contact);
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Box-Box collision
			if (box1 && box2)
			{
				if (CheckBoxBoxCollision(box1, box2, contact))
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
	float radius1 = sphere1->GetRadius() * sphere1Tr->GetScale().x * 0.5f;
	float radius2 = sphere2->GetRadius() * sphere2Tr->GetScale().x * 0.5f;
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
	XMVECTOR boxHalfSize = box->GetSize() * 0.5f;

	// 박스의 로컬 좌표로 구의 위치를 변환
	XMVECTOR boxRotationEuler = boxTr->GetLocalRotation(); // 오일러 각 (라디안 단위)
	XMMATRIX boxRotation = XMMatrixRotationRollPitchYawFromVector(boxRotationEuler);
	XMMATRIX boxWorld = XMMatrixScalingFromVector(boxScale) * boxRotation * XMMatrixTranslationFromVector(boxPosition);
	XMMATRIX boxWorldInverse = XMMatrixInverse(nullptr, boxWorld);
	XMVECTOR localSpherePosition = XMVector3TransformCoord(spherePosition, boxWorldInverse);

	// 박스의 로컬 좌표에서 구의 위치와 가장 가까운 점 찾기 
	XMVECTOR closestPoint = XMVectorClamp(localSpherePosition, XMVectorNegate(boxHalfSize), boxHalfSize);

	XMVECTOR diff = XMVectorSubtract(localSpherePosition, closestPoint);
	float distanceSquared = XMVectorGetX(XMVector3LengthSq(diff));
	float sphereRadius = sphere->GetRadius() * XMVectorGetX(sphereTr->GetScale()) * 0.5f;

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

bool PhysicsManager::CheckBoxBoxCollision(BoxCollider* box1, BoxCollider* box2, Contact& contact)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform();
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	Vec3 box1Center = box1Tr->GetPosition();
	Vec3 box2Center = box2Tr->GetPosition();

	// 박스 콜라이더의 사이즈에 트랜스폼의 스케일을 반영하여 실제 크기를 계산합니다.
	Vec3 box1Size = box1->GetSize() * box1Tr->GetScale();
	Vec3 box2Size = box2->GetSize() * box2Tr->GetScale();

	Matrix box1World = box1Tr->GetWorldMatrix();
	Matrix box2World = box2Tr->GetWorldMatrix();

	// 각 박스의 로컬 축을 추출하고 정규화합니다.
	Vec3 box1Axes[3] = {
	   Normalize(Vec3(box1World._11, box1World._12, box1World._13)), 
	   Normalize(Vec3(box1World._21, box1World._22, box1World._23)), 
	   Normalize(Vec3(box1World._31, box1World._32, box1World._33)) 
	};

	Vec3 box2Axes[3] = {
		Normalize(Vec3(box2World._11, box2World._12, box2World._13)), 
		Normalize(Vec3(box2World._21, box2World._22, box2World._23)), 
		Normalize(Vec3(box2World._31, box2World._32, box2World._33)) 
	};

	// 두 박스의 중심 간의 번역 벡터를 계산합니다.
	Vec3 translation = box2Center - box1Center; 

	// 번역 벡터를 첫 번째 박스의 로컬 축에 투영하여 로컬 좌표로 변환합니다.
	float translationLocal[3] = {
		Dot(translation, box1Axes[0]), 
		Dot(translation, box1Axes[1]), 
		Dot(translation, box1Axes[2]) 
	};

	// 각 박스의 절반 크기를 계산합니다.
	float box1Extents[3] = {
		box1Size.x / 2.0f,
		box1Size.y / 2.0f,
		box1Size.z / 2.0f
	};
	float box2Extents[3] = {
		box2Size.x / 2.0f,
		box2Size.y / 2.0f,
		box2Size.z / 2.0f
	};

	// 두 박스의 축 간의 내적을 계산하고 절대값을 구합니다.
	float R[3][3];
	float AbsR[3][3];

	for (int i = 0; i < 3; ++i) { 
		for (int j = 0; j < 3; ++j) { 
			R[i][j] = Dot(box1Axes[i], box2Axes[j]); 
			AbsR[i][j] = fabs(R[i][j]) + FLT_EPSILON; 
		}
	}

	// SAT를 사용하여 충돌을 검사합니다.
	float minPenetration = FLT_MAX;
	Vec3 bestAxis; 

	// 첫 번째 박스의 축을 기준으로 충돌을 검사합니다.
	for (int i = 0; i < 3; ++i) {
		float penetration = box1Extents[i] + box2Extents[0] * AbsR[i][0] + box2Extents[1] * AbsR[i][1]
			+ box2Extents[2] * AbsR[i][2] - fabs(translationLocal[i]);
		if (penetration < 0) return false;
		if (penetration < minPenetration) {
			minPenetration = penetration;
			bestAxis = box1Axes[i];
		}
	}

	// 두 번째 박스의 축을 기준으로 충돌을 검사합니다.
	for (int j = 0; j < 3; ++j) {
		float penetration = box1Extents[0] * AbsR[0][j] + box1Extents[1] * AbsR[1][j]
			+ box1Extents[2] * AbsR[2][j] + box2Extents[j] - fabs(Dot(translation, box2Axes[j]));
		if (penetration < 0) return false;
		if (penetration < minPenetration) {
			minPenetration = penetration;
			bestAxis = box2Axes[j];
		}
	}

	// 교차 축을 기준으로 충돌을 검사합니다.
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			Vec3 axis = Cross(box1Axes[i], box2Axes[j]);
			float ra = box1Extents[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + box1Extents[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
			float rb = box2Extents[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + box2Extents[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
			float penetration = ra + rb - fabs(Dot(translation, axis));
			if (penetration < 0) return false;
			if (penetration < minPenetration) {
				minPenetration = penetration;
				bestAxis = axis;
			}
		}
	}

	// 충돌이 발생한 경우 충돌 지점과 법선 벡터, 침투 깊이를 계산합니다.
	contact.normal = Normalize(bestAxis);
	contact.penetrationDepth = minPenetration;
	contact.point = box1Center + contact.normal * (minPenetration / 2.0f);

	Debug::Log("Contact Point: " + ToString(contact.point));
	Debug::Log("Contact PenetrationDepth: " + to_string(contact.penetrationDepth)); 

	return true;
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
	RigidBody* rb1 = obj1->GetComponent<RigidBody>();
	RigidBody* rb2 = obj2->GetComponent<RigidBody>();

	// 두 객체 모두 리지드바디 컴포넌트가 없다면 함수를 종료합니다.
	if (rb1 == nullptr && rb2 == nullptr)
		return;

	// 두 객체 모두 키네틱 상태라면 함수를 종료합니다. 
	if ((rb1 && rb1->IsKinematic()) && (rb2 && rb2->IsKinematic()))
		return;

	// 상대 속도 계산: 두 객체 모두 리지드바디가 있다면 차이를, 하나만 있다면 해당 리지드바디의 속도를 사용합니다.
	Vec3 relativeVelocity;
	if (rb1 && rb2) relativeVelocity = rb2->GetVelocity() - rb1->GetVelocity();
	else if (rb1) relativeVelocity = -rb1->GetVelocity();
	else if (rb2) relativeVelocity = rb2->GetVelocity();

	// 역질량 계산: 각 객체에 리지드바디가 있다면 역질량을, 없다면 0을 사용합니다.
	float invMass1 = (rb1 && !rb1->IsKinematic()) ? rb1->GetInverseMass() : 0.0f;
	float invMass2 = (rb2 && !rb2->IsKinematic()) ? rb2->GetInverseMass() : 0.0f;

	// 복원 계수 계산: 두 객체 모두 리지드바디가 있다면 최소 복원 계수를, 하나만 있다면 해당 리지드바디의 복원 계수를 사용합니다.
	float restitution = 0.0f;
	if (rb1 && rb2) restitution = min(rb1->GetRestitution(), rb2->GetRestitution());
	else if (rb1) restitution = rb1->GetRestitution();
	else if (rb2) restitution = rb2->GetRestitution();

	// 충돌 정규에 따른 속도 계산
	float velocityAlongNormal = relativeVelocity.Dot(contact.normal);
	// 두 객체가 서로 멀어지는 경우, 충돌 해결을 진행하지 않습니다.
	if (velocityAlongNormal > 0)
		return;

	// 충격량 계산
	float j = -(1 + restitution) * velocityAlongNormal;
	j /= invMass1 + invMass2;

	// 충격량 적용
	Vec3 impulse = contact.normal * j;
	if (rb1 && !rb1->IsKinematic()) rb1->ApplyImpulse(-impulse);
	if (rb2 && !rb2->IsKinematic()) rb2->ApplyImpulse(impulse);

	// 토크 적용
	Vec3 r1 = contact.point - obj1->GetTransform()->GetPosition();
	Vec3 r2 = contact.point - obj2->GetTransform()->GetPosition();
	Vec3 torque1 = Cross(r1, -impulse);
	Vec3 torque2 = Cross(r2, impulse);
	if (rb1 && !rb1->IsKinematic()) rb1->ApplyTorque(torque1);
	if (rb2 && !rb2->IsKinematic()) rb2->ApplyTorque(torque2);

	// 위치 보정
	const float percent = 0.2f;     // usually 20% to 80%
	const float slop = 0.01f;       // usually small like 0.01 to 0.1

	float penetrationCorrection = max(contact.penetrationDepth - slop, 0.0f) / (invMass1 + invMass2) * percent;
	Vec3 correction = contact.normal * penetrationCorrection;

	if (rb1 && !rb1->IsKinematic()) obj1->GetTransform()->Translate(-correction * invMass1);
	if (rb2 && !rb2->IsKinematic()) obj2->GetTransform()->Translate(correction * invMass2);

	// 마찰력 적용
	Vec3 tangent = relativeVelocity - (contact.normal * velocityAlongNormal);
	tangent = Normalize(tangent);
	float jt = -relativeVelocity.Dot(tangent);
	jt /= invMass1 + invMass2;

	// 마찰 계수
	float friction = rb1 ? rb1->GetFriction() : 0.0f;
	if (rb2) friction = min(friction, rb2->GetFriction());

	// 마찰력 적용
	Vec3 frictionImpulse;
	if (abs(jt) < j * friction)
	{
		frictionImpulse = tangent * jt;
	}
	else
	{
		frictionImpulse = tangent * -j * friction;
	}

	if (rb1 && !rb1->IsKinematic()) rb1->ApplyImpulse(-frictionImpulse); 
	if (rb2 && !rb2->IsKinematic()) rb2->ApplyImpulse(frictionImpulse); 
}