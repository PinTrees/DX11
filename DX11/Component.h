#pragma once
#include <nlohmann/json.hpp>
#include "JsonUtility.h"

using json = nlohmann::json;

class GameObject;

class Component
{
protected:
	GameObject* m_pGameObject;
	string m_InspectorTitleName = "";
	int m_componentID;
public:
	Component();
	virtual ~Component();

	virtual void Awake()  { }
	virtual void Start()  { }	
	virtual void Update() { }
	virtual void LateUpdate()  { }
	virtual void FixedUpdate() { }
	virtual void Render() { }

	// Editor Only
	void RenderInspectorGUI();
	virtual void OnInspectorGUI() { }
	virtual void OnDrawGizmos() { }
	virtual void OnDestroy() { }
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

