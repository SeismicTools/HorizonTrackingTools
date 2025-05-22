#include <gtest/gtest.h>
#include "fotiadi_math.h"
#include "rapidcsv.h"
#include <chrono>
#include "save_csv.h"
#include "test_utils.h"

using namespace std;

static TEST(Seismic2D, dataParsing) {
    std::list<Trace> traces;
    auto start = std::chrono::high_resolution_clock::now();
    ParseData("../../FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt", -1, traces);
    auto end = std::chrono::high_resolution_clock::now();

    std::vector<std::tuple<std::string, std::vector <double>>> data;
    for (const auto& cur_val : traces) {
        data.push_back(std::make_tuple(std::string("trace")+ std::to_string(cur_val.id), cur_val.getData()));
    }
    save_csv("data.init.csv", data);

    // Рассчитываем время выполнения в секундах
    std::chrono::duration<double> duration = end - start;
    double seconds = duration.count();
    std::cout << "Execution time: " << seconds << " seconds." << std::endl;
}

static TEST(Seismic2D, horParsing) {
    std::vector<Hor> hors;
    ParseHor("../../FotiadiMath.test/Data/seismic2D/ex1/hor_1.dat", hors);
    std::cout << "Execution time: seconds." << std::endl;
}

// тест на две одинаковые трассы
static TEST(Seismic2D, seismic2DCorrelation) {
    std::string namefile_exp = "../../FotiadiMath.test/Data/ex1/Los-Yar_41_2D_Trace_1500.csv";

    rapidcsv::Document doc_ex(
        namefile_exp,
        rapidcsv::LabelParams(),
        rapidcsv::SeparatorParams(';')
    );
    std::vector<double> time_ex = doc_ex.GetColumn<double>("time");
    std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");

    int time_start_interval = 1750;
    int time_finish_interval = 2000;
    int max_time_shift_data = 10;
    double step_data = 1;
    int32_t r_idx_convolution = 10;

    const double time_start = time_start_interval, time_finish = time_finish_interval, step = step_data;
    const double max_time_shift = max_time_shift_data;
    const int32_t n_array_grid_x = (time_ex.back() - time_ex.front()) / step + 1;
    std::vector<double> signal_x(n_array_grid_x);
    const int32_t n_array_grid_y = (time_ex.back() - time_ex.front()) / step + 1;
    std::vector<double> signal_y(n_array_grid_y);
    int32_t n_interval;
    int32_t n_signal_idx_x, shift_idx_x;
    int32_t n_signal_idx_y, shift_idx_y;
    int32_t max_shift_point_idx;
    FM_PrepareDataForCorrelation(
        time_ex.size(), time_ex.data(), amp_ex.data(),
        time_ex.size(), time_ex.data(), amp_ex.data(),
        0, NULL, NULL, NULL, time_start, time_finish, step, max_time_shift,
        n_interval,
        signal_x.size(), n_signal_idx_x, shift_idx_x, signal_x.data(),
        signal_y.size(), n_signal_idx_y, shift_idx_y, signal_y.data(),
        NULL, NULL, NULL, max_shift_point_idx
    );
    int32_t r_idx = r_idx_convolution;

    std::vector<double> signals;
    signals.insert(signals.end(), signal_x.begin(), signal_x.end());
    signals.insert(signals.end(), signal_y.begin(), signal_y.end());

    std::vector<int32_t> result(2 * n_signal_idx_x);
    FM_Seismic2DCorrelationByWaveAlgorithm(2, n_signal_idx_x, signals.data(), max_shift_point_idx,
        0, NULL, NULL, MetricFunctionSimpleConvolution, &r_idx, result.data());
    
    auto first = result.begin();
    auto last = result.begin() + n_signal_idx_x;
    std::vector<int32_t> trace1;
    trace1.insert(trace1.end(), first, last);

    first = result.begin() + n_signal_idx_x;
    last = result.begin() + 2 * n_signal_idx_x;
    std::vector<int32_t> trace2;
    trace2.insert(trace2.end(), first, last);

    std::vector<int> ids;
    for (int i = 1; i <= n_signal_idx_x; i++) {
        ids.push_back(i);
    }

    save_csv("seismic.ex1.out.csv",
        {
            std::make_tuple("id", ids),
            std::make_tuple("trace1", trace1),
            std::make_tuple("trace2", trace2),
        }
    );
}

