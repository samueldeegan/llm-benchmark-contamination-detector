#include "correctness.h"

CheckReport checkCorrectness(const std::vector<int>& flagged, const std::vector<std::pair<int,int>>& answerKey, int questionCount, const std::vector<std::string>& cleanedQuestions){
    CheckReport report;
    std::vector<bool> isPlanted(questionCount, false);
    for (const auto& pair: answerKey){
        isPlanted[pair.first] = true;
    }
    std::vector<bool> isFlagged(questionCount, false);
    for (int id : flagged){
        isFlagged[id] = true;
    }
    report.numPlanted = answerKey.size();
    for (const auto& pair: answerKey){
        if (!isFlagged[pair.first]){
            report.missed.push_back(pair.first);
        }
    }
    report.found = report.numPlanted - report.missed.size();

    for (int id: flagged){
        if (!isPlanted[id]){
            report.unplantedHits.push_back({id, cleanedQuestions[id]});
        }
    }
    return report;
}
