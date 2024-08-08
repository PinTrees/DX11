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
	GetCurrentDirectory(MAX_PATH, _szContentPath); // ���� ���������� ��� ����
	
	// ��� �̵��� �ϴ� ������ ���������� ����,
	// visual studio ����� ���� ���� �� ��� �̻��� �����ϱ� ����
	int ilen = wcslen(_szContentPath);

	for (int i = ilen - 1; i >= 0; i--) // �������� �̵�
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
