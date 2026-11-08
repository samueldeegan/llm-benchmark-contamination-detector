#include <vector>
#include <string>
#include <memory>
#include <cstddef>
#include "contamination.h"

struct LookupRun{
    double ms;
    std::vector<int> flagged;
};

struct ResultsRecord{
    std::string name;
    double medianBuildMs = 0;
    double medianLookupMs = 0;
    std::size_t bytesUsed;
    std::vector<int> flagged;
};

std::unique_ptr<Structure> makeStructure(const std::string& kind);
double timeOneBuild(Structure& structure, const std::vector<int>& corpus);
LookupRun timeOneLookupRun(const Structure& structure, const std::vector<std::vector<int>>& questions);
double median(std::vector<double> time);
