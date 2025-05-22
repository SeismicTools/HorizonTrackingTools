#pragma once

#include <string>
#include <vector>
#include <tuple>

void save_csv(const std::string& namefile, const std::vector<std::tuple<std::string, std::vector < double >>>& data);

void save_csv(const std::string& namefile, const std::vector<std::tuple<std::string, std::vector < int32_t >>>& data);
