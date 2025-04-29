// RUN: mkdir -p %t
// RUN: %cxx -O3 -ffast-math -DNDEBUG -std=c++17 -fsave-optimization-record \
// RUN:      -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | FileCheck %s

#include <vector>
#include <zip.h>

template<typename T>
struct Result {
    T x;
    T y;
};

Result<int> SumReduce2dIntOffset(const std::vector<int>& x, const std::vector<int>& y) {
    int sum_x = 0;
    int sum_y = 0;

    const auto begin = zip::make_iterator(zip::offset_iterator_tag{}, std::cbegin(x), std::cbegin(y));
    const auto end = zip::make_iterator(zip::offset_iterator_tag{}, std::cend(x), std::cend(y));

    // CHECK: PASSED(loop-vectorize) SumReduce2d.cpp:23
    for(auto it = begin; it != end; ++it) {
        const auto [value_x, value_y] = *it;
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}

Result<float> SumReduce2dFloatOffset(const std::vector<float>& x, const std::vector<float>& y) {
    float sum_x = 0.f;
    float sum_y = 0.f;

    const auto begin = zip::make_iterator(zip::offset_iterator_tag{}, std::cbegin(x), std::cbegin(y));
    const auto end = zip::make_iterator(zip::offset_iterator_tag{}, std::cend(x), std::cend(y));

    // CHECK: PASSED(loop-vectorize) SumReduce2d.cpp:40
    for(auto it = begin; it != end; ++it) {
        const auto [value_x, value_y] = *it;
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}
