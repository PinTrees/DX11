#pragma once
#include "EditorGUIStyle.h"

struct FontLoadContainer
{
	string FontKey;
	EditorTextStyle FontStyle;
};

class EditorGUIResourceManager
{
	SINGLE_HEADER(EditorGUIResourceManager)

public:
	// �ε�� ��Ʈ���� ������ �� (��Ʈ �̸� -> ImFont* ����)
	unordered_map<string, ImFont*> m_FontMap;
	unordered_map<string, bool> m_LoadFontTask;

	vector<FontLoadContainer> loadedAA;

public:
	void Init();

	ImFont* LoadFont(EditorTextStyle style);

private: 
	void LoadFontAsync(FontLoadContainer container);
};

