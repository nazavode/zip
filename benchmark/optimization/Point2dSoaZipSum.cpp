#include <vector>
#include <zip.h>

struct Result {
    int x;
    int y;
};

extern "C"
Result Point2dSoaZipSum(const std::vector<int>& x, const std::vector<int>& y) {
    int sum_x = 0;
    int sum_y = 0;

    const auto begin = zip::make_iterator(std::begin(x), std::begin(y));
    const auto end = zip::make_iterator(std::end(x), std::end(y));

    for(auto it = begin; it != end; ++it) {
        const auto [value_x, value_y] = *it;
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}