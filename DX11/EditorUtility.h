#pragma once

class EditorUtility
{
public:
	static std::wstring OpenFileDialog();
	static std::wstring OpenFileDialog(const std::string& initialPath);
};