// тест на две одинаковые трассы
static TEST(Seismic2D, seismic2DCorrelationOnBigDataWithoutBordersTwoEqual) {
    std::list<Trace> traces;
    ParseData("../../FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt", -1, traces);

    std::vector<Trace> tracesVector(traces.begin(), traces.end());
    auto size = tracesVector[0].values.size();
    std::vector<double> time_ex(size);
    std::vector<double> amp_ex(size);
    GetTimeAndAmp(tracesVector[0], time_ex, amp_ex); // взял первую трассу, вытащил время и амплитуды

    int time_start_interval = 100;
    int time_finish_interval = 2100;
    int max_time_shift_data = 10;
    double step_data = 2;
    int32_t r_idx_convolution = 10;

    const double time_start = time_start_interval, time_finish = time_finish_interval, step = step_data;
    const double max_time_shift = max_time_shift_data;
    const int32_t n_array_grid_x = (time_ex.back() - time_ex.front()) / step + 1;
    std::vector<double> signal_x(n_array_grid_x);
    const int32_t n_array_grid_y = (time_ex.back() - time_ex.front()) / step + 1;
    std::vector<double> signal_y(n_array_grid_y);
    int32_t n_interval;
    int32_t n_signal_idx_x, shift_idx_x;
    int32_t n_signal_idx_y, shift_idx_y;
    int32_t max_shift_point_idx;
    FM_PrepareDataForCorrelation(
        time_ex.size(), time_ex.data(), amp_ex.data(),
        time_ex.size(), time_ex.data(), amp_ex.data(),
        0, NULL, NULL, NULL, 
        time_start, time_finish, 
        step, max_time_shift,
        n_interval,
        signal_x.size(), n_signal_idx_x, shift_idx_x, signal_x.data(),
        signal_y.size(), n_signal_idx_y, shift_idx_y, signal_y.data(),
        NULL, NULL, NULL, max_shift_point_idx
    );
    int32_t r_idx = r_idx_convolution;

    // все сигналы слепил в один массив
    std::vector<double> signals;
    signals.insert(signals.end(), signal_x.begin(), signal_x.end());
    signals.insert(signals.end(), signal_y.begin(), signal_y.end());

    std::vector<int32_t> result(2 * n_signal_idx_x);
    FM_Seismic2DCorrelationByWaveAlgorithm(2, n_signal_idx_x, signals.data(), max_shift_point_idx,
        0, NULL, NULL, MetricFunctionSimpleConvolution, &r_idx, result.data());

    auto first = result.begin();
    auto last = result.begin() + n_signal_idx_x;
    std::vector<int32_t> trace1;
    trace1.insert(trace1.end(), first, last);

    first = result.begin() + n_signal_idx_x;
    last = result.begin() + 2 * n_signal_idx_x;
    std::vector<int32_t> trace2;
    trace2.insert(trace2.end(), first, last);

    std::vector<int> ids; 
    for (int i = 1; i <= n_signal_idx_x; i++) {
        ids.push_back(i);
    }

    save_csv("seismic.ex2.out.csv",
        {
            std::make_tuple("id", ids),
            std::make_tuple("trace1", trace1),
            std::make_tuple("trace2", trace2),
        }
    );
}

static TEST(Seismic2D, seismic2DCorrelationOnBigDataWithoutBordersAll)
{
    std::list<Trace> traces;
    ParseData("../../FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt", -1, traces);
    std::vector<Trace> tracesVector(traces.begin(), traces.end()); // массив всех трасс с сопоставлением времени и амплитуды

    size_t size = tracesVector[0].values.size();
    std::vector<double> time_ex_all(size * tracesVector.size()); // массив [n_trace, time]
    std::vector<double> amp_ex_all(size * tracesVector.size()); // массив [n_trace, amp]

    size_t idAtVectors = 0;
    // заполненеие time_ex_all и amp_ex_all
    for(int i = 0; i < tracesVector.size(); i++)
    {
        std::vector<double> time_ex(size);
        std::vector<double> amp_ex(size);
        GetTimeAndAmp(tracesVector[i], time_ex, amp_ex); // взял первую трассу, вытащил время и амплитуды

        for(int i = 0; i < time_ex.size(); i++)
        {
            time_ex_all[idAtVectors] = time_ex[i];
            amp_ex_all[idAtVectors] = amp_ex[i];
            idAtVectors++;
        }
    }

    int time_start_interval = 100;
    int time_finish_interval = 2100;
    int max_time_shift_data = 10;
    double step_data = 2;
    int32_t r_idx_convolution = 10;

    const double time_start = time_start_interval, time_finish = time_finish_interval, step = step_data;
    const double max_time_shift = max_time_shift_data;
    int32_t n_interval;
    int32_t n_signal_idx;
    //FM_PrepareDataForCorrelationSeveral(size, (int32_t)tracesVector.size(), time_ex_all.data(), amp_ex_all.data(), 0, NULL, NULL,
    //    time_start, time_finish,
    //    step, max_time_shift, 
    //    n_interval,
    //    tracesVector[0].values.size(), // n_signal
    //    n_signal_idx, // n_signal_idx


    //    );
}

