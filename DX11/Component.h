#pragma once

class GameObject;

class Component
{
private:
	GameObject* m_pGameObject;

public:
	Component();
	virtual ~Component();

	virtual void Awake() { }
	virtual void Start() { }	
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void FixedUpdate() { }

public:
	GameObject* GetGameObject() { return m_pGameObject; }

private:
	friend class GameObject;
	void SetGameObject(GameObject* gameObject) { m_pGameObject = gameObject; }
};

