#include "pch.h"
#include "CollisionDetector.h"

CollisionDetector::CollisionDetector()
	: friction(0.6f),
	objectRestitution(0.3f),
	groundRestitution(0.2f) 
{
}

void CollisionDetector::DetectCollision(std::vector<Contact*>& contacts, vector<GameObject*>& gameObjects)
{
    for (size_t i = 0; i < gameObjects.size(); ++i)
    {
        Collider* colliderPtrI = gameObjects[i]->GetComponent<Collider>();
        if (!colliderPtrI) continue;

        for (size_t j = i + 1; j < gameObjects.size(); ++j)
        {
            Collider* colliderPtrJ = gameObjects[j]->GetComponent<Collider>(); 
            if (!colliderPtrJ) continue;

            if (typeid(*colliderPtrI) == typeid(SphereCollider))
            {
                SphereCollider* collider1 = static_cast<SphereCollider*>(colliderPtrI);
                if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // 구 - 구 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
					CheckSphereSphereCollision(contacts, collider1, collider2);
                }
                else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // 구 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
                    CheckSphereBoxCollision(contacts, collider1, collider2);
                }
            }
            else if (typeid(*colliderPtrI) == typeid(BoxCollider))
            {
                BoxCollider* collider1 = static_cast<BoxCollider*>(colliderPtrI);
                if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // 구 - 직육면체 충돌
                {
                    SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
					CheckSphereBoxCollision(contacts, collider2, collider1);
                }
                else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // 직육면체 - 직육면체 충돌
                {
                    BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
                    CheckBoxBoxCollision(contacts, collider1, collider2);
                }
            }
        }
    }
}


bool CollisionDetector::CheckSphereSphereCollision(std::vector<Contact*>& contacts, SphereCollider* sphere1, SphereCollider* sphere2)
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

		contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool CollisionDetector::CheckSphereBoxCollision(std::vector<Contact*>& contacts, SphereCollider* sphere, BoxCollider* box)
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

		contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool CollisionDetector::CheckBoxBoxCollision(std::vector<Contact*>& contacts, BoxCollider* box1, BoxCollider* box2)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform();
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	RigidBody* box1Rb = box1->GetGameObject()->GetComponent<RigidBody>();
	RigidBody* box2Rb = box2->GetGameObject()->GetComponent<RigidBody>();

	/* 겹침 검사의 기준이 될 축 저장 */
	std::vector<Vector3> axes;

	/* box1 의 세 축 저장 */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box1Tr->GetAxis(i));

	/* box2 의 세 축 저장 */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box2Tr->GetAxis(i));

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

	contacts.push_back(newContact);
	return true;
}




float CollisionDetector::calcPenetration(BoxCollider* box1, BoxCollider* box2, const Vector3& axis)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform(); 
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	Vec3 box1HalfSize = box1->GetSize() / 2;
	Vec3 box2HalfSize = box1->GetSize() / 2;

	/* 두 박스의 중심 간 거리를 계산한다 */
	Vector3 centerToCenter = box2Tr->GetPosition() - box1Tr->GetPosition();
	float projectedCenterToCenter = abs(centerToCenter.Dot(axis));

	/* 두 박스를 주어진 축에 사영시킨 길이의 합을 계산한다 */
	float projectedSum =
		  abs((box1Tr->GetAxis(0) * box1HalfSize.x).Dot(axis)) 
		+ abs((box1Tr->GetAxis(1) * box1HalfSize.y).Dot(axis)) 
		+ abs((box1Tr->GetAxis(2) * box1HalfSize.z).Dot(axis))
		+ abs((box2Tr->GetAxis(0) * box2HalfSize.x).Dot(axis))
		+ abs((box2Tr->GetAxis(1) * box2HalfSize.y).Dot(axis))
		+ abs((box2Tr->GetAxis(2) * box2HalfSize.z).Dot(axis));

	/* "사영시킨 길이의 합 - 중심 간 거리" 가 겹친 정도이다 */
	return projectedSum - projectedCenterToCenter;
}




void CollisionDetector::calcContactPointOnPlane(
	BoxCollider* box1,
	BoxCollider* box2,
	int minAxisIdx,
	Contact* contact
)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform(); 
	Transform* box2Tr = box2->GetGameObject()->GetTransform(); 

	Vec3 box1HalfSize = box1->GetSize() / 2;
	Vec3 box2HalfSize = box1->GetSize() / 2;

	/* 충돌 정점 */
	Vec3 contactPoint1;
	Vec3 contactPoint2;

	if (minAxisIdx < 3) // 충돌면이 box1 의 면일 때
	{
		contactPoint2 = Vec3(box2HalfSize.x, box2HalfSize.y, box2HalfSize.z);

		if (box2Tr->GetAxis(0).Dot(contact->normal) < 0)
			contactPoint2.x *= -1.0f;
		if (box2Tr->GetAxis(1).Dot(contact->normal) < 0)
			contactPoint2.y *= -1.0f;
		if (box2Tr->GetAxis(2).Dot(contact->normal) < 0)
			contactPoint2.z *= -1.0f;

		/* 월드 좌표로 변환한다 */
		contactPoint2 = Vec3::Transform(contactPoint2, box2Tr->GetWorldMatrix());
		contactPoint1 = Vec3(contactPoint2 - contact->normal * contact->penetration);
	}
	else // 충돌면이 box2 의 면일 때
	{
		contactPoint1 = Vec3(box1HalfSize.x, box1HalfSize.y, box1HalfSize.z);

		if (box1Tr->GetAxis(0).Dot(contact->normal) > 0)
			contactPoint1.x *= -1.0f;
		if (box1Tr->GetAxis(1).Dot(contact->normal) > 0)
			contactPoint1.y *= -1.0f;
		if (box1Tr->GetAxis(2).Dot(contact->normal) > 0)
			contactPoint1.z *= -1.0f;

		/* 월드 좌표로 변환한다 */
		contactPoint1 = Vec3::Transform(contactPoint1, box1Tr->GetWorldMatrix());
		contactPoint2 = Vec3(contactPoint1 - contact->normal * contact->penetration);
	}

	contact->contactPoint[0] = contactPoint1;
	contact->contactPoint[1] = contactPoint2;
}




