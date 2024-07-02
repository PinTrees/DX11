#pragma once


class EditorWindow
{
private:
	string windowName;
	bool isDocked;
	ImVec2 dockedPos;

public:
	EditorWindow(const std::string& name);
	~EditorWindow();

	void Render();

protected:
	virtual void OnRender() {}
	virtual void OnRenderExit() {}
};

