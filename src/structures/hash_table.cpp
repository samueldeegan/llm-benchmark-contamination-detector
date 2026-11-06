#include "hash_table.h"
#include <cstdint>


void HashTable::build(const std::vector<int>& corpus){
    this->corpus = &corpus;
    int size = this->corpus->size();
    std::vector<int> validWindowStarts;
    for (int i = 0; i <= size - WINDOW; i++){
        if (isValidWindow(i)){
            validWindowStarts.push_back(i);
        }
    }
    int validWindows = validWindowStarts.size();
    if (validWindows != 0){
        for (int i = 0; i <= validWindows*2; i++){
            this->table.push_back(-1);
        }
    }
    else{
        this->table.push_back(-1);
    }
    for (std::size_t i = 0; i < validWindowStarts.size(); i++){
        std::size_t calculatedTableSlot = hashWindow(corpus, validWindowStarts.at(i));
        if (this->table.at(calculatedTableSlot) == -1){
            this->table.at(calculatedTableSlot) = validWindowStarts.at(i);
        }
        else{
            for (std::size_t j = 0; j < this->table.size(); j++){
                std::size_t probingSlot = (calculatedTableSlot + j) % this->table.size();
                if (this->table.at(probingSlot) == -1){
                    this->table.at(probingSlot) = validWindowStarts.at(i);
                    break;
                }
                else if (windowsMatch(this->table.at(probingSlot), corpus, validWindowStarts.at(i))){
                    break;
                }
            }
        }
    }

}

bool HashTable::contains(const std::vector<int>& question, int start) const{
    for (int k = 0; k < WINDOW; k++){
        if (question.at(start+k) == UNKNOWN){
            return false;
        }
    }
    std::size_t calculatedTableSlot = hashWindow(question, start); 
    for (std::size_t j = 0; j < this->table.size(); j++){
        std::size_t probingSlot = (calculatedTableSlot + j) % this->table.size();
        if (this->table.at(probingSlot) == -1){
            return false;
        }
        else if (windowsMatch(this->table.at(probingSlot), question, start)){
            return true;
        }
    }
    return false;

}

std::size_t HashTable::bytesUsed() const{
    return this->table.size() * sizeof(int);
}

bool HashTable::isValidWindow(int pos) const {
    for (int j = pos; j < pos + WINDOW; j++){
        if ((*corpus)[j] == DOC_END){
            return false;
        }
    }
    return true;
}

bool HashTable::windowsMatch(int storedPos, const std::vector<int>& list, int pos) const {
    for (int i = 0; i < WINDOW; i++){
        if ((*corpus).at(storedPos + i) != list.at(pos + i)){
            return false;
        }
    }
    return true;
}

std::size_t HashTable::hashWindow(const std::vector<int>& list, int pos) const {
    std::uint64_t h = 0;
    for (int i = pos; i < pos + WINDOW; i++){
        h = h*31 + list.at(i);
    }
    return h % table.size();
}
