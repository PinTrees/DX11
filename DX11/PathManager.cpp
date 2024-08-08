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

/// <summary>
/// 현재 프로젝트 경로 + 가야할 경로
/// </summary>
/// <param name="movePath"></param>
/// <returns></returns>
const wchar_t* PathManager::GetMovePath(wstring movePath)
{
	size_t len1 = std::wcslen(_szContentPath);
	size_t len2 = movePath.size();
	
	if (MAX_PATH > (len1 + len2 + 2))
		return nullptr;

	wchar_t* result = new wchar_t[len1 + len2 + 2];

	//std::wcsncpy(result, _szContentPath, len1);

	//std::wcsncpy(result + len1, movePath, MAX_PATH - len1);

	result[len1 + len2] = L'\0';

	return nullptr;

}