void CollisionDetector::calcContactPointOnLine(
	BoxCollider* box1,
	BoxCollider* box2,
	int minAxisIdx,
	Contact* contact
)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform();
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	Vec3 box1HalfSize = box1->GetSize() / 2;
	Vec3 box2HalfSize = box1->GetSize() / 2;

	/* 접촉한 변 위의 정점을 찾는다 */
	Vector3 vertexOne(box1HalfSize.x, box1HalfSize.y, box1HalfSize.z);
	Vector3 vertexTwo(box2HalfSize.x, box2HalfSize.y, box2HalfSize.z);

	if (box1Tr->GetAxis(0).Dot(contact->normal) > 0)
		vertexOne.x *= -1.0f;
	if (box1Tr->GetAxis(1).Dot(contact->normal) > 0)
		vertexOne.y *= -1.0f; 
	if (box1Tr->GetAxis(2).Dot(contact->normal) > 0)
		vertexOne.z *= -1.0f;
							  
	if (box2Tr->GetAxis(0).Dot(contact->normal) < 0) 
		vertexTwo.x *= -1.0f;
	if (box2Tr->GetAxis(1).Dot(contact->normal) < 0) 
		vertexTwo.y *= -1.0f;
	if (box2Tr->GetAxis(2).Dot(contact->normal) < 0) 
		vertexTwo.z *= -1.0f;

	/* 변의 방향을 찾는다 */
	Vector3 directionOne, directionTwo;

	switch (minAxisIdx)
	{
	case 6: // box1 의 x 축 X box2 의 x 축
		directionOne = box1Tr->GetAxis(0);
		if (vertexOne.x > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(0);
		if (vertexTwo.x > 0) directionTwo *= -1.0f;
		break;

	case 7: // box1 의 x 축 X box2 의 y 축
		directionOne = box1Tr->GetAxis(0);
		if (vertexOne.x > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(1);
		if (vertexTwo.y > 0) directionTwo *= -1.0f;
		break;

	case 8: // box1 의 x 축 X box2 의 z 축
		directionOne = box1Tr->GetAxis(0);
		if (vertexOne.x > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(2);
		if (vertexTwo.z > 0) directionTwo *= -1.0f;
		break;

	case 9: // box1 의 y 축 X box2 의 x 축
		directionOne = box1Tr->GetAxis(1);
		if (vertexOne.y > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(0);
		if (vertexTwo.x > 0) directionTwo *= -1.0f;
		break;

	case 10: // box1 의 y 축 X box2 의 y 축
		directionOne = box1Tr->GetAxis(1);
		if (vertexOne.y > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(1);
		if (vertexTwo.y > 0) directionTwo *= -1.0f;
		break;

	case 11: // box1 의 y 축 X box2 의 z 축
		directionOne = box1Tr->GetAxis(1);
		if (vertexOne.y > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(2);
		if (vertexTwo.z > 0) directionTwo *= -1.0f;
		break;

	case 12: // box1 의 z 축 X box2 의 x 축
		directionOne = box1Tr->GetAxis(2);
		if (vertexOne.z > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(0);
		if (vertexTwo.x > 0) directionTwo *= -1.0f;
		break;

	case 13: // box1 의 z 축 X box2 의 y 축
		directionOne = box1Tr->GetAxis(2);
		if (vertexOne.z > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(1);
		if (vertexTwo.y > 0) directionTwo *= -1.0f;
		break;

	case 14: // box1 의 z 축 X box2 의 z 축
		directionOne = box1Tr->GetAxis(2);
		if (vertexOne.z > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(2);
		if (vertexTwo.z > 0) directionTwo *= -1.0f;
		break;

	default:
		break;
	}

	/* 정점을 월드 좌표계로 변환한다 */
	vertexOne = Vec3::Transform(vertexOne, box1Tr->GetWorldMatrix());
	vertexTwo = Vec3::Transform(vertexTwo, box2Tr->GetWorldMatrix());

	/* box2 의 변과 가장 가까운 box1 위의 점을 찾는다 */
	float k = directionOne.Dot(directionTwo);
	Vec3 closestPointOne = Vec3(vertexOne + directionOne * ((vertexTwo - vertexOne).Dot(directionOne - directionTwo * k) / (1 - k * k)));

	/* box1 의 변과 가장 가까운 box2 위의 점을 찾는다 */
	Vec3 closestPointTwo = Vec3(vertexTwo + directionTwo * ((closestPointOne - vertexTwo).Dot(directionTwo)));

	contact->contactPoint[0] = closestPointOne; 
	contact->contactPoint[1] = closestPointTwo; 
}