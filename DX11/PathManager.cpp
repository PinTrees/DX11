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

	wcscat_s(_szContentPath, MAX_PATH, L"\\");
}

wstring PathManager::GetContentPathW()
{
	wstring path = _szContentPath;

	return path;
}

string PathManager::GetContentPathS()
{
	string path = wstring_to_string(_szContentPath);

	return path;
}

wstring PathManager::GetMovePathW(wstring movePath)
{
	wstring path = _szContentPath;
	path += movePath;

	// 최대 폴더 경로 제한 검사
	if (path.size() > MAX_PATH)
		assert(false);

	return path;
}

string PathManager::GetMovePathS(string movePath)
{
	wstring path = _szContentPath;
	path += string_to_wstring(movePath);

	string result = wstring_to_string(path);

	// 최대 폴더 경로 제한 검사
	if (result.size() > MAX_PATH)
		assert(false);

	return result;
}

wstring PathManager::GetCutSolutionPath(wstring path)
{
	wstring result = path;
	wstring soluPath = GetContentPathW();
	size_t pos = result.find(soluPath);
	result.erase(pos, soluPath.length());

	return result;
}

string PathManager::GetCutSolutionPath(string path)
{
	string result = path;
	string soluPath = GetContentPathS();
	size_t pos = result.find(soluPath);
	result.erase(pos, soluPath.length());

	return result;
}
