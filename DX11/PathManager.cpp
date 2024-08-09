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

	wcscat_s(_szContentPath, MAX_PATH, L"\\");
}

wstring PathManager::GetMovePath(wstring movePath)
{
	wstring path = _szContentPath;
	path += movePath;

	// �ִ� ���� ��� ���� �˻�
	if (path.size() > MAX_PATH)
		assert(false);

	return path;
}
