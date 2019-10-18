#include <vector>

struct Result {
    int x;
};

extern "C"
Result Point1dIteratorSum(const std::vector<int>& x) {
    int sum_x = 0;

    for(auto it = std::begin(x); it != std::end(x); ++it) {
        sum_x += *it;
    }

    return {sum_x};
}