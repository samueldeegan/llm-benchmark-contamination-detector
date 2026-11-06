#include "csv_reader.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<std::vector<std::string>> readCsv(const std::string filePath){
    std::ifstream file(filePath);
    if (!file){
        throw std::runtime_error("Could not open " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();

    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> row;
    std::string field;
    bool inQuotes = false;
    std::size_t csvLength = text.size();

    for (std::size_t i = 0; i < csvLength; i++){
        if (inQuotes){
            if ( text[i] == '"' && text[i+1] == '"'){
                field += '"';
                i++;
            }
            else if (text[i] == '"' && text[i+1] != '"'){
                inQuotes = false;
            }
            else{
                field += text[i];
            }
        }
        else{
            if (text[i] == '"'){
                inQuotes = true;
            }
            else if (text[i] == ','){
                row.push_back(field);
                field = "";
            }
            else if (text[i] == '\n'){
                row.push_back(field);
                rows.push_back(row);
                field = "";
                row.clear();
            }
            else if (text[i] != '\r'){
                field += text[i];
            }
        }
    }
    if (field != ""){
        row.push_back(field);
    }
    if (!row.empty()){
        rows.push_back(row);
    }
    return rows;
}
std::vector<std::string> extractText(const std::vector<std::vector<std::string>>& rows, const std::vector<int>& columns, bool header){
    std::vector<std::string> extractedText;
    std::size_t i = header ? 1 : 0;
    for (; i < rows.size(); i++){
        std::string text = "";
        for (std::size_t j = 0; j < columns.size(); j++){
            if (j > 0){
                text += " ";
            }
            if (static_cast<std::size_t>(columns[j]) < rows[i].size()){
                text += rows[i][columns[j]];
            }
        }
        extractedText.push_back(text);
    }
    return extractedText;
}

