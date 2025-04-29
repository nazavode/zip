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

Result<int> SumReduce3dInt(const std::vector<int>& x, const std::vector<int>& y,
                           const std::vector<int>& z) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;

    // zip_iterator inferred category is random_access, clang is unable
    // to vectorize while keeping track of 3 independent iterators:
    // CHECK: MISSED(loop-vectorize) SumReduce3d.cpp:26
    for (auto [value_x, value_y, value_z] : zip::zip(x, y, z)) {
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
    }

    return {sum_x, sum_y, sum_z};
}

Result<int> SumReduce3dIntOffset(const std::vector<int>& x, const std::vector<int>& y,
                                 const std::vector<int>& z) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;

    // offset_iterator must be requested explicitly, but allows
    // clang to vectorize since it advances all iterators using
    // a single induction variable:
    // CHECK: PASSED(loop-vectorize) SumReduce3d.cpp:45
    for (auto [value_x, value_y, value_z] :
         zip::zip(zip::offset_iterator_tag{}, x, y, z)) {
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
    }

    return {sum_x, sum_y, sum_z};
}

Result<float> SumReduce3dFloat(const std::vector<float>& x, const std::vector<float>& y,
                               const std::vector<float>& z) {
    float sum_x = 0.f;
    float sum_y = 0.f;
    float sum_z = 0.f;

    // zip_iterator inferred category is random_access, clang is unable
    // to vectorize while keeping track of 3 independent iterators:
    // CHECK: MISSED(loop-vectorize) SumReduce3d.cpp:64
    for (auto [value_x, value_y, value_z] : zip::zip(x, y, z)) {
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

    // offset_iterator must be requested explicitly, but allows
    // clang to vectorize since it advances all iterators using
    // a single induction variable:
    // CHECK: PASSED(loop-vectorize) SumReduce3d.cpp:84
    for (auto [value_x, value_y, value_z] :
         zip::zip(zip::offset_iterator_tag{}, x, y, z)) {
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
    }

    return {sum_x, sum_y, sum_z};
}
