#include "test_utils.h"
#include <list>
#include "fotiadi_math.h"
#include "rapidcsv.h"

void fix_zero(std::vector<double>& data) {
    for (int32_t i = 0; i < data.size(); i++) {
        if (std::abs(data[i]) < 1e-9) {
            data[i] = NAN;
        }
    }
}

int ParseHor(std::string filePath, std::vector<Hor>& hors)
{
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла." << std::endl;
        return 1;
    }

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Hor hor;
        iss >> hor.id >> hor.value;
        hors.push_back(hor);
    }

    file.close();

    return FM_ERROR_NO;
}

/*
* Читает файл с данными вида 2D_test_autocorr.txt
*/
int ParseData(
    std::string filePath,
    int32_t maxTraces,
    std::list<Trace>& traces //!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
)
{
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла." << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string bufferStr(size, ' ');
    file.read(&bufferStr[0], size);
    std::istringstream buffer(bufferStr);

    std::string line;
    std::string traceHeader = "-> Trace #";

    int numOfTraces = 0;

    while ((line.size() >= 1 && line[0] == '-') // если уже считали хедер или можно читать файл дальше
        || std::getline(buffer, line)) {
        if (line[0] == '-') {
            Trace trace;
            numOfTraces++;
            if(maxTraces != -1 && numOfTraces > maxTraces)
            {
                break;
            }

            auto substr = line.substr(traceHeader.size());
            std::istringstream iss(substr);
            iss >> trace.id;
            int timePrev = -1;
            while (std::getline(buffer, line)
                && !line.empty()
                && line[0] != '-') {
                std::istringstream iss(line);
                TraceData data;
                iss >> data.time >> data.value;
                if(data.time == timePrev)
                {
                    break;
                }
                timePrev = data.time;
                trace.values.push_back(data);
            }

            traces.push_back(trace);
        }
    }

    file.close();
    return FM_ERROR_NO;
}

int GetTimeAndAmp(Trace trace, std::vector<double>& times, std::vector<double>& amps)
{
    size_t id = 0;
    for (TraceData var : trace.values)
    {
        times[id] = var.time;
        amps[id] = var.value;
        id++;
    }
    return FM_ERROR_NO;
}

int32_t GetTimeAndAmpFromCsv(
    std::string namefile,
    std::vector<double>& times,     // [out] время
    std::vector<double>& amps       // [out] амплитуды
)
{
    rapidcsv::Document doc_ex(
        namefile,
        rapidcsv::LabelParams(),
        rapidcsv::SeparatorParams(';')
    );
    std::vector<double> time_ex = doc_ex.GetColumn<double>("time");
    times.clear();
    times.insert(times.end(), time_ex.begin(), time_ex.end());

    std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
    amps.clear();
    amps.insert(amps.end(), amp_ex.begin(), amp_ex.end());

    return FM_ERROR_NO;
}

template<typename T>
int findClosestIndex(const std::vector<T>& arr, double target) {
    int left = 0;
    int right = arr.size() - 1;
    int closestIndex = -1;
    int minDiff = INT_MAX;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        double diff = std::abs(arr[mid] - target);

        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = mid;
        }

        if (arr[mid] == target) {
            return mid;
        }
        else if (arr[mid] < target) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return closestIndex;
}

template<typename T>
int32_t LoadSeismic3DTemplate(
    const std::vector<std::pair<int32_t, std::string>>& names_files_trace,
    const std::vector<std::string>& names_files_borders,
    std::vector<T>& time,
    int32_t& n,
    int32_t& m,
    std::vector<T>& signals,
    int32_t& num_borders,
    std::vector<int16_t>& borders,
    const int32_t ntrace
) {
    signals.resize(0);
    std::unordered_map<int32_t, int32_t> traceIdAtFileToIdAtVector;
    n = names_files_trace.size();
    num_borders = names_files_borders.size();
    for (int i = 0; i < names_files_trace.size(); i++)
    {
        traceIdAtFileToIdAtVector[names_files_trace[i].first] = i;

        std::list<Trace> traces;
        auto error = ParseData(names_files_trace[i].second, ntrace, traces);
        if (error != 0)
        {
            return FM_ERROR_PARAMETRS_NOT_VALID;
        }
        if (i == 0) // заполняем time
        {
            auto trace = traces.front();
            for (const auto& traceData : trace.values)
            {
                time.push_back(traceData.time);
            }
            m = traces.size();
        }

        for (const auto& trace : traces) {
            auto amps = trace.getData();
            signals.insert(signals.end(), amps.begin(), amps.end());
        }

        std::cout << "Loaded file with traces " << i + 1 << std::endl;
    }

    int bordersSize = n * m * num_borders;
    borders.resize(bordersSize);
    std::fill(borders.begin(), borders.end(), INT16_MIN);

    for (int i = 0; i < num_borders; i++)
    {
        std::ifstream file(names_files_borders[i]);

        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла." << std::endl;
            return 1;
        }

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string bufferStr(size, ' ');
        file.read(&bufferStr[0], size);
        file.close();
        std::istringstream buffer(bufferStr);

        std::string line;
        int minTraceId = -1;
        int minTracesFileId = -1;
        std::vector<BordersForTracesFile<T>*> borderInfo;
        BordersForTracesFile<T>* current;
        int lastFileWithTracesId = -1;
        while (std::getline(buffer, line)) {
            std::istringstream iss(line);
            std::string word;
            iss >> word; // мусорное слово
            iss >> word; // мусорное слово
            iss >> word;
            int fileWithTracesId = -1;
            try
            {
                fileWithTracesId = std::stoi(word);
                if (lastFileWithTracesId != fileWithTracesId)
                {
                    lastFileWithTracesId = fileWithTracesId;
                    current = new BordersForTracesFile<T>{ lastFileWithTracesId, std::vector<BorderForTracesFile<T>>() };
                    borderInfo.push_back(current); // одна граница распространяется на несколько Inline файлов
                }
            }
            catch (...)
            {
                break;
            }
            iss >> word; // мусорное слово
            iss >> word; // мусорное слово
            iss >> word;
            int traceId = std::stoi(word); // смещенный traceId
            if (minTraceId == -1)
                minTraceId = traceId;
            iss >> word;
            double x = std::stod(word);
            iss >> word;
            double y = std::stod(word);
            iss >> word;
            double time1 = std::stod(word);

            BorderForTracesFile<T> newBor{ traceId, x, y, time1 };
            current->borders.push_back(newBor); // у одного Inline файла много трасс
        }

        for (int j = 0; j < borderInfo.size() && j < n; j++) // проход каждого файла inline
        {
            for (int k = 0; k < borderInfo[j]->borders.size() && k < m; k++) // проход по трассам inline`а
            {
                // traceIdAtFileToIdAtVector[fileWithTracesId] // id среди файлов с трассами
                // traceId - minTraceId в файле с границами id трасс смещены, убираем смещение
                auto fileWithTracesId = borderInfo[j]->fileWithTracesId;
                auto traceId = borderInfo[j]->borders[k].traceId;
                auto time1 = borderInfo[j]->borders[k].time;

                const auto idx_y = traceIdAtFileToIdAtVector[fileWithTracesId];
                const auto idx_x = (traceId - minTraceId);
                if (idx_x < m && idx_y < n) {
                    borders[num_borders * (idx_y * m + idx_x) + i] = findClosestIndex<T>(time, time1);
                }
            }
        }

        for (auto& border : borderInfo)
        {
            delete border;
        }

        std::cout << "Loaded border file " << i + 1 << std::endl;
    }

    return FM_ERROR_NO;
}

// загрузка нескольких файлов вида inline_7110.txt + границ вида 3d_bottom.txt
int32_t LoadSeismic3D(
    const std::vector<std::pair<int32_t, std::string>>& names_files_trace,
    const std::vector<std::string>& names_files_borders,
    std::vector<double>& time,
    int32_t& n, // число inline`ов
    int32_t& m,
    std::vector<double>& signals,
    int32_t& num_borders,
    std::vector<int16_t>& borders,
    const int32_t ntrace
) {
    signals.resize(0);
    std::unordered_map<int32_t, int32_t> traceIdAtFileToIdAtVector;
    n = names_files_trace.size();
    num_borders = names_files_borders.size();
    for(int i = 0; i < names_files_trace.size(); i++)
    {
        traceIdAtFileToIdAtVector[names_files_trace[i].first] = i;

        std::list<Trace> traces;
        auto error = ParseData(names_files_trace[i].second, ntrace, traces);
        if(error != 0)
        {
            return FM_ERROR_PARAMETRS_NOT_VALID;
        }
        if(i == 0) // заполняем time
        {
            auto trace = traces.front();
            for(const auto& traceData : trace.values)
            {
                time.push_back(traceData.time);
            }
            m = traces.size();
        }

        for (const auto& trace : traces) {
            auto amps = trace.getData();
            signals.insert(signals.end(), amps.begin(), amps.end());
        }

        std::cout << "Loaded file with traces " << i+1 << std::endl;
    }

    int bordersSize = n * m * num_borders;
    borders.resize(bordersSize);
    std::fill(borders.begin(), borders.end(), INT16_MIN);

    for(int i = 0; i < num_borders; i++)
    {
        std::ifstream file(names_files_borders[i]);

        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла." << std::endl;
            return 1;
        }

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string bufferStr(size, ' ');
        file.read(&bufferStr[0], size);
        file.close();
        std::istringstream buffer(bufferStr);

        std::string line;
        int minTraceId = -1;
        int minTracesFileId = -1;
        std::vector<BordersForTracesFile<double>*> borderInfo;
        BordersForTracesFile<double>* current;
        int lastFileWithTracesId = -1;
        while (std::getline(buffer, line)) {
            std::istringstream iss(line);
            std::string word;
            iss >> word; // мусорное слово
            iss >> word; // мусорное слово
            iss >> word;
            int fileWithTracesId = -1;
            try
            {
                fileWithTracesId = std::stoi(word);
                if(lastFileWithTracesId != fileWithTracesId)
                {
                    lastFileWithTracesId = fileWithTracesId;
                    current = new BordersForTracesFile<double>{ lastFileWithTracesId, std::vector<BorderForTracesFile<double>>()};
                    borderInfo.push_back(current); // одна граница распространяется на несколько Inline файлов
                }
            }
            catch(...)
            {
                break;
            }
            iss >> word; // мусорное слово
            iss >> word; // мусорное слово
            iss >> word;
            int traceId = std::stoi(word); // смещенный traceId
            if(minTraceId == -1)
                minTraceId = traceId;
            iss >> word;
            double x = std::stod(word);            
            iss >> word;
            double y = std::stod(word);
            iss >> word;
            double time1 = std::stod(word);

            BorderForTracesFile<double> newBor{traceId, x, y, time1};
            current->borders.push_back(newBor); // у одного Inline файла много трасс
        }

        for(int j = 0; j < borderInfo.size() && j < n; j++) // проход каждого файла inline
        {
            for(int k = 0; k < borderInfo[j]->borders.size() &&  k < m; k++) // проход по трассам inline`а
            {
                // traceIdAtFileToIdAtVector[fileWithTracesId] // id среди файлов с трассами
                // traceId - minTraceId в файле с границами id трасс смещены, убираем смещение
                auto fileWithTracesId = borderInfo[j]->fileWithTracesId;
                auto traceId = borderInfo[j]->borders[k].traceId;
                auto time1 = borderInfo[j]->borders[k].time;

                const auto idx_y = traceIdAtFileToIdAtVector[fileWithTracesId];
                const auto idx_x = (traceId - minTraceId);
                if (idx_x < m && idx_y < n) {
                    borders[num_borders * (idx_y * m + idx_x) + i] = findClosestIndex<double>(time, time1);
                }
            }
        }

        for(auto& border : borderInfo)
        {
            delete border;
        }

        std::cout << "Loaded border file " << i + 1 << std::endl;
    }

    return FM_ERROR_NO;
}

