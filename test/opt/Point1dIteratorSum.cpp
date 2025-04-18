// RUN: mkdir -p %t
// RUN: %cxx -march=skylake-avx512 -mtune=skylake-avx512 -O3 -DNDEBUG -std=c++17 -stdlib=libc++ \
// RUN:      -fsave-optimization-record -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | FileCheck %s

#include <vector>

int Point1dIteratorSum(const std::vector<int>& x) {
    int sum_x = 0;
    // CHECK: PASSED(loop-vectorize) Point1dIteratorSum.cpp:11 VectorizationFactor=8 InterleaveCount=4
    for(auto it = std::begin(x); it != std::end(x); ++it) {
        sum_x += *it;
    }
    return sum_x;
}
