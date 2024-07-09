#include "pch.h"
#include "PhysicsManager.h"
#include "Debug.h"

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
}

void PhysicsManager::Update(float deltaTime)
{
	auto gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		rigidBody->ApplyForce(Vec3(0.0f, -m_GravityAcceleration * rigidBody->GetMass(), 0.0f)); // �߷� ���� 
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

			// �ڱ� �ڽŰ��� �浹�� ���
			if (obj1->GetInstanceID() == obj2->GetInstanceID())
				continue;

			SphereCollider* sphere1 = obj1->GetComponent<SphereCollider>();
			SphereCollider* sphere2 = obj2->GetComponent<SphereCollider>();
			BoxCollider* box1 = obj1->GetComponent<BoxCollider>();
			BoxCollider* box2 = obj2->GetComponent<BoxCollider>();

			// �� �浹ü ���� ���̵� ����
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

	// �ڽ��� ���� ��ǥ�� ���� ��ġ�� ��ȯ
	XMVECTOR boxRotationEuler = boxTr->GetLocalRotation(); // ���Ϸ� �� (���� ����)
	XMMATRIX boxRotation = XMMatrixRotationRollPitchYawFromVector(boxRotationEuler);
	XMMATRIX boxWorld = XMMatrixScalingFromVector(boxScale) * boxRotation * XMMatrixTranslationFromVector(boxPosition);
	XMMATRIX boxWorldInverse = XMMatrixInverse(nullptr, boxWorld);
	XMVECTOR localSpherePosition = XMVector3TransformCoord(spherePosition, boxWorldInverse);

	// �ڽ��� ���� ��ǥ���� ���� ��ġ�� ���� ����� �� ã�� 
	XMVECTOR closestPoint = XMVectorClamp(localSpherePosition, XMVectorNegate(boxHalfSize), boxHalfSize);

	XMVECTOR diff = XMVectorSubtract(localSpherePosition, closestPoint);
	float distanceSquared = XMVectorGetX(XMVector3LengthSq(diff));
	float sphereRadius = sphere->GetRadius() * XMVectorGetX(sphereTr->GetScale()) * 0.5f;

	if (distanceSquared <= sphereRadius * sphereRadius)
	{
		float distance = std::sqrt(distanceSquared);
		// �浹 ������ ���� ��ǥ�� ��ȯ
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

	// �ڽ� �ݶ��̴��� ����� Ʈ�������� �������� �ݿ��Ͽ� ���� ũ�⸦ ����մϴ�.
	Vec3 box1Size = box1->GetSize() * box1Tr->GetScale();
	Vec3 box2Size = box2->GetSize() * box2Tr->GetScale();

	Matrix box1World = box1Tr->GetWorldMatrix();
	Matrix box2World = box2Tr->GetWorldMatrix();

	// �� �ڽ��� ���� ���� �����ϰ� ����ȭ�մϴ�.
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

	// �� �ڽ��� �߽� ���� ���� ���͸� ����մϴ�.
	Vec3 translation = box2Center - box1Center; 

	// ���� ���͸� ù ��° �ڽ��� ���� �࿡ �����Ͽ� ���� ��ǥ�� ��ȯ�մϴ�.
	float translationLocal[3] = {
		Dot(translation, box1Axes[0]), 
		Dot(translation, box1Axes[1]), 
		Dot(translation, box1Axes[2]) 
	};

	// �� �ڽ��� ���� ũ�⸦ ����մϴ�.
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

	// �� �ڽ��� �� ���� ������ ����ϰ� ���밪�� ���մϴ�.
	float R[3][3];
	float AbsR[3][3];

	for (int i = 0; i < 3; ++i) { 
		for (int j = 0; j < 3; ++j) { 
			R[i][j] = Dot(box1Axes[i], box2Axes[j]); 
			AbsR[i][j] = fabs(R[i][j]) + FLT_EPSILON; 
		}
	}

	// SAT�� ����Ͽ� �浹�� �˻��մϴ�.
	float minPenetration = FLT_MAX;
	Vec3 bestAxis; 

	// ù ��° �ڽ��� ���� �������� �浹�� �˻��մϴ�.
	for (int i = 0; i < 3; ++i) {
		float penetration = box1Extents[i] + box2Extents[0] * AbsR[i][0] + box2Extents[1] * AbsR[i][1]
			+ box2Extents[2] * AbsR[i][2] - fabs(translationLocal[i]);
		if (penetration < 0) return false;
		if (penetration < minPenetration) {
			minPenetration = penetration;
			bestAxis = box1Axes[i];
		}
	}

	// �� ��° �ڽ��� ���� �������� �浹�� �˻��մϴ�.
	for (int j = 0; j < 3; ++j) {
		float penetration = box1Extents[0] * AbsR[0][j] + box1Extents[1] * AbsR[1][j]
			+ box1Extents[2] * AbsR[2][j] + box2Extents[j] - fabs(Dot(translation, box2Axes[j]));
		if (penetration < 0) return false;
		if (penetration < minPenetration) {
			minPenetration = penetration;
			bestAxis = box2Axes[j];
		}
	}

	// ���� ���� �������� �浹�� �˻��մϴ�.
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

	// �浹�� �߻��� ��� �浹 ������ ���� ����, ħ�� ���̸� ����մϴ�.
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

	// �� ��ü ��� ������ٵ� ������Ʈ�� ���ٸ� �Լ��� �����մϴ�.
	if (rb1 == nullptr && rb2 == nullptr)
		return;

	// �� ��ü ��� Ű��ƽ ���¶�� �Լ��� �����մϴ�. 
	if ((rb1 && rb1->IsKinematic()) && (rb2 && rb2->IsKinematic()))
		return;

	// ��� �ӵ� ���: �� ��ü ��� ������ٵ� �ִٸ� ���̸�, �ϳ��� �ִٸ� �ش� ������ٵ��� �ӵ��� ����մϴ�.
	Vec3 relativeVelocity;
	if (rb1 && rb2) relativeVelocity = rb2->GetVelocity() - rb1->GetVelocity();
	else if (rb1) relativeVelocity = -rb1->GetVelocity();
	else if (rb2) relativeVelocity = rb2->GetVelocity();

	// ������ ���: �� ��ü�� ������ٵ� �ִٸ� ��������, ���ٸ� 0�� ����մϴ�.
	float invMass1 = (rb1 && !rb1->IsKinematic()) ? rb1->GetInverseMass() : 0.0f;
	float invMass2 = (rb2 && !rb2->IsKinematic()) ? rb2->GetInverseMass() : 0.0f;

	// ���� ��� ���: �� ��ü ��� ������ٵ� �ִٸ� �ּ� ���� �����, �ϳ��� �ִٸ� �ش� ������ٵ��� ���� ����� ����մϴ�.
	float restitution = 0.0f;
	if (rb1 && rb2) restitution = min(rb1->GetRestitution(), rb2->GetRestitution());
	else if (rb1) restitution = rb1->GetRestitution();
	else if (rb2) restitution = rb2->GetRestitution();

	// �浹 ���Կ� ���� �ӵ� ���
	float velocityAlongNormal = relativeVelocity.Dot(contact.normal);
	// �� ��ü�� ���� �־����� ���, �浹 �ذ��� �������� �ʽ��ϴ�.
	if (velocityAlongNormal > 0)
		return;

	// ��ݷ� ���
	float j = -(1 + restitution) * velocityAlongNormal;
	j /= invMass1 + invMass2;

	// ��ݷ� ����
	Vec3 impulse = contact.normal * j;
	if (rb1 && !rb1->IsKinematic()) rb1->ApplyImpulse(-impulse);
	if (rb2 && !rb2->IsKinematic()) rb2->ApplyImpulse(impulse);

	// ��ũ ����
	Vec3 r1 = contact.point - obj1->GetTransform()->GetPosition();
	Vec3 r2 = contact.point - obj2->GetTransform()->GetPosition();
	Vec3 torque1 = Cross(r1, -impulse);
	Vec3 torque2 = Cross(r2, impulse);
	if (rb1 && !rb1->IsKinematic()) rb1->ApplyTorque(torque1);
	if (rb2 && !rb2->IsKinematic()) rb2->ApplyTorque(torque2);

	// ��ġ ����
	const float percent = 0.2f;     // usually 20% to 80%
	const float slop = 0.01f;       // usually small like 0.01 to 0.1

	float penetrationCorrection = max(contact.penetrationDepth - slop, 0.0f) / (invMass1 + invMass2) * percent;
	Vec3 correction = contact.normal * penetrationCorrection;

	if (rb1 && !rb1->IsKinematic()) obj1->GetTransform()->Translate(-correction * invMass1);
	if (rb2 && !rb2->IsKinematic()) obj2->GetTransform()->Translate(correction * invMass2);

	// ������ ����
	Vec3 tangent = relativeVelocity - (contact.normal * velocityAlongNormal);
	tangent = Normalize(tangent);
	float jt = -relativeVelocity.Dot(tangent);
	jt /= invMass1 + invMass2;

	// ���� ���
	float friction = rb1 ? rb1->GetFriction() : 0.0f;
	if (rb2) friction = min(friction, rb2->GetFriction());

	// ������ ����
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