int32_t LoadSeismic3DSingleTrace(
    const std::pair<int32_t, std::string>& file_trace_name,
    std::vector<double>& time,
    int32_t& m,
    std::vector<double>& signals
    )
{
    signals.resize(0);
    std::unordered_map<int32_t, int32_t> traceIdAtFileToIdAtVector;

    traceIdAtFileToIdAtVector[file_trace_name.first] = 0;

    std::list<Trace> traces;
    auto error = ParseData(file_trace_name.second, -1, traces);
    if (error != 0)
    {
        return FM_ERROR_PARAMETRS_NOT_VALID;
    }

    Trace trace = traces.front();
    for (const auto& traceData : trace.values)
    {
        time.push_back(traceData.time);
    }
    m = traces.size();

    for (const auto& trace : traces) {
        auto amps = trace.getData();
        signals.insert(signals.end(), amps.begin(), amps.end());
    }

    return FM_ERROR_NO;
}

int32_t LoadSeismic3DDouble(
    const std::vector<std::pair<int32_t, std::string>>& names_files_trace,   // имена inline_x.txt файлов
    const std::vector<std::string>& names_files_borders,                    // имена файлов с границами
    std::vector<double>& time,                                              // [out] временная сетка, можно взять из любой трассы
    int32_t& n,                                                             // [out] количество строк (количество наборов с трассами, другими словами inline`ы)
    int32_t& m,                                                             // [out] количество столбцов (количество трасс в наборе с трассами (в одном inline`е))
    std::vector<double>& signals,                                           // [out] загруженные сигналы [n*m*time.size]
    int32_t& num_borders,                                                   // [out] количество границ
    std::vector<int16_t>& borders,                                          // [out] хранит id в time. Размер: n * m * num_borders (если данных нет, брать минимальное значения для int16_t)
    const int32_t ntrace
)
{
    return LoadSeismic3DTemplate<double>(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders, ntrace);
}

int32_t LoadSeismic3DFloat(
    const std::vector<std::pair<int32_t, std::string>>& names_files_trace,   // имена inline_x.txt файлов
    const std::vector<std::string>& names_files_borders,                    // имена файлов с границами
    std::vector<float>& time,                                              // [out] временная сетка, можно взять из любой трассы
    int32_t& n,                                                             // [out] количество строк (количество наборов с трассами, другими словами inline`ы)
    int32_t& m,                                                             // [out] количество столбцов (количество трасс в наборе с трассами (в одном inline`е))
    std::vector<float>& signals,                                           // [out] загруженные сигналы [n*m*time.size]
    int32_t& num_borders,                                                   // [out] количество границ
    std::vector<int16_t>& borders,                                          // [out] хранит id в time. Размер: n * m * num_borders (если данных нет, брать минимальное значения для int16_t)
    const int32_t ntrace
)
{
    return LoadSeismic3DTemplate<float>(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders, ntrace);
}

int32_t LoadSeismic2DAutocorrTxt(
    std::string filePath, 
    int32_t maxTraces,
    std::vector<Trace>& traces
)
{
    std::list<Trace> tracesList;
    ParseData("../../FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt", maxTraces, tracesList);
    std::vector<Trace> tracesVector(traces.begin(), traces.end()); // массив всех трасс с сопоставлением времени и амплитуды
    traces.clear();
    traces.insert(traces.end(), tracesList.begin(), tracesList.end());
    return FM_ERROR_NO;
}
