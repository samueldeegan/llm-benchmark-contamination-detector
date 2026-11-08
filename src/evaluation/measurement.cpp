#include "measurement.h"
#include "question_checker.h"
#include "structures/hash_table.h"
#include <chrono>
#include <algorithm>

std::unique_ptr<Structure> makeStructure(const std::string& kind){
    if (kind == "hash table"){
        return std::make_unique<HashTable>();
    }
    return nullptr;
}
double timeOneBuild(Structure& structure, const std::vector<int>& corpus){
    auto start = std::chrono::steady_clock::now();
    structure.build(corpus);
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}
LookupRun timeOneLookupRun(const Structure& structure, const std::vector<std::vector<int>>& questions){
    auto start = std::chrono::steady_clock::now();
    std::vector<int> result = checkQuestions(structure, questions);
    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    return {ms, result};
}
double median(std::vector<double> times){
    std::sort(times.begin(), times.end());
    return times[times.size()/2];
}