#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>   

class FileSaverHelper
{
    public:
	    static void SaveTraceToCsv(
            const int numOfTraces,
            const std::vector<double>& signalsOfSeveralTraces,
            const int numOfSignalsAtOneTrace,
            const std::string& folderPath
        );

        static void deleteAllFilesInDirectory(const std::string& folderPath);
};
