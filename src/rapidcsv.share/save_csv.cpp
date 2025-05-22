#include "save_csv.h"
#include <fstream>

void save_csv(const std::string& namefile, const std::vector<std::tuple<std::string, std::vector < double>>>& data) {
	std::ofstream out(namefile);
	int counter = 0;
	for (const auto& cur : data) {
		out << std::get<0>(cur);
		if (counter < data.size() - 1) {
			out << ";";
		}
		counter++;
	}
	out << std::endl;
	
	const auto n = std::get<1>(data.front()).size();
	for (std::size_t i = 0; i < n; i++) {
		counter = 0;
		for (const auto& cur : data) {
			out << std::get<1>(cur)[i];
			if (counter < data.size() - 1) {
				out << ";";
			}
			counter++;
		}
		out << std::endl;
	}
	out.close();
}

void save_csv(const std::string& namefile, const std::vector<std::tuple<std::string, std::vector < int32_t >>>& data) {
	std::ofstream out(namefile);
	int counter = 0;
	for (const auto& cur : data) {
		out << std::get<0>(cur);
		if (counter < data.size() - 1) {
			out << ";";
		}
		counter++;
	}
	out << std::endl;

	const auto n = std::get<1>(data.front()).size();
	for (std::size_t i = 0; i < n; i++) {
		counter = 0;
		for (const auto& cur : data) {
			out << std::get<1>(cur)[i];
			if (counter < data.size() - 1) {
				out << ";";
			}
			counter++;
		}
		out << std::endl;
	}
	out.close();
}
