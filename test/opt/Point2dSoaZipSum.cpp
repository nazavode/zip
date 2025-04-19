// RUN: mkdir -p %t
// RUN: %cxx -march=skylake-avx512 -mtune=skylake-avx512 -O3 -DNDEBUG -std=c++17 -stdlib=libc++ \
// RUN:      -fsave-optimization-record -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | FileCheck %s

#include <vector>
#include <zip.h>

struct Result {
    int x;
    int y;
};

Result Point2dSoaZipSum(const std::vector<int>& x, const std::vector<int>& y) {
    int sum_x = 0;
    int sum_y = 0;

    const auto begin = zip::make_iterator(zip::offset_iterator_tag{}, std::begin(x), std::begin(y));
    const auto end = zip::make_iterator(zip::offset_iterator_tag{}, std::end(x), std::end(y));

    // CHECK: PASSED(loop-vectorize) Point2dSoaZipSum.cpp:22 VectorizationFactor=8 InterleaveCount=4
    for(auto it = begin; it != end; ++it) {
        const auto [value_x, value_y] = *it;
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}
