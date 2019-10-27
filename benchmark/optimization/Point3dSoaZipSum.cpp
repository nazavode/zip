#include <vector>
#include <zip.h>

struct Result {
    int x;
    int y;
    int z;
};

extern "C"
Result Point3dSoaZipSum(const std::vector<int>& x, const std::vector<int>& y, const std::vector<int>& z) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;

    const auto begin = zip::make_iterator(zip::offset, std::begin(x), std::begin(y), std::begin(z));
    const auto end = zip::make_iterator(zip::offset, std::end(x), std::end(y), std::end(z));

    for(auto it = begin; it != end; ++it) {
        const auto [value_x, value_y, value_z] = *it;
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
    }

    return {sum_x, sum_y, sum_z};
}