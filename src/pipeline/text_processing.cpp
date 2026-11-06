#include "text_processing.h"
#include "contamination.h"
#include <sstream>


std::string cleanText(const std::string& text){
    std::string result = text;
    std::size_t chars = result.length();
    for (std::size_t i = 0; i < chars; i++){
        if (isupper(static_cast<unsigned char>(result[i]))){
            result[i] = (char)tolower(static_cast<unsigned char>(result[i]));
        }
        if (!isalnum(static_cast<unsigned char>(result[i]))){
            result[i] = ' ';
        }
    }
    return result;
}
std::vector<std::string>splitWords(const std::string& text){
    std::vector<std::string> words;
    std::istringstream in(text);
    std::string w;
    while (in >> w){
        words.push_back(w);
    }
    return words;
}
std::vector<int> buildCorpus(const std::vector<std::string>& documents, std::unordered_map<std::string, int>& wordTable){
    std::vector<int> corpus;
    for (const auto& document : documents){
        std::vector<std::string> words = splitWords(document);
        for (const auto& word: words){
            auto it = wordTable.find(word);
            int id;
            if (it == wordTable.end()){
                id = wordTable.size();
                wordTable[word] = id;
            }
            else{
                id = it->second;
            }
            corpus.push_back(id);
        }
        corpus.push_back(DOC_END);
    }
    return corpus;
}
std::vector<std::vector<int>> questionsToIds(const std::vector<std::string>& questions, const std::unordered_map<std::string, int>& wordTable){
    std::vector<std::vector<int>> result;
    for (const auto& question: questions){
        std::vector<int> ids;
        std::vector<std::string> words = splitWords(question);
        for (const auto& word : words){
            auto it = wordTable.find(word);
            if (it == wordTable.end()){
                ids.push_back(UNKNOWN);
            }
            else{
                ids.push_back(it->second);
            }
        }
        result.push_back(ids);
    }
    return result;
}