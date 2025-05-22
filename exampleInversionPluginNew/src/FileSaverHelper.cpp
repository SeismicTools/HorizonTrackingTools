#include "FileSaverHelper.h"

//namespace fs = std::filesystem;

void FileSaverHelper::SaveTraceToCsv(
    const int numOfTraces, 
    const std::vector<double>& signalsOfSeveralTraces, 
    const int numOfSignalsAtOneTrace, 
    const std::string& folderPath
)
{
    for (int i = 0; i < numOfTraces; i++)
    {
        std::string fileNameForTrace = folderPath + std::string("trace_") + std::to_string(i) + std::string(".csv");
        std::ofstream file(fileNameForTrace.c_str());

        if (file.is_open()) {
            file << "amp\n";

            for (size_t j = 0; j < numOfSignalsAtOneTrace; ++j) {
                file << signalsOfSeveralTraces[i * numOfSignalsAtOneTrace + j] << "\n";
            }

            file.close();
        }
        else {
            std::cerr << "Ошибка открытия файла для записи" << std::endl;
        }
    }
}

void FileSaverHelper::deleteAllFilesInDirectory(const std::string& folderPath)
{
    //for (const auto& entry : fs::directory_iterator(folderPath)) {
    //    if (fs::is_regular_file(entry.path())) {
    //        fs::remove(entry.path());
    //        std::cout << "Deleted file: " << entry.path() << std::endl;
    //    }
    //}
};
