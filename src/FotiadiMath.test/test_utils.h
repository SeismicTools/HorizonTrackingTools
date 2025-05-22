#pragma once
#include <gtest/gtest.h>
#include <list>  

typedef struct TraceData {
    int time;
    double value;
} TraceData;

typedef struct Trace {
    int id;
    std::list<TraceData> values;
    std::vector<double> getData()const {
        std::vector<double> data(values.size());
        auto it = values.begin();
        for (int i = 0; i < data.size(); i++) {
            data[i] = it->value;
            it++;
        }
        return data;
    }
} Trace;

typedef struct Hor {
    int id;
    double value;
} Hor;

template <typename T>
struct BorderForTracesFile // описание границы для одного inline_xxxx.txt файла
{
    int traceId;            // смещенный traceId
    int xCoord;
    int yCoord;
    double time;
};

template <typename T>
struct BordersForTracesFile
{
    int fileWithTracesId;                       // id файла с trace`ами inline_xxxx.txt
    std::vector<BorderForTracesFile<T>> borders;   // описание для одного файла с trace`ами inline_xxxx.txt
};

template<typename T, size_t size>
::testing::AssertionResult ArraysMatch(const T(&expected)[size],
    const T(&actual)[size]) {
    for (size_t i(0); i < size; ++i) {
        if (expected[i] != actual[i]) {
            return ::testing::AssertionFailure() << "array[" << i
                << "] (" << actual[i] << ") != expected[" << i
                << "] (" << expected[i] << ")";
        }
    }

    return ::testing::AssertionSuccess();
}

template<typename T, size_t size>
::testing::AssertionResult ArraysMatch(const T(&expected)[size],
    const T(&actual)[size],const T eps) {
    for (size_t i(0); i < size; ++i) {
        if (std::abs(expected[i] - actual[i]) >eps) {
            return ::testing::AssertionFailure() << "array[" << i
                << "] (" << actual[i] << ") != expected[" << i
                << "] (" << expected[i] << ")";
        }
    }

    return ::testing::AssertionSuccess();
}

void fix_zero(
    std::vector<double>& data
);

//! Распознает файлы вида FotiadiMath.test/Data/seismic2D/ex1/hor_1.dat и сохраняет их в hors
int ParseHor(
    std::string filePath,
    std::vector<Hor>& hors //! [out]
);

//! Распознает файлы вида FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt и сохраняет их в traces
int ParseData(
    std::string filePath,
    int32_t maxTraces,     //! ограничение на число считанных трасс (-1 - нет ограничения)
    std::list<Trace>& traces    //!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
);

// вытащить время и амплитуды из трассы
int GetTimeAndAmp(
    Trace trace, 
    std::vector<double>& times,     // [out] время
    std::vector<double>& amps       // [out] амплитуды
);

int32_t LoadSeismic3D(
    const std::vector<std::pair<int32_t,std::string>>& names_files_trace,   // имена inline_x.txt файлов
    const std::vector<std::string>& names_files_borders,                    // имена файлов с границами
    std::vector<double>& time,                                              // [out] временная сетка, можно взять из любой трассы
    int32_t& n,                                                             // [out] количество строк (количество наборов с трассами, другими словами inline`ы)
    int32_t& m,                                                             // [out] количество столбцов (количество трасс в наборе с трассами (в одном inline`е))
    std::vector<double>& signals,                                           // [out] загруженные сигналы [n*m*time.size]
    int32_t& num_borders,                                                   // [out] количество границ
    std::vector<int16_t>& borders,                                          // [out] хранит id в time. Размер: n * m * num_borders (если данных нет, брать минимальное значения для int16_t)
    const int32_t ntrace = -1
 );

int32_t LoadSeismic3DSingleTrace(
    const std::pair<int32_t, std::string>& file_trace_name,
    std::vector<double>& time,
    int32_t& m,
    std::vector<double>& signals
);

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
);

int32_t LoadSeismic3DDouble(
    const std::vector<std::pair<int32_t, std::string>>& names_files_trace,   // имена inline_x.txt файлов
    const std::vector<std::string>& names_files_borders,                    // имена файлов с границами
    std::vector<double>& time,                                              // [out] временная сетка, можно взять из любой трассы
    int32_t& n,                                                             // [out] количество строк (количество наборов с трассами, другими словами inline`ы)
    int32_t& m,                                                             // [out] количество столбцов (количество трасс в наборе с трассами (в одном inline`е))
    std::vector<double>& signals,                                           // [out] загруженные сигналы [n*m*time.size]
    int32_t& num_borders,                                                   // [out] количество границ
    std::vector<int16_t>& borders,                                          // [out] хранит id в time. Размер: n * m * num_borders (если данных нет, брать минимальное значения для int16_t)
    const int32_t ntrace = -1
);

int32_t LoadSeismic3DFloat(
    const std::vector<std::pair<int32_t, std::string>>& names_files_trace,   // имена inline_x.txt файлов
    const std::vector<std::string>& names_files_borders,                    // имена файлов с границами
    std::vector<float>& time,                                              // [out] временная сетка, можно взять из любой трассы
    int32_t& n,                                                             // [out] количество строк (количество наборов с трассами, другими словами inline`ы)
    int32_t& m,                                                             // [out] количество столбцов (количество трасс в наборе с трассами (в одном inline`е))
    std::vector<float>& signals,                                           // [out] загруженные сигналы [n*m*time.size]
    int32_t& num_borders,                                                   // [out] количество границ
    std::vector<int16_t>& borders,                                          // [out] хранит id в time. Размер: n * m * num_borders (если данных нет, брать минимальное значения для int16_t)
    const int32_t ntrace = -1
);

//! загрузка файла вида FotiadiMath\src\FotiadiMath.test\Data\seismic2D\ex1\2D_test_autocorr.txt
int32_t LoadSeismic2DAutocorrTxt(
    std::string filePath,               //! путь к файлу
    int32_t maxTraces,
    std::vector<Trace>& traces           //! [out] загруженные трассы
);

int32_t GetTimeAndAmpFromCsv(
    std::string namefile,
    std::vector<double>& times,     // [out] время
    std::vector<double>& amps       // [out] амплитуды
);
