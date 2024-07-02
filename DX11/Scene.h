#pragma once

class GameObject;

class Scene
{
private:
	vector<GameObject*> m_arrGameObjects[(UINT)32];		// �±� ��� �迭
	vector<GameObject*> m_RootGameObjects;

public:
	Scene();
	~Scene();

public:
	void Enter();
	void Exit();

	void RenderScene();
	void UpdateScene();

public:
	void AddRootGameObject(GameObject* gameObject);
	vector<GameObject*> GetRootGameObjects() { return m_RootGameObjects; }
};

