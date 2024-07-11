#pragma once
#include "Component.h"

class Transform : public Component
{
	using Super = Component;

private:
	shared_ptr<Transform> _parent;
	vector<shared_ptr<Transform>> _children;
	
	Vec3 _localScale = { 1.f, 1.f, 1.f };
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localPosition = { 0.f, 0.f, 0.f };

	// Cache
	Matrix _matLocal = Matrix::Identity;
	Matrix m_WorldMatrix = Matrix::Identity;

	Vec3 _scale;
	Vec3 _rotation;
	Vec3 _position;

	Quaternion m_Rotation;

public:
	Transform();
	~Transform();

	virtual void Awake() override;
	virtual void Update() override;

	void UpdateTransform();

	static Vec3 ToEulerAngles(Quaternion q);

	// Local
	Vec3 GetLocalScale() { return _localScale; }
	void SetLocalScale(const Vec3& localScale) { _localScale = localScale; UpdateTransform(); }
	Vec3 GetLocalRotation() { return _localRotation; }
	void SetLocalRotation(const Vec3& localRotation) { _localRotation = localRotation; UpdateTransform(); }
	Vec3 GetLocalPosition() { return _localPosition; }
	void SetLocalPosition(const Vec3& localPosition) { _localPosition = localPosition; UpdateTransform(); }

	// World
	Vec3 GetScale() { return _scale; }
	void SetScale(const Vec3& scale);
	Vec3 GetRotation() { return _rotation; }
	void SetRotation(const Vec3& rotation);
	Quaternion GetRotationQ() { return m_Rotation; }
	void SetRotationQ(Quaternion q);
	Vec3 GetPosition() { return _position; }
	void SetPosition(const Vec3& position);
	void SetPosition(float x, float y, float z) { SetPosition(Vec3(x, y, z)); }  

	void Translate(const Vec3& position) { SetPosition(GetPosition() + position); }

	Vec3 GetAxis(int index) const;
	Vec3 GetRight() { return m_WorldMatrix.Right(); }
	Vec3 GetUp() { return m_WorldMatrix.Up(); }
	Vec3 GetLook() { return m_WorldMatrix.Backward(); }
	Vec3 GetForward() { return m_WorldMatrix.Forward(); } 
	Matrix GetWorldMatrix() { return m_WorldMatrix; }

	// °èÃþ °ü°è
	bool HasParent() { return _parent != nullptr; }

	shared_ptr<Transform> GetParent() { return _parent; }
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }

	const vector<shared_ptr<Transform>>& GetChildren() { return _children; }
	void AddChild(shared_ptr<Transform> child) { _children.push_back(child); }

public:
	virtual void OnInspectorGUI() override;

public:
	GENERATE_COMPONENT_BODY(Transform)
};

REGISTER_COMPONENT(Transform)