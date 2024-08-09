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

wstring PathManager::GetContentWPath()
{
	wstring path = _szContentPath;

	return path;
}

string PathManager::GetContentSPath()
{
	string path = wstring_to_string(_szContentPath);

	return path;
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

wstring PathManager::GetMovePath(string movePath)
{
	wstring path = _szContentPath;
	path += string_to_wstring(movePath);

	// �ִ� ���� ��� ���� �˻�
	if (path.size() > MAX_PATH)
		assert(false);

	return path;
}

string PathManager::GetMoveSPath(wstring movePath)
{
	wstring path = _szContentPath;
	path += movePath;

	string result = wstring_to_string(path);

	// �ִ� ���� ��� ���� �˻�
	if (result.size() > MAX_PATH)
		assert(false);

	return result;
}

string PathManager::GetMoveSPath(string movePath)
{
	wstring path = _szContentPath;
	path += string_to_wstring(movePath);

	string result = wstring_to_string(path);

	// �ִ� ���� ��� ���� �˻�
	if (result.size() > MAX_PATH)
		assert(false);

	return result;
}

wstring PathManager::GetCutSolutionPath(wstring path)
{
	wstring result = path;
	wstring soluPath = GetContentWPath();
	size_t pos = result.find(soluPath);
	result.erase(pos, soluPath.length());

	return result;
}

string PathManager::GetCutSolutionPath(string path)
{
	string result = path;
	string soluPath = GetContentSPath();
	size_t pos = result.find(soluPath);
	result.erase(pos, soluPath.length());

	return result;
}
