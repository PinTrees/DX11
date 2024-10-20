#pragma once

class EditorDialog; 
class EditorWindow;

class EditorGUIManager
{
	SINGLE_HEADER(EditorGUIManager)

private:
	vector<EditorWindow*> m_pEditorWindows;
	vector<EditorDialog*> m_pEditorDialogs;

public:
	void Init();
	void Update();
	void RenderEditorWindows();

public:
	void RegisterWindow(EditorWindow* window);
	void RegisterEditorDialog(EditorDialog* dialog);
	void RemoveEditorDialog(EditorDialog* dialog);
};

 