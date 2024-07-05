#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameObject;

class Scene
{
private:
	wstring m_ScenePath;

	vector<GameObject*> m_arrGameObjects[(UINT)32];		// 태그 기반 배열
	vector<GameObject*> m_RootGameObjects;

public:
	Scene();
	~Scene();

public:
	wstring GetScenePath() const { return m_ScenePath; }

public:
	void Enter();
	void Exit();

	void UpdateScene();
	void RenderScene();
	void RenderSceneShadow();
	void RenderSceneShadowNormal();

public:
	static void Load(string scenePath);
	static void Save(Scene* scene);
	static void SaveNewScene(Scene* scene);

public:
	void AddRootGameObject(GameObject* gameObject);
	vector<GameObject*> GetRootGameObjects() { return m_RootGameObjects; }
	vector<GameObject*> GetAllGameObjects() const { return m_arrGameObjects[0]; }

public:
	json toJson() const
	{
		json j;
		j["rootGameObjects"] = json::array();
		for (const GameObject* gameObject : m_RootGameObjects)
			j["rootGameObjects"].push_back(gameObject->toJson());
		
		return j;
	}

	void fromJson(const json& j)
	{
		for (const auto& gameObjectJson : j.at("rootGameObjects"))
		{
			GameObject* gameObject = new GameObject();
			gameObject->fromJson(gameObjectJson);
			m_RootGameObjects.push_back(gameObject);
		}
	}
};

