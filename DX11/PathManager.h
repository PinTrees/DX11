#pragma once

class PathManager
{
	SINGLE_HEADER(PathManager)
private:
	wchar_t _szContentPath[MAX_PATH]; // Project Path
public:
	void Init();

	const wchar_t* GetContentPath() { return _szContentPath; }
	wstring GetContentWPath();
	string GetContentSPath();

	wstring GetMovePath(wstring movePath);
	wstring GetMovePath(string movePath);
	string GetMoveSPath(wstring movePath);
	string GetMoveSPath(string movePath);

	wstring GetCutSolutionPath(wstring path);
	string GetCutSolutionPath(string path);
};

#define PATH_SOLUTION PathManager::GetI()->GetContentPath();