#pragma once
#include <atomic>

class MonoBehaviour;
class Component;

class GameObject
{
private:
	uint64 m_InstanceID;	// 고유 인스턴스 ID
	static atomic<uint64> g_NextInstanceID;

	string m_Name;

	uint8 m_LayerIndex;
	vector<Component*> m_Components;
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
		shared_ptr<T> component = make_shared<T>();
		Component* baseComponent = (Component*)(component.get());

		baseComponent->gameObject = this;
		m_Components.push_back(static_pointer_cast<Component>(component));

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

private:
	friend class GameObject;
	void SetChild(GameObject* child);
};

