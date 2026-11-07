#pragma once
#include <string>
#include <vector>
#include <utility>

struct PipelineSettings{
    std::vector<int> corpusCols = {1,2};
    bool corpusHeader = false;
    std::vector<int> benchCols = {0};
    bool benchHeader = false;
    int plantCount = 100;
    unsigned seed = 42;
};

struct PipelineOutput{
    
    std::vector<int> corpus;
    std::vector<std::vector<int>> questions;
    std::vector<std::pair<int,int>> answerKey;
    std::vector<std::string> cleanedQuestions;
    int numDocuments = 0;
    int numCorpusWords = 0;
    int numShortQuestions = 0;
    int numPlanted = 0;
    int numQuestions = 0;

};

PipelineOutput runPipeline(const std::string& corpusPath, const std::string& benchPath, const PipelineSettings& settings);