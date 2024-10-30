#pragma once

class EditorDialog
{
private:
	string m_WindowName;

	bool m_WindowOpen = true;			// â ���� ���� ����
	bool m_PreviousWindowOpen = true;	// ���� â ���� ���� ����

public:
	EditorDialog(const std::string& name); 
	~EditorDialog();

	void Render();
	virtual void Show();
	virtual void Close();

	virtual void Update() {}

protected:
	virtual void OnRender() {}

private:
	void PushStyle();
	void PopStyle();

	void PushStyle_TitleBar();
	void PopStyle_TitleBar();

protected:
	void PushStyle_Body(); 
	void PopStyle_Body(); 
};

