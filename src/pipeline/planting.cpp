#include "planting.h"
#include "contamination.h"
#include "text_processing.h"
#include <algorithm>



std::vector<int> pickQuestionsToPlant(const std::vector<std::string>& questions, int plantCount, std::mt19937& rng, int& shortCount){
    shortCount = 0;
    std::vector<int> candidates;
    for (std::size_t i = 0; i < questions.size(); i++){
        if (splitWords(questions[i]).size() < WINDOW){
            shortCount++;
        }
        else{
            candidates.push_back(i);
        }
    }
    std::shuffle(candidates.begin(), candidates.end(), rng);
    int returnedCount = std::min(static_cast<std::size_t>(plantCount), candidates.size());
    std::vector<int> returnedCandidates;
    for (int j = 0; j < returnedCount; j++){
        returnedCandidates.push_back(candidates[j]);
    }
    return returnedCandidates;

}
std::vector<std::pair<int,int>> plantQuestions(std::vector<std::string>& documents, const std::vector<int>& chosenIds, const std::vector<std::string>& questions, std::mt19937& rng){
    std::vector<int> docIds;
    for (std::size_t i = 0; i < documents.size(); i++){
        docIds.push_back(i);
    }
    std::shuffle(docIds.begin(), docIds.end(), rng);
    std::vector<std::pair<int,int>> answerKey;
    std::size_t chosenSize = chosenIds.size();
    for (size_t j = 0; j < chosenSize; j++){
        int q = chosenIds[j];
        int d = docIds[j];
        documents[d] += " " + questions[q];
        answerKey.push_back({q,d});
    }
    return answerKey;
}