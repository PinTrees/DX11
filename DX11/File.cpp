#include "pch.h"
#include "File.h"

string File::GetExtension(const std::string& fullPath)
{
	size_t dotPos = fullPath.find_last_of(L'.');
	if (dotPos != std::string::npos)
	{
		return fullPath.substr(dotPos + 1);
	}
	return "";
}
