#include <vector>
#include <zip.h>

struct Result {
    int x;
    int y;
    int z;
    int w;
};

extern "C"
Result Point4dSoaZipSum(const std::vector<int>& x, const std::vector<int>& y,
                        const std::vector<int>& z, const std::vector<int>& w) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;
    int sum_w = 0;

    const auto begin =
        zip::zip_iterator{std::begin(x), std::begin(y), std::begin(z), std::begin(w)};
    
    const auto end =
        zip::zip_iterator{std::end(x), std::end(y), std::end(z), std::end(w)};

    for(auto it = begin; it != end; ++it) {
        const auto [value_x, value_y, value_z, value_w] = *it;
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
        sum_w += value_w;
    }

    return {sum_x, sum_y, sum_z, sum_w};
}