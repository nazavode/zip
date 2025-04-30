// RUN: mkdir -p %t
// RUN: %cxx -O3 -ffast-math -DNDEBUG -std=c++17 -fsave-optimization-record \
// RUN:      -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | %filecheck %s

#include <zip.h>

#include <vector>

template <typename T>
struct Result {
    T x;
    T y;
};

Result<int> SumReduce2dInt(const std::vector<int>& x, const std::vector<int>& y) {
    int sum_x = 0;
    int sum_y = 0;

    // zip_iterator inferred category is random_access, clang is unable
    // to vectorize while keeping track of 2 independent iterators:
    // CHECK: MISSED(loop-vectorize) SumReduce2d.cpp:23
    for (auto [value_x, value_y] : zip::zip(x, y)) {
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}

Result<int> SumReduce2dIntOffset(const std::vector<int>& x, const std::vector<int>& y) {
    int sum_x = 0;
    int sum_y = 0;

    // offset_iterator must be requested explicitly, but allows
    // clang to vectorize since it advances all iterators using
    // a single induction variable:
    // CHECK: PASSED(loop-vectorize) SumReduce2d.cpp:39
    for (auto [value_x, value_y] : zip::zip(zip::offset_iterator_tag{}, x, y)) {
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}

Result<float> SumReduce2dFloat(const std::vector<float>& x, const std::vector<float>& y) {
    float sum_x = 0.f;
    float sum_y = 0.f;

    // zip_iterator inferred category is random_access, clang is unable
    // to vectorize while keeping track of 2 independent iterators:
    // CHECK: MISSED(loop-vectorize) SumReduce2d.cpp:54
    for (auto [value_x, value_y] : zip::zip(x, y)) {
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}

Result<float> SumReduce2dFloatOffset(const std::vector<float>& x,
                                     const std::vector<float>& y) {
    float sum_x = 0.f;
    float sum_y = 0.f;

    // offset_iterator must be requested explicitly, but allows
    // clang to vectorize since it advances all iterators using
    // a single induction variable:
    // CHECK: PASSED(loop-vectorize) SumReduce2d.cpp:71
    for (auto [value_x, value_y] : zip::zip(zip::offset_iterator_tag{}, x, y)) {
        sum_x += value_x;
        sum_y += value_y;
    }

    return {sum_x, sum_y};
}
