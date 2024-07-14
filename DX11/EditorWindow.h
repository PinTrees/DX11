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
	virtual void Update() {}

protected:
	virtual void PushStyle() {}
	virtual void OnRenderBegin() {}
	virtual void OnRender() {}
	virtual void OnRenderExit() {}
	virtual void PopStyle() {}
};

