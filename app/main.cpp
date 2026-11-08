#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <exception>
#include "pipeline/pipeline.h"
#include "evaluation/measurement.h"
#include "evaluation/correctness.h"

// 1. Parse columns string like "1,2" into a vector of integers
std::vector<int> parseColumns(const std::string& text) {
    std::vector<int> cols;
    std::stringstream ss(text);
    std::string part;
    while (std::getline(ss, part, ',')) {
        cols.push_back(std::stoi(part));
    }
    return cols;
}

// 2. Parse command line arguments
void parseArgs(int argc, char** argv, std::string& corpusPath, std::string& benchPath, PipelineSettings& settings, int& runs) {
    if (argc < 3) {
        throw std::runtime_error("missing file paths");
    }
    corpusPath = argv[1];
    benchPath = argv[2];

    for (int i = 3; i < argc; i++) {
        std::string flag = argv[i];
        if (i + 1 >= argc) {
            throw std::runtime_error("missing value for " + flag);
        }
        std::string value = argv[i + 1];
        i++; // skip past the value

        if (flag == "--corpus-cols") {
            settings.corpusCols = parseColumns(value);
        } else if (flag == "--corpus-header") {
            settings.corpusHeader = (value == "yes");
        } else if (flag == "--bench-cols") {
            settings.benchCols = parseColumns(value);
        } else if (flag == "--bench-header") {
            settings.benchHeader = (value == "yes");
        } else if (flag == "--plant") {
            settings.plantCount = std::stoi(value);
        } else if (flag == "--seed") {
            settings.seed = std::stoul(value);
        } else if (flag == "--runs") {
            runs = std::stoi(value);
        } else {
            throw std::runtime_error("unknown option " + flag);
        }
    }
}

// 3. Print usage instructions
void printUsage(const std::string& reason) {
    std::cerr << "Error: " << reason << "\n";
    std::cerr << "Usage: ./detector <corpus.csv> <benchmark.csv> [--corpus-cols 1,2] [--corpus-header no] [--bench-cols 0] [--bench-header no] [--plant 100] [--seed 42] [--runs 5]\n";
}

// 4. Measure structure build and lookup performance
ResultsRecord measureStructure(const std::string& kind, const PipelineOutput& data, int runs) {
    ResultsRecord record;
    std::vector<double> buildTimes;
    std::vector<double> lookupTimes;
    std::unique_ptr<Structure> s;

    for (int r = 0; r < runs; ++r) {
        s = makeStructure(kind);
        buildTimes.push_back(timeOneBuild(*s, data.corpus));
    }

    record.bytesUsed = s->bytesUsed();

    for (int r = 0; r < runs; ++r) {
        LookupRun run = timeOneLookupRun(*s, data.questions);
        lookupTimes.push_back(run.ms);
        record.flagged = run.flagged;
    }

    record.name = kind;
    record.medianBuildMs = median(buildTimes);
    record.medianLookupMs = median(lookupTimes);

    return record;
}

// 5. Print comprehensive results report
void printResults(const PipelineOutput& data, const ResultsRecord& ht, const CheckReport& report, int runs) {
    std::cout << "Data\n";
    std::cout << "  Documents: " << data.numDocuments << "   Corpus words: " << data.numCorpusWords << "\n";
    std::cout << "  Questions: " << data.numQuestions << "    Shorter than 13 words: " << data.numShortQuestions << "   Planted: " << data.numPlanted << "\n\n";

    std::cout << "Result\n";
    std::cout << "  " << ht.flagged.size() << " benchmark questions were found in the training set\n";
    if (data.numPlanted > 0) {
        std::cout << "  (includes the " << data.numPlanted << " planted test questions)\n";
    }
    std::cout << "\n";

    std::cout << "Performance (median of " << runs << " runs)\n";
    std::cout << "  Structure      Build (ms)   Lookup (ms)   Memory (bytes)   Flagged\n";
    std::cout << "  Hash table     " << ht.medianBuildMs << "          " << ht.medianLookupMs << "           " << ht.bytesUsed << "              " << ht.flagged.size() << "\n\n";

    std::cout << "Correctness\n";
    if (report.numPlanted > 0) {
        std::cout << "  Planted found: " << report.found << "/" << report.numPlanted << "\n";
    }
    
    std::cout << "  Planted missed: ";
    if (report.missed.empty()) {
        std::cout << "none\n";
    } else {
        for (size_t i = 0; i < report.missed.size(); ++i) {
            std::cout << report.missed[i] << (i + 1 < report.missed.size() ? ", " : "");
        }
        std::cout << "\n";
    }

    std::cout << "  Unplanted hits: " << report.unplantedHits.size() << " (first 20 shown)\n";
    size_t limit = report.unplantedHits.size() < 20 ? report.unplantedHits.size() : 20;
    for (size_t i = 0; i < limit; ++i) {
        std::cout << "    [ID " << report.unplantedHits[i].first << "] " 
                  << report.unplantedHits[i].second.substr(0, 80) << "\n";
    }
}

// 6. Main execution flow
int main(int argc, char** argv) {
    std::string corpusPath, benchPath;
    PipelineSettings settings;
    int runs = 5;

    try {
        parseArgs(argc, argv, corpusPath, benchPath, settings, runs);
        PipelineOutput data = runPipeline(corpusPath, benchPath, settings);

        if (data.numPlanted < settings.plantCount) {
            std::cerr << "Warning: only " << data.numPlanted << " questions qualified.\n";
        }

        ResultsRecord ht = measureStructure("hash table", data, runs);
        CheckReport report = checkCorrectness(ht.flagged, data.answerKey, data.numQuestions, data.cleanedQuestions);
        printResults(data, ht, report, runs);

    } catch (const std::exception& e) {
        printUsage(e.what());
        return 1;
    }

    return 0;
}