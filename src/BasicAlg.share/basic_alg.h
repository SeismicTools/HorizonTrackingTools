#pragma once 

#include <cstdint>

void FilterWindowAvg(const int32_t n,const double arr_init[],const int32_t r_window, double arr_filtered[]);

double linef(const double x1, const double y1, const double x2, const double y2, const double x);

void LineTransformFunction(const int32_t n, const double grid[], const double f[], const int32_t n_new, const double grid_new[], double f_new[]);