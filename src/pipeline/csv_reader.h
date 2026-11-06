#pragma once
#include <string>
#include <vector>


std::vector<std::vector<std::string>> readCsv(const std::string filePath);
std::vector<std::string> extractText(const std::vector<std::vector<std::string>>& rows, const std::vector<int>& columns, bool header);