// тест на две разные трассы
static TEST(Seismic2D, seismic2DCorrelationOnBigDataWithoutBordersTwoDifference) {
    std::list<Trace> traces;
    ParseData("../../FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt", -1, traces);
    std::vector<Trace> tracesVector(traces.begin(), traces.end());

    auto size = tracesVector[0].values.size();
    std::vector<double> time_ex(size);
    std::vector<double> amp_ex(size);
    GetTimeAndAmp(tracesVector[0], time_ex, amp_ex); // взял первую трассу, вытащил время и амплитуды

    auto sizeR = tracesVector[900].values.size();
    std::vector<double> time_exR(sizeR);
    std::vector<double> amp_exR(sizeR);
    GetTimeAndAmp(tracesVector[900], time_exR, amp_exR); // взял первую трассу, вытащил время и амплитуды

    int time_start_interval = 100;
    int time_finish_interval = 2100;
    int max_time_shift_data = 10;
    double step_data = 2;
    int32_t r_idx_convolution = 10;

    const double time_start = time_start_interval, time_finish = time_finish_interval, step = step_data;
    const double max_time_shift = max_time_shift_data;
    const int32_t n_array_grid_x = (time_ex.back() - time_ex.front()) / step + 1;
    std::vector<double> signal_x(n_array_grid_x);
    const int32_t n_array_grid_y = (time_exR.back() - time_exR.front()) / step + 1;
    std::vector<double> signal_y(n_array_grid_y);
    int32_t n_interval;
    int32_t n_signal_idx_x, shift_idx_x;
    int32_t n_signal_idx_y, shift_idx_y;
    int32_t max_shift_point_idx;
    FM_PrepareDataForCorrelation(
        time_ex.size(), time_ex.data(), amp_ex.data(),
        time_exR.size(), time_exR.data(), amp_exR.data(),
        0, NULL, NULL, NULL,
        time_start, time_finish,
        step, max_time_shift,
        n_interval,
        signal_x.size(), n_signal_idx_x, shift_idx_x, signal_x.data(),
        signal_y.size(), n_signal_idx_y, shift_idx_y, signal_y.data(),
        NULL, NULL, NULL, max_shift_point_idx
    );
    int32_t r_idx = r_idx_convolution;

    // все сигналы слепил в один массив
    std::vector<double> signals;
    signals.insert(signals.end(), signal_x.begin(), signal_x.end());
    signals.insert(signals.end(), signal_y.begin(), signal_y.end());

    std::vector<int32_t> result(2 * n_signal_idx_x);
    FM_Seismic2DCorrelationByWaveAlgorithm(2, n_signal_idx_x, signals.data(), max_shift_point_idx,
        0, NULL, NULL, MetricFunctionSimpleConvolution, &r_idx, result.data());

    auto first = result.begin();
    auto last = result.begin() + n_signal_idx_x;
    std::vector<int32_t> trace1;
    trace1.insert(trace1.end(), first, last);

    first = result.begin() + n_signal_idx_x;
    last = result.begin() + 2 * n_signal_idx_x;
    std::vector<int32_t> trace2;
    trace2.insert(trace2.end(), first, last);

    std::vector<int> ids;
    for (int i = 1; i <= n_signal_idx_x; i++) {
        ids.push_back(i);
    }

    save_csv("seismic.ex2.out.csv",
        {
            std::make_tuple("id", ids),
            std::make_tuple("trace1", trace1),
            std::make_tuple("trace2", trace2),
        }
    );
}

static TEST(Seismic2D, test_test) {
    std::string bla;
    auto res = FM_TestFunc(777, bla);
    EXPECT_EQ(res, 0);
}