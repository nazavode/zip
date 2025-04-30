// RUN: mkdir -p %t
// RUN: %cxx -O3 -ffast-math -DNDEBUG -std=c++17 -fsave-optimization-record \
// RUN:      -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | %filecheck %s

#include <zip.h>

#include <vector>

// Smoke tests: if we cannot auto-vectorize these, we are in big troubles.

int SumReduce1dInt(const std::vector<int>& x) {
    int sum = 0;
    // CHECK: PASSED(loop-vectorize) SumReduce1d.cpp:15
    for (auto it = std::cbegin(x); it != std::cend(x); ++it) {
        sum += *it;
    }
    return sum;
}

int SumReduce1dFloat(const std::vector<float>& x) {
    float sum = 0.f;
    // CHECK: PASSED(loop-vectorize) SumReduce1d.cpp:24
    for (auto it = std::cbegin(x); it != std::cend(x); ++it) {
        sum += *it;
    }
    return sum;
}

// Zip

int SumReduce1dIntZip(const std::vector<int>& x) {
    int sum = 0;
    // CHECK: PASSED(loop-vectorize) SumReduce1d.cpp:35
    for (auto [value] : zip::zip(x)) {
        sum += value;
    }
    return sum;
}

float SumReduce1dFloatZip(const std::vector<float>& x) {
    float sum = 0.f;
    // CHECK: PASSED(loop-vectorize) SumReduce1d.cpp:44
    for (auto [value] : zip::zip(x)) {
        sum += value;
    }
    return sum;
}
