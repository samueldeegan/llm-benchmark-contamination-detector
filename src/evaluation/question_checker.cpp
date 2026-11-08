#include "question_checker.h"

std::vector<int> checkQuestions(const Structure& structure, const std::vector<std::vector<int>>& questions){
    std::vector<int> flagged;
    for (std::size_t i = 0; i < questions.size(); i++){
        std::size_t L = questions[i].size();
        if (L < WINDOW){
            continue;
        }
        for (std::size_t s = 0; s <= L - WINDOW; s++){
            if (structure.contains(questions[i],s)){
                flagged.push_back(i);
                break;
            }
        }
    }
    return flagged;
}
