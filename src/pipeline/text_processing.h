#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

std::string cleanText(const std::string& text);
std::vector<std::string>splitWords(const std::string& text);
std::vector<int> buildCorpus(const std::vector<std::string>& documents, std::unordered_map<std::string, int>& wordTable);
std::vector<std::vector<int>> questionsToIds(const std::vector<std::string>& questions, const std::unordered_map<std::string, int>& wordTable);
