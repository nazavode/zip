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
    T z;
    T w;
};

Result<int> SumReduce4dInt(const std::vector<int>& x, const std::vector<int>& y,
                           const std::vector<int>& z, const std::vector<int>& w) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;
    int sum_w = 0;

    // zip_iterator inferred category is random_access, clang is unable
    // to vectorize while keeping track of 4 parallel iterators:
    // CHECK: MISSED(loop-vectorize) SumReduce4d.cpp:28
    for (auto [value_x, value_y, value_z, value_w] : zip::zip(x, y, z, w)) {
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
        sum_z += value_w;
    }

    return {sum_x, sum_y, sum_z, sum_w};
}

Result<int> SumReduce4dIntOffset(const std::vector<int>& x, const std::vector<int>& y,
                                 const std::vector<int>& z, const std::vector<int>& w) {
    int sum_x = 0;
    int sum_y = 0;
    int sum_z = 0;
    int sum_w = 0;

    // offset_iterator must be requested explicitly, but allows
    // clang to vectorize since it advances all iterators using
    // a single induction variable:
    // CHECK: PASSED(loop-vectorize) SumReduce4d.cpp:49
    for (auto [value_x, value_y, value_z, value_w] :
         zip::zip(zip::offset_iterator_tag{}, x, y, z, w)) {
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
        sum_z += value_w;
    }

    return {sum_x, sum_y, sum_z, sum_w};
}

Result<float> SumReduce4dFloat(const std::vector<float>& x, const std::vector<float>& y,
                               const std::vector<float>& z, const std::vector<float>& w) {
    float sum_x = 0.f;
    float sum_y = 0.f;
    float sum_z = 0.f;
    float sum_w = 0.f;

    // zip_iterator inferred category is random_access, clang is unable
    // to vectorize while keeping track of 4 parallel iterators:
    // CHECK: MISSED(loop-vectorize) SumReduce4d.cpp:70
    for (auto [value_x, value_y, value_z, value_w] : zip::zip(x, y, z, w)) {
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
        sum_z += value_w;
    }

    return {sum_x, sum_y, sum_z, sum_w};
}

Result<float> SumReduce4dFloatOffset(const std::vector<float>& x,
                                     const std::vector<float>& y,
                                     const std::vector<float>& z,
                                     const std::vector<float>& w) {
    float sum_x = 0.f;
    float sum_y = 0.f;
    float sum_z = 0.f;
    float sum_w = 0.f;

    // offset_iterator must be requested explicitly, but allows
    // clang to vectorize since it advances all iterators using
    // a single induction variable:
    // CHECK: PASSED(loop-vectorize) SumReduce4d.cpp:93
    for (auto [value_x, value_y, value_z, value_w] :
         zip::zip(zip::offset_iterator_tag{}, x, y, z, w)) {
        sum_x += value_x;
        sum_y += value_y;
        sum_z += value_z;
        sum_z += value_w;
    }

    return {sum_x, sum_y, sum_z, sum_w};
}
