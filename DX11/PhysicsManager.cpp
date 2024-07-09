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
	UpdateCollision();

	ResolveCollision(m_Contacts, deltaTime);
	m_Contacts.clear();
}

void PhysicsManager::UpdateCollision()
{
	m_Contacts.clear();
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

			if (sphere1 && sphere2)
			{
				if (CheckSphereSphereCollision(sphere1, sphere2, contact))
				{
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Sphere-Box collision
			if (sphere1 && box2)
			{
				if (CheckSphereBoxCollision(sphere1, box2, contact))
				{
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Sphere-Box collision
			if (box1 && sphere2)
			{
				if (CheckSphereBoxCollision(sphere2, box1, contact))
				{
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Box-Box collision
			if (box1 && box2)
			{
				if (CheckBoxBoxCollision(box1, box2, contact))
				{
					HandleCollision(obj1, obj2, colliderID); 
				}
			}
		}
	}
}

float friction = 0.6f;
float objectRestitution = 0.3f;

bool PhysicsManager::CheckSphereSphereCollision(SphereCollider* sphere1, SphereCollider* sphere2, Contact& contact)
{
	Transform* sphere1Tr = sphere1->GetGameObject()->GetTransform();
	Transform* sphere2Tr = sphere2->GetGameObject()->GetTransform();

	RigidBody* sphere1Rb = sphere1->GetGameObject()->GetComponent<RigidBody>();
	RigidBody* sphere2Rb = sphere2->GetGameObject()->GetComponent<RigidBody>();

	/* 두 구 사이의 거리를 구한다 */
	float distanceSquared =
		(sphere1Tr->GetPosition() - sphere2Tr->GetPosition()).LengthSquared();

	/* 두 구 사이의 거리가 두 구의 반지름의 합보다 작다면 충돌이 발생한 것이다 */
	float radiusSum = sphere1->GetRadius() + sphere2->GetRadius();
	if (distanceSquared < radiusSum * radiusSum)
	{
		Vector3 centerToCenter = sphere1Tr->GetPosition() - sphere2Tr->GetPosition();
		centerToCenter.Normalize();

		/* 충돌 정보를 생성한다 */
		Contact* newContact = new Contact;
		newContact->bodies[0] = sphere1Rb;
		newContact->bodies[1] = sphere2Rb;
		newContact->normal = centerToCenter;
		newContact->contactPoint[0] = Vector3(sphere1Tr->GetPosition() - centerToCenter * sphere1->GetRadius());
		newContact->contactPoint[1] = Vector3(sphere2Tr->GetPosition() + centerToCenter * sphere2->GetRadius());
		newContact->penetration = radiusSum - sqrtf(distanceSquared);
		newContact->restitution = objectRestitution;
		newContact->friction = friction;
		newContact->normalImpulseSum = 0.0f;
		newContact->tangentImpulseSum1 = 0.0f;
		newContact->tangentImpulseSum2 = 0.0f;

		m_Contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool PhysicsManager::CheckSphereBoxCollision(SphereCollider* sphere, BoxCollider* box, Contact& contact)
{
	Transform* sphereTr = sphere->GetGameObject()->GetTransform();
	Transform* boxTr = box->GetGameObject()->GetTransform();

	/* 구의 중심을 직육면체의 로컬 좌표계로 변환한다 */
	Matrix boxWorld = box->GetGameObject()->GetTransform()->GetWorldMatrix();
	Matrix worldToLocal = box->GetGameObject()->GetTransform()->GetWorldMatrix().Invert(); 
	Vec3 sphereInBoxLocal = Vec3::Transform(sphere->GetGameObject()->GetTransform()->GetPosition(), worldToLocal);
	Vec3 boxHalfSize = box->GetSize() / 2;

	/* 구의 중심과 가장 가까운 직육면체 위의 점을 찾는다 */
	Vec3 closestPoint;
	/* x 축 성분 비교 */
	if (sphereInBoxLocal.x > boxHalfSize.x)
		closestPoint.x = boxHalfSize.x;
	else if (sphereInBoxLocal.x < -boxHalfSize.x)
		closestPoint.x = -boxHalfSize.x;
	else
		closestPoint.x = sphereInBoxLocal.x;
	/* y 축 성분 비교 */
	if (sphereInBoxLocal.y > boxHalfSize.y)
		closestPoint.y = boxHalfSize.y;
	else if (sphereInBoxLocal.y < -boxHalfSize.y)
		closestPoint.y = -boxHalfSize.y;
	else
		closestPoint.y = sphereInBoxLocal.y;
	/* z 축 성분 비교 */
	if (sphereInBoxLocal.z > boxHalfSize.z)
		closestPoint.z = boxHalfSize.z;
	else if (sphereInBoxLocal.z < -boxHalfSize.z)
		closestPoint.z = -boxHalfSize.z;
	else
		closestPoint.z = sphereInBoxLocal.z;

	/* 위의 결과와 구의 중심 사이의 거리가 구의 반지름보다 작다면 충돌이 발생한 것이다 */
	float distanceSquared = (closestPoint - sphereInBoxLocal).LengthSquared(); 
	if (distanceSquared < sphere->GetRadius() * sphere->GetRadius())
	{
		/* 구에 가장 가까운 직육면체 위의 점을 월드 좌표계로 변환한다 */
		Vector3 closestPointWorld = Vec3::Transform(closestPoint, boxWorld); 

		/* 충돌 정보를 생성한다 */
		Contact* newContact = new Contact;
		newContact->bodies[0] = sphere->GetGameObject()->GetComponent<RigidBody>();
		newContact->bodies[1] = box->GetGameObject()->GetComponent<RigidBody>();
		newContact->normal = sphere->GetGameObject()->GetTransform()->GetPosition() - closestPointWorld;
		newContact->normal.Normalize();
		newContact->contactPoint[0] = Vec3(sphereTr->GetPosition() - newContact->normal * sphere->GetRadius());
		newContact->contactPoint[1] = Vec3(closestPointWorld);
		newContact->penetration = sphere->GetRadius() - sqrtf(distanceSquared);
		newContact->restitution = objectRestitution;
		newContact->friction = friction;
		newContact->normalImpulseSum = 0.0f;
		newContact->tangentImpulseSum1 = 0.0f;
		newContact->tangentImpulseSum2 = 0.0f;

		m_Contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool PhysicsManager::CheckBoxBoxCollision(BoxCollider* box1, BoxCollider* box2, Contact& contact)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform();
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	RigidBody* box1Rb = box1->GetGameObject()->GetComponent<RigidBody>();
	RigidBody* box2Rb = box2->GetGameObject()->GetComponent<RigidBody>();

	/* 겹침 검사의 기준이 될 축 저장 */
	std::vector<Vector3> axes;

	/* box1 의 세 축 저장 */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box1.body->getAxis(i));

	/* box2 의 세 축 저장 */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box2.body->getAxis(i));

	/* 각 축 사이의 외적 저장 */
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Vector3 crossProduct = axes[i].Cross(axes[3 + j]);
			crossProduct.Normalize();
			axes.push_back(crossProduct);
		}
	}

	float minPenetration = FLT_MAX;
	int minAxisIdx = 0;

	/* 모든 축에 대해 겹침 검사 */
	for (int i = 0; i < axes.size(); ++i)
	{
		float penetration = calcPenetration(box1, box2, axes[i]);

		/* 한 축이라도 겹치지 않으면 충돌이 발생하지 않은 것이다 */
		if (penetration <= 0.0f)
			return false;

		/* 가장 적게 겹치는 정도와 그때의 기준 축을 추적한다 */
		if (penetration <= minPenetration)
		{
			minPenetration = penetration;
			minAxisIdx = i;
		}
	}

	/* 모든 축에 걸쳐 겹침이 감지됐다면 충돌이 발생한 것이다 */
	Contact* newContact = new Contact;
	newContact->bodies[0] = box1Rb;
	newContact->bodies[1] = box2Rb;
	newContact->penetration = minPenetration;
	newContact->restitution = objectRestitution;
	newContact->friction = friction;
	newContact->normalImpulseSum = 0.0f;
	newContact->tangentImpulseSum1 = 0.0f;
	newContact->tangentImpulseSum2 = 0.0f;

	/* 충돌 법선을 방향에 유의하여 설정한다 */
	Vector3 centerToCenter = box1Tr->GetPosition() - box2Tr->GetPosition();
	if (axes[minAxisIdx].Dot(centerToCenter) > 0)
		newContact->normal = axes[minAxisIdx] * -1.0f;
	else
		newContact->normal = axes[minAxisIdx];

	/* 충돌 지점을 찾는다 */
	if (minAxisIdx < 6) // 면-점 접촉일 때
	{
		calcContactPointOnPlane(box1, box2, minAxisIdx, newContact);
	}
	else // 선-선 접촉일 때
	{
		calcContactPointOnLine(box1, box2, minAxisIdx, newContact);
	}

	m_Contacts.push_back(newContact);
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

void PhysicsManager::ResolveCollision(std::vector<Contact*>& contacts, float deltaTime)
{
	for (int i = 0; i < 10; ++i)
	{
		for (auto& contact : contacts)
		{
			sequentialImpulse(contact, deltaTime);
		}
	}
}

float penetrationTolerance = 0.0005f;
float closingSpeedTolerance = 0.005f;

void PhysicsManager::sequentialImpulse(Contact* contact, float deltaTime)
{
	float effectiveMass;

	float totalInvMass = contact->bodies[0]->GetInverseMass();
	if (contact->bodies[1] != nullptr)
		totalInvMass += contact->bodies[1]->GetInverseMass();
	if (totalInvMass == 0.0f)
		return;

	Vec3 contactPointFromCenter1 = contact->contactPoint[0] - contact->bodies[0]->GetGameObject()->GetTransform()->GetPosition();
	Vector3 contactPointFromCenter2;
	if (contact->bodies[1] != nullptr)
		contactPointFromCenter2 = contact->contactPoint[1] - contact->bodies[1]->GetGameObject()->GetTransform()->GetPosition();

	Vec3 termInDenominator1 = Vec3::Transform(contactPointFromCenter1.Cross(contact->normal), contact->bodies[0]->GetInverseInertiaTensorWorld())
		.Cross(contactPointFromCenter1);

	Vec3 termInDenominator2;
	if (contact->bodies[1] != nullptr) {
		termInDenominator2 = Vec3::Transform(contactPointFromCenter2.Cross(contact->normal), contact->bodies[1]->GetInverseInertiaTensorWorld())
			.Cross(contactPointFromCenter2); 
	}
	effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(contact->normal);
	if (effectiveMass == 0.0f)
		return;

	float closingSpeed = contact->normal.Dot(contact->bodies[0]->GetVelocity())
		+ contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter1.Cross(contact->normal));
	if (contact->bodies[1] != nullptr)
	{
		closingSpeed -= contact->normal.Dot(contact->bodies[1]->GetVelocity())
			+ contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter2.Cross(contact->normal));
	}

	/* bias 계산 */
	float bias = 0.0f;
	/* Baumgarte Stabilization */
	float baumgarte = 0.0f;
	if (contact->penetration > penetrationTolerance)
		baumgarte = (-0.1f / deltaTime) * (contact->penetration - penetrationTolerance);
	bias += baumgarte;

	/* bias 에 restitution term 추가 */
	float restitutionTerm = 0.0f;
	if (closingSpeed > closingSpeedTolerance)
		restitutionTerm = contact->restitution * (closingSpeed - closingSpeedTolerance);
	bias -= restitutionTerm;

	float impulse = -(closingSpeed + bias) / effectiveMass;
	if (isnan(impulse) != 0)
	{
		std::cout << "ERROR::CollisionResolver::sequentialImpulse()::impulse is nan" << std::endl;
		return;
	}

	/* 충격량의 누적값을 clamp */
	float prevImpulseSum = contact->normalImpulseSum; 
	contact->normalImpulseSum += impulse; 
	if (contact->normalImpulseSum < 0.0f) 
		contact->normalImpulseSum = 0.0f; 
	impulse = contact->normalImpulseSum - prevImpulseSum;

	/* 속도 & 각속도 변화 */
	Vector3 linearImpulse = contact->normal * impulse;
	Vector3 angularImpulse1 = contactPointFromCenter1.Cross(contact->normal) * impulse;
	Vector3 angularImpulse2 = contactPointFromCenter2.Cross(contact->normal) * impulse;

	contact->bodies[0]->SetVelocity(
		contact->bodies[0]->GetVelocity() + linearImpulse * contact->bodies[0]->GetInverseMass()
	);
	contact->bodies[0]->GetGameObject()->GetTransform()->SetRotation( 
		contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation() + Vector3::Transform(angularImpulse1, contact->bodies[0]->GetInverseInertiaTensorWorld())
	); 

	if (contact->bodies[1] != nullptr)
	{
		contact->bodies[1]->SetVelocity(
			contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
		);
		contact->bodies[1]->GetGameObject()->GetTransform()->SetRotation(
			contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation() - Vector3::Transform(angularImpulse2, contact->bodies[1]->GetInverseInertiaTensorWorld())
		);
	}

	/* 충돌 법선에 수직하는 벡터 찾기(erin catto 방법) */
	Vector3 tangent1, tangent2;
	if (contact->normal.x >= 0.57735f)
		tangent1 = Vector3(contact->normal.y, -contact->normal.x, 0.0f);
	else
		tangent1 = Vector3(0.0f, contact->normal.z, -contact->normal.y);
	tangent2 = contact->normal.Cross(tangent1);

	/* tangent1 벡터에 대한 마찰 계산 */
	Vec3 termInDenominator1 = Vector3::Transform(contactPointFromCenter1.Cross(tangent1), contact->bodies[0]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter1);
	if (contact->bodies[1] != nullptr) {
		termInDenominator2 = Vector3::Transform(contactPointFromCenter2.Cross(tangent1), contact->bodies[1]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter2);
	}

	effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(tangent1);

	closingSpeed = tangent1.Dot(contact->bodies[0]->GetVelocity())
		+ contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter1.Cross(tangent1));
	if (contact->bodies[1] != nullptr)
	{
		closingSpeed -= tangent1.Dot(contact->bodies[1]->GetVelocity())
			+ contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter2.Cross(tangent1));
	}
	impulse = -closingSpeed / effectiveMass;
	if (isnan(impulse) != 0)
	{
		std::cout << "ERROR::CollisionResolver::sequentialImpulse()::tangential impulse1 is nan" << std::endl;
		return;
	}

	/* 충격량의 누적값을 clamp */
	prevImpulseSum = contact->tangentImpulseSum1;
	contact->tangentImpulseSum1 += impulse;
	if (contact->tangentImpulseSum1 < (-contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum1 = -contact->friction * contact->normalImpulseSum;
	else if (contact->tangentImpulseSum1 > (contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum1 = contact->friction * contact->normalImpulseSum;
	impulse = contact->tangentImpulseSum1 - prevImpulseSum;

	/* 속도 & 각속도 변화 */
	linearImpulse = tangent1 * impulse;
	angularImpulse1 = contactPointFromCenter1.Cross(tangent1) * impulse;
	angularImpulse2 = contactPointFromCenter2.Cross(tangent1) * impulse;

	contact->bodies[0]->SetVelocity(
		contact->bodies[0]->GetVelocity() + linearImpulse * contact->bodies[0]->GetInverseMass()
	);
	contact->bodies[0]->GetGameObject()->GetTransform()->SetRotation(
    contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation() + Vector3::Transform(angularImpulse1, contact->bodies[0]->GetInverseInertiaTensorWorld())
);
	if (contact->bodies[1] != nullptr)
	{
		contact->bodies[1]->SetVelocity(
			contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
		);
		contact->bodies[1]->GetGameObject()->GetTransform()->SetRotation(
			contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation() - Vec3::Transform(angularImpulse2, contact->bodies[1]->GetInverseInertiaTensorWorld())
		);
	}

	/* tangent2 벡터에 대한 마찰 계산 */
	termInDenominator1 = Vec3::Transform(contactPointFromCenter1.Cross(tangent2), contact->bodies[0]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter1);
	if (contact->bodies[1] != nullptr) {
		termInDenominator2 = Vec3::Transform(contactPointFromCenter2.Cross(tangent2),
			contact->bodies[1]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter2);
	}
	effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(tangent2);

	closingSpeed = tangent2.Dot(contact->bodies[0]->GetVelocity())
		+ contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter1.Cross(tangent2));
	if (contact->bodies[1] != nullptr)
	{
		closingSpeed -= tangent2.Dot(contact->bodies[1]->GetVelocity())
			+ contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter2.Cross(tangent2));
	}
	impulse = -closingSpeed / effectiveMass;
	if (isnan(impulse) != 0)
	{
		std::cout << "ERROR::CollisionResolver::sequentialImpulse()::tangential impulse2 is nan" << std::endl;
		return;
	}

	/* 충격량의 누적값을 clamp */
	prevImpulseSum = contact->tangentImpulseSum2;
	contact->tangentImpulseSum2 += impulse;
	if (contact->tangentImpulseSum2 < (-contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum2 = -contact->friction * contact->normalImpulseSum;
	else if (contact->tangentImpulseSum2 > (contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum2 = contact->friction * contact->normalImpulseSum;
	impulse = contact->tangentImpulseSum2 - prevImpulseSum;

	/* 속도 & 각속도 변화 */
	linearImpulse = tangent2 * impulse;
	angularImpulse1 = contactPointFromCenter1.Cross(tangent2) * impulse;
	angularImpulse2 = contactPointFromCenter2.Cross(tangent2) * impulse;

	contact->bodies[0]->SetVelocity(
		contact->bodies[0]->GetVelocity() + linearImpulse * contact->bodies[0]->GetInverseMass()
	);
	contact->bodies[0]->SetRotation(
		contact->bodies[0]->GetRotation() + Vec3::Transform(angularImpulse1, contact->bodies[0]->GetInverseInertiaTensorWorld())
	);
	if (contact->bodies[1] != nullptr)
	{
		contact->bodies[1]->SetVelocity(
			contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
		);
		contact->bodies[1]->SetRotation(
			contact->bodies[1]->GetRotation() - Vec3::Transform(angularImpulse2, contact->bodies[1]->GetInverseInertiaTensorWorld())
		);
	}
}



