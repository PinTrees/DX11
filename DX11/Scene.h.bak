#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameObject;
class Component;

class Scene
{
private:
	wstring m_ScenePath;

	vector<GameObject*> m_arrGameObjects[(UINT)32];		// 태그 기반 배열 - 충돌 처리 최적화
	
	vector<GameObject*> m_vecAllGameObjects;			// 미 구현
	vector<GameObject*> m_vecRootGameObjects;

	vector<GameObject*> m_cullingGameObjects;

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

	// Editor Only
	void _Editor_RenderScene();
	void _Editor_RenderSceneShadowNormal();

	void RenderSceneGizmos();
	void LastFramUpdate();

public:
	static Scene* Load(wstring scenePath);
	static void Save(Scene* scene);
	static void SaveNewScene(Scene* scene);

public:
	void DestroyComponent(Component* component);  
	void DestroyGameObject(GameObject* gameobject);
	 
	void AddRootGameObject(GameObject* gameObject);
	vector<GameObject*> GetRootGameObjects() { return m_vecRootGameObjects; }
	vector<GameObject*> GetAllGameObjects() const { return m_arrGameObjects[0]; }
	vector<GameObject*>& GetCullingGameObjects() { return m_cullingGameObjects; }

	void RemoveRootGameObjects(GameObject* gameObject);

public:
	friend void from_json(const json& j, Scene& scene);
	friend void to_json(json& j, const Scene& scene);
};

