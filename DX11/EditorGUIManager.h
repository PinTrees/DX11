#pragma once

class EditorGUIManager
{
	SINGLE_HEADER(EditorGUIManager)

private:
	vector<EditorWindow*> m_pEditorWindows;

public:
	void Init();
	void Update();
	void RenderEditorWindows();

public:
	void RegisterWindow(EditorWindow* window);
};

