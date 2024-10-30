#pragma once


class EditorWindow
{
private:
	string m_WindowTitleName;
	string m_Icon; 

	bool m_IsDocked; 
	ImVec2 dockedPos; 

public:
	EditorWindow(const string& name, const string& icon="");  
	~EditorWindow();

	void Render();
	virtual void Update() {}

protected:
	virtual void PushStyle() {}
	virtual void OnRender() {}
	virtual void PopStyle() {}
};

