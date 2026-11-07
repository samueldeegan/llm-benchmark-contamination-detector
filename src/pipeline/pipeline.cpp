#include "pipeline.h"
#include "csv_reader.h"
#include "text_processing.h"
#include "planting.h"
#include <random>
#include <unordered_map>


PipelineOutput runPipeline(const std::string& corpusPath, const std::string& benchPath, const PipelineSettings& settings){
    PipelineOutput out;
    
    std::vector<std::vector<std::string>> benchmarkCsv = readCsv(benchPath);
    std::vector<std::string> benchmarkTextVectors = extractText(benchmarkCsv, settings.benchCols, settings.benchHeader);
    for (const std::string& text : benchmarkTextVectors){
        std::string cleanedBenchmarkText = cleanText(text);
        out.cleanedQuestions.push_back(cleanedBenchmarkText); 
    }
    
    std::vector<std::vector<std::string>> corpusCsv = readCsv(corpusPath);
    std::vector<std::string> corpusTextVectors = extractText(corpusCsv, settings.corpusCols, settings.corpusHeader);
    std::vector<std::string> docs;
    for (const std::string& text : corpusTextVectors){
        std::string cleanedCorpusText = cleanText(text);
        docs.push_back(cleanedCorpusText); 
    }

    std::mt19937 rng(settings.seed);
    std::vector<int> chosen = pickQuestionsToPlant(out.cleanedQuestions, settings.plantCount, rng, out.numShortQuestions);
    out.answerKey = plantQuestions(docs, chosen, out.cleanedQuestions, rng);
    std::unordered_map<std::string, int> wordTable;
    out.corpus = buildCorpus(docs, wordTable);
    out.questions = questionsToIds(out.cleanedQuestions, wordTable);
    out.numDocuments = docs.size();
    out.numCorpusWords = out.corpus.size() - out.numDocuments;
    out.numQuestions = out.cleanedQuestions.size();
    out.numPlanted = out.answerKey.size();

    return out;

}