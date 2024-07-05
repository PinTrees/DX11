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

	void LastFramUpdate();

public:
	static Scene* Load(wstring scenePath);
	static void Save(Scene* scene);
	static void SaveNewScene(Scene* scene);

public:
	void AddRootGameObject(GameObject* gameObject);
	vector<GameObject*> GetRootGameObjects() { return m_RootGameObjects; }
	vector<GameObject*> GetAllGameObjects() const { return m_arrGameObjects[0]; }

public:
	friend void from_json(const json& j, Scene& scene);
	friend void to_json(json& j, const Scene& scene);
};

