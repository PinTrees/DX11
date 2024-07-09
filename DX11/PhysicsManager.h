#pragma once

class GameObject;

union COLLIDER_ID
{
	struct
	{
		UINT Left_id;
		UINT RIght_id;
	};

	ULONGLONG ID;
};

struct Contact
{
	RigidBody* bodies[2];		// Pointers to the involved rigid bodies
	Vec3 normal;				// Normal at the point of contact
	Vec3 contactPoint[2];		// Contact points on each body
	float penetration;			// Depth of penetration
	float restitution;			// Restitution coefficient
	float friction;				// Friction coefficient
	float normalImpulseSum;		// Accumulated normal impulse
	float tangentImpulseSum1;	// Accumulated tangential impulse for the first tangent
	float tangentImpulseSum2;	// Accumulated tangential impulse for the second tangent
};

class PhysicsManager
{
	SINGLE_HEADER(PhysicsManager)

private:
	float m_GravityAcceleration;
	map<ULONGLONG, bool>	mMapColInfo;	// 충돌체 간의 이전 프레임 충돌 정보 
	vector<Contact*> m_Contacts;

public:
	void SetGravity(float gravity) { m_GravityAcceleration = gravity; }
	float GetGravity() const { return m_GravityAcceleration; }

public:
	void Init();
	void Update(float deltaTime);

private:
	void UpdateCollision();

	bool CheckSphereSphereCollision(SphereCollider* sphere1, SphereCollider* sphere2, Contact& contact);
	bool CheckSphereBoxCollision(SphereCollider* sphere, BoxCollider* box, Contact& contact);
	bool CheckBoxBoxCollision(BoxCollider* box1, BoxCollider* box2, Contact& contact);
	void HandleCollision(GameObject* obj1, GameObject* obj2, COLLIDER_ID colliderID);

	void ResolveCollision(GameObject* obj1, GameObject* obj2, const Contact& contact);

	void ResolveCollision(std::vector<Contact*>& contacts, float deltaTime);
	void sequentialImpulse(Contact* contact, float deltaTime);
};

