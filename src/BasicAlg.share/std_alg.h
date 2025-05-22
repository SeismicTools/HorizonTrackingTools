#pragma once

#include <cstdint>
#include <vector>
#include <limits>
#include<functional>

/**
 * @param v - sorted vector instance
 * @param data - value to search
 * @return 0-based index if data found, -1 otherwise
*/
template<typename T> int32_t binary_search_find_index(const std::vector<T>& arr,const T& key) {
    /*bool flag = false;
    int32_t l = 0; // левая граница
    int32_t r = arr.size()-1; // правая граница
    int32_t mid;
    while ((l <= r) && (flag != true)) {
        mid = (l + r) / 2; // считываем срединный индекс отрезка [l,r]

        if (arr[mid] == key) flag = true; //проверяем ключ со серединным элементом
        if (arr[mid] > key) r = mid - 1; // проверяем, какую часть нужно отбросить
        else l = mid + 1;
    }
    return mid;*/
    auto it = std::lower_bound(arr.begin(), arr.end(), key);
    if (it == arr.end()) {
        return -1;
    }
    else {
        std::size_t index = std::distance(arr.begin(), it);
        return index;
    }
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

//поиск минимального элемента в массиве
template <typename T> T array_min(
    const size_t size,			//!< количество сигналов на сетке X 
    const T data[]			//!< сигналы на сетке X	[n_grid]
) {
    T gmin = std::numeric_limits<T>::max();
    for (int64_t i = 0; i < size; i++) {
        gmin = std::min(gmin, data[i]);
    }
    return gmin;
}

template <typename T> T array_min_parallel(
    const size_t size,			//!< количество сигналов на сетке X 
    const T data[]			//!< сигналы на сетке X	[n_grid]
) {
    T gmin = std::numeric_limits<T>::max();
    #pragma omp parallel for shared(data) reduction(min:gmin) 
    for (int64_t i = 0; i < size; i++) {
        gmin = std::min(gmin, data[i]);
    }
    return gmin;
}

//поиск минимального элемента в массиве
template <typename T> T array_max(
    const size_t size,			//!< количество сигналов на сетке X 
    const T data[]			//!< сигналы на сетке X	[n_grid]
) {
    T gmax = std::numeric_limits<T>::min();
    for (int64_t i = 0; i < size; i++) {
        gmax = std::max(gmax, data[i]);
    }
    return gmax;
}

template <typename T> T array_max_parallel(
    const size_t size,	
    const T data[]			
) {
    T gmax = std::numeric_limits<T>::min();
#pragma omp parallel for shared(data) reduction(max:gmax) 
    for (int64_t i = 0; i < size; i++) {
        gmax = std::max(gmax, data[i]);
    }
    return gmax;
}

template <typename T> void array_interval(
    const size_t size,			
    const T data[],
    T& min,
    T& max
) {
    min = array_min(size, data);
    max = array_max(size, data);
}

template <typename T> void array_interval_parallel(
    const size_t size,
    const T data[],
    T& min,
    T& max
) {
    min = array_min_parallel(size, data);
    max = array_max_parallel(size, data);
}

template <typename T> void array_change(
    const size_t size,			
    T data[],			
    std::function<T(T)> f) {
    for (int64_t i = 0; i < size; i++) {
        data[i] = f(data[i]);
    }
}

template <typename T> void array_change_parallel(
    const size_t size,			
    T data[],			
    std::function<T(T)> f) {
    #pragma omp parallel for
    for (int64_t i = 0; i < size; i++) {
        data[i] = f(data[i]);
    }
}

