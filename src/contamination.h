#pragma once
#include <cstddef>
#include <vector>

constexpr int WINDOW = 13;
constexpr int DOC_END = -1;
constexpr int UNKNOWN = -2;

class Structure{
    public:
        virtual ~Structure() = default;
        virtual void build(const std::vector<int>& corpus) = 0;
        virtual bool contains(const std::vector<int>& question, int start) const = 0;
        virtual std::size_t bytesUsed() const = 0;
};