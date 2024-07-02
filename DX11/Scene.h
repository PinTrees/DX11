#pragma once

class GameObject;

class Scene
{
private:
	vector<GameObject*> m_arrGameObjects[(UINT)32];		// 태그 기반 배열
	vector<GameObject*> m_RootGameObjects;

public:
	Scene();
	~Scene();

public:
	void Enter();
	void Exit();

	void UpdateScene();
	void RenderScene();
	void RenderSceneShadow();
	void RenderSceneShadowNormal();

public:
	void AddRootGameObject(GameObject* gameObject);
	vector<GameObject*> GetRootGameObjects() { return m_RootGameObjects; }
	vector<GameObject*> GetAllGameObjects() const { return m_arrGameObjects[0]; }
};

