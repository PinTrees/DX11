#include "pch.h"
#include "PathManager.h"

SINGLE_BODY(PathManager)

PathManager::PathManager()
{

}

PathManager::~PathManager()
{

}


void PathManager::Init()
{
	GetCurrentDirectory(MAX_PATH, _szContentPath); // 현재 실행파일의 경로 저장
	
	// 경로 이동을 하는 이유는 게임폴더를 변경,
	// visual studio 디버그 모드로 실행 시 경로 이상이 없게하기 위해
	int ilen = wcslen(_szContentPath);

	for (int i = ilen - 1; i >= 0; i--) // 상위폴더 이동
	{
		if ('\\' == _szContentPath[i])
		{
			_szContentPath[i] = '\0';
			break;
		}
	}
}

wstring PathManager::GetMovePath(wstring movePath)
{
	size_t len = wcslen(_szContentPath);

	//_szContentPath[len] = '\\';


	wstring path = _szContentPath;

	// Ensure the base path ends with a backslash
	if (!path.empty() && path.back() != L'\\') {
		path.append(L"\\");
	}

	// Append the move path
	path.append(movePath);
	return path;
}
