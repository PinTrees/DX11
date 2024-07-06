#pragma once
#include "EditorWindow.h"

class GameViewEditorWindow
	: public EditorWindow
{
public:
	GameViewEditorWindow();
	~GameViewEditorWindow();

public:
	virtual void OnRender() override;
};

