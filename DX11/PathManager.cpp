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

/// <summary>
/// ���� ������Ʈ ��� + ������ ���
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
