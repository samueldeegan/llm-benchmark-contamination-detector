#include <vector>
#include <string>
#include <random>
#include <utility>

std::vector<int> pickQuestionsToPlant(const std::vector<std::string>& questions, int plantCount, std::mt19937& rng, int& shortCount);
std::vector<std::pair<int,int>> plantQuestions(std::vector<std::string>& documents, const std::vector<int>& chosenIds, const std::vector<std::string>& questions, std::mt19937& rng);