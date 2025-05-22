#include "PathHelper.h"
#include <Windows.h>

std::string PathHelper::GetFullPathRelativeStartUpDirectoryOfApplication(const std::string pathToAddToStartUpDirectory)
{
	CHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	std::string concat = std::string(path) + pathToAddToStartUpDirectory;
	return concat;
}
