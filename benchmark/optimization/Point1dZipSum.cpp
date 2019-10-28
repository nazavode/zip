#include <vector>
#include <zip.h>

struct Result {
    int x;
};

extern "C"
Result Point1dZipSum(const std::vector<int>& x) {
    int sum_x = 0;

    const auto begin = zip::make_iterator(zip::offset_iterator_tag{}, std::begin(x));
    const auto end = zip::make_iterator(zip::offset_iterator_tag{}, std::end(x));

    for(auto it = begin; it != end; ++it) {
        const auto [value_x] = *it;
        sum_x += value_x;
    }

    return {sum_x};
}