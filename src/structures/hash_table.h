#pragma once
#include "contamination.h"

class HashTable: public Structure {
    public:
        void build(const std::vector<int>& corpus) override;
        bool contains(const std::vector<int>& question, int start) const override;
        std::size_t bytesUsed() const override;
    private:
        std::vector<int> table;
        const std::vector<int>* corpus = nullptr;
        bool isValidWindow(int pos) const;
        bool windowsMatch(int storedPos, const std::vector<int>& list, int pos) const;
        std::size_t hashWindow(const std::vector<int>& list, int pos) const;

};