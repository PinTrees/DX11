#pragma once

class PathManager
{
	SINGLE_HEADER(PathManager)
private:
	wchar_t _szContentPath[MAX_PATH]; // Project Path
public:
	void Init();

	const wchar_t* GetContentPath() { return _szContentPath; }
	wstring GetMovePath(wstring movePath);
};

