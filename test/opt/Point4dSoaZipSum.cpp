// RUN: mkdir -p %t
// RUN: %cxx -march=skylake-avx512 -mtune=skylake-avx512 -O3 -DNDEBUG -std=c++17 -stdlib=libc++ \
// RUN:      -fsave-optimization-record -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | FileCheck %s

#include <vector>
#include <zip.h>

struct Result {
    int x;
    int y;
    int z;
    int w;
};

Result Point4dSoaZipSum(const std::vector<int>& x, const std::vector<int>& y,
                        const std::vector<int>& z, const std::vector<int>& w) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;
    int sum_w = 0;

    const auto begin =
        zip::make_iterator(zip::offset_iterator_tag{}, std::begin(x), std::begin(y), std::begin(z), std::begin(w));
    
    const auto end =
        zip::make_iterator(zip::offset_iterator_tag{}, std::end(x), std::end(y), std::end(z), std::end(w));

    // CHECK: PASSED(loop-vectorize) Point4dSoaZipSum.cpp:30 VectorizationFactor=8 InterleaveCount=2
    for(auto it = begin; it != end; ++it) {
        const auto [value_x, value_y, value_z, value_w] = *it;
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
        sum_w += value_w;
    }

    return {sum_x, sum_y, sum_z, sum_w};
}
