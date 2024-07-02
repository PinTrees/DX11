#pragma once
#include "EditorWindow.h"

class ProjectEditorWindow
	: public EditorWindow
{
public:
	ProjectEditorWindow();
	~ProjectEditorWindow();

protected:
	virtual void OnRender() override;

private:
	void DisplayDirectoryTree(const std::string& directory);
	void DisplayDirectoryContents(const std::string& directory);

	std::string GetSolutionDirectory();

	std::string solutionDirectory;
	std::string currentDirectory;
};

