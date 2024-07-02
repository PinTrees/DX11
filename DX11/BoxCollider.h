#pragma once
#include "Collider.h"

class BoxCollider
	: public Collider
{
private:
	Vec3 m_Size;

public:
	BoxCollider();
	~BoxCollider();

public:
	void SetSize(Vec3 size) { m_Size = size; }
	Vec3 GetSize() const { return m_Size; }

public:
	virtual void Render() override;
	virtual void OnInspectorGUI() override;
};

