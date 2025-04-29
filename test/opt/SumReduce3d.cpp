// RUN: mkdir -p %t
// RUN: %cxx -O3 -ffast-math -DNDEBUG -std=c++17 -fsave-optimization-record \
// RUN:      -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | FileCheck %s

#include <zip.h>

#include <vector>

template <typename T>
struct Result {
    T x;
    T y;
    T z;
};

Result<int> SumReduce3dIntOffset(const std::vector<int>& x, const std::vector<int>& y,
                                 const std::vector<int>& z) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;

    const auto begin = zip::make_iterator(zip::offset_iterator_tag{}, std::cbegin(x),
                                          std::cbegin(y), std::cbegin(z));
    const auto end = zip::make_iterator(zip::offset_iterator_tag{}, std::cend(x),
                                        std::cend(y), std::cend(z));

    // CHECK: PASSED(loop-vectorize) SumReduce3d.cpp:29
    for (auto it = begin; it != end; ++it) {
        const auto [value_x, value_y, value_z] = *it;
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
    }

    return {sum_x, sum_y, sum_z};
}

Result<float> SumReduce3dFloatOffset(const std::vector<float>& x,
                                     const std::vector<float>& y,
                                     const std::vector<float>& z) {
    float sum_x = 0.f;
    float sum_y = 0.f;
    float sum_z = 0.f;

    const auto begin = zip::make_iterator(zip::offset_iterator_tag{}, std::cbegin(x),
                                          std::cbegin(y), std::cbegin(z));
    const auto end = zip::make_iterator(zip::offset_iterator_tag{}, std::cend(x),
                                        std::cend(y), std::cend(z));

    // CHECK: PASSED(loop-vectorize) SumReduce3d.cpp:52
    for (auto it = begin; it != end; ++it) {
        const auto [value_x, value_y, value_z] = *it;
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
    }

    return {sum_x, sum_y, sum_z};
}
