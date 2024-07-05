#pragma once

class Scene;

class SceneManager
{
	SINGLE_HEADER(SceneManager)

private:
	Scene* m_pCurrScene;
	std::map<wstring, Scene*> m_Scenes;

public:
	void Init();

	void LoadScene(wstring scenePath);

	void UpdateScene();
	void RenderScene();

	Scene* GetCurrentScene() { return m_pCurrScene; }

public:
	void HandleSaveScene();

private:
	void CreateScene();
};

