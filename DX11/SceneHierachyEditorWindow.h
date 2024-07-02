#pragma once
#include "EditorWindow.h"

class SceneHierachyEditorWindow
	: public EditorWindow
{
public:
	SceneHierachyEditorWindow();
	~SceneHierachyEditorWindow();

protected:
	virtual void OnRender() override;
	virtual void OnRenderExit() override;

private:
	void DrawGameObject(GameObject* gameObject);

	// Handle Drag an Drop
	void HandleFbxFileDrop(const std::string& filePath, GameObject* parent);
};

