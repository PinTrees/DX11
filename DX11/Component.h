#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameObject;

class Component
{
protected:
	GameObject* m_pGameObject;

public:
	Component();
	virtual ~Component();

	virtual void Awake()  { }
	virtual void Start()  { }	
	virtual void Update() { }
	virtual void LateUpdate()  { }
	virtual void FixedUpdate() { }
	virtual void Render() { }

	virtual void OnInspectorGUI() { }

public:
	GameObject* GetGameObject() { return m_pGameObject; }

private:
	friend class GameObject;
	void SetGameObject(GameObject* gameObject) { m_pGameObject = gameObject; }

public:
	virtual json toJson() const = 0;
	virtual void fromJson(const json& j) = 0;
	virtual std::string GetType() const = 0;
};

