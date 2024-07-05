#pragma once
#include <atomic>
#include <nlohmann/json.hpp>
#include "Component.h"

using json = nlohmann::json;
using std::make_shared;
using std::static_pointer_cast;
	
class MonoBehaviour;

class GameObject
{
private:
	uint64 m_InstanceID;	// 고유 인스턴스 ID
	static atomic<uint64> g_NextInstanceID;

	string m_Name;

	uint8 m_LayerIndex;
	vector<shared_ptr<Component>> m_Components;
	vector<MonoBehaviour*> m_Scripts;

	GameObject* m_pParentGameObject;
	vector<GameObject*> m_pChildGameObjects;

public:
	GameObject();
	GameObject(const string& name);
	~GameObject();

public:
	uint64 GetInstanceID() const { return m_InstanceID; }
	const string& GetName() { return m_Name; }
	vector<GameObject*> GetChildren() { return m_pChildGameObjects; }
	
	void SetParent(GameObject* parent);

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();

	void SetLayerIndex(uint8 layer) { m_LayerIndex = layer; }
	uint8 GetLayerIndex() { return m_LayerIndex; }

	template <class T>
	T* AddComponent()
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		Component* baseComponent = static_cast<Component*>(component.get());

		baseComponent->SetGameObject(this);
		m_Components.push_back(component);

		return component.get();
	}
	template <class T>
	T* GetComponent()
	{
		for (auto& component : m_Components)
		{
			std::shared_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(component);
			if (castedComponent)
			{
				return castedComponent.get();
			}
		}
		return nullptr;
	}
	vector<shared_ptr<Component>> GetComponents()
	{
		return m_Components;
	}

private:
	friend class GameObject;
	void SetChild(GameObject* child);

public:
	json toJson() const 
	{
		json j;
		j["name"] = m_Name;
		j["components"] = json::array();
		for (const shared_ptr<Component> component : m_Components) 
			j["components"].push_back(component->toJson());
		
		return j;
	}

	void fromJson(const json& j)
	{
		m_Name = j.at("name").get<string>();
		for (const auto& compJson : j.at("components"))
		{
			string type = compJson.at("type").get<string>();
			shared_ptr<Component> component = nullptr;
			if (type == "Transform")
			{
				//component = make_shared<Transform>();
			}

			// 다른 컴포넌트 타입도 여기에 추가
			component->fromJson(compJson);
			m_Components.push_back(component);
		}
	}
};

