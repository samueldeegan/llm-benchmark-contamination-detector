#pragma once
#include <vector>
#include <string>
#include <utility>

struct CheckReport{
    int numPlanted = 0;
    int found = 0;
    std::vector<int> missed;
    std::vector<std::pair<int, std::string>> unplantedHits;
};

CheckReport checkCorrectness(const std::vector<int>& flagged, const std::vector<std::pair<int,int>>& answerKey, int questionCount, const std::vector<std::string>& cleanedQuestions);
