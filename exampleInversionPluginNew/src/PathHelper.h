#pragma once

#include <string>

class PathHelper
{
	public:
		static std::string GetFullPathRelativeStartUpDirectoryOfApplication(
			const std::string pathToAddToStartUpDirectory);
};