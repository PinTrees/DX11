#pragma once

class Scene;

class SceneManager
{
	SINGLE_HEADER(SceneManager)

private:
	Scene* m_pCurrScene;

public:
	void Init();

	void LoadScene(string sceneName);

	void UpdateScene();
	void RenderScene();

	Scene* GetCurrentScene() { return m_pCurrScene; }

public:
	void HandleSaveScene();

private:
	void CreateScene();
};

