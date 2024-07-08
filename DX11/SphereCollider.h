#pragma once
#include "Collider.h"

class SphereCollider
	: public Collider
{
private:
	float m_Radius;

public:
	SphereCollider();
	~SphereCollider();

public:
	virtual void OnDrawGizmos() override;
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(SphereCollider)
};

REGISTER_COMPONENT(SphereCollider)

