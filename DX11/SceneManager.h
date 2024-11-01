#pragma once

class Scene;

class SceneManager
{
	SINGLE_HEADER(SceneManager)

private:
	Scene* m_pCurrScene;
	std::map<wstring, Scene*> m_Scenes;

	// Editor
	vector<function<void()>>		m_Editor_LastUpdateActions;

public:
	void Init();

	void LoadScene(wstring scenePath);

	void UpdateScene();
	void RenderScene();
	void LastUpdate(); 

	Scene* GetCurrentScene() { return m_pCurrScene; }

public:
	void AddLastUpdate(function<void()> action) { m_Editor_LastUpdateActions.push_back(action); } 
	// Editor
	void HandleSaveScene();
	void HandlePlay();
	void HandleStop();

private:
	void CreateScene();
};

