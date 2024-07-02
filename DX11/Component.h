#pragma once

class GameObject;

class Component
{
protected:
	GameObject* m_pGameObject;

public:
	Component();
	virtual ~Component();

	virtual void Awake() { }
	virtual void Start() { }	
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void FixedUpdate() { }
	virtual void Render() {}

	virtual void OnInspectorGUI() { }

public:
	GameObject* GetGameObject() { return m_pGameObject; }

private:
	friend class GameObject;
	void SetGameObject(GameObject* gameObject) { m_pGameObject = gameObject; }
};

