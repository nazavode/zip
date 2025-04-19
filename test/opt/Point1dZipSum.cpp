// RUN: mkdir -p %t
// RUN: %cxx -march=skylake-avx512 -mtune=skylake-avx512 -O3 -DNDEBUG -std=c++17 -stdlib=libc++ \
// RUN:      -fsave-optimization-record -foptimization-record-file=%t/remarks.yaml -o %t/out.o -c %s
// RUN: %opt-report-summary < %t/remarks.yaml | FileCheck %s

#include <vector>
#include <zip.h>

struct Result {
    int x;
};

Result Point1dZipSum(const std::vector<int>& x) {
    int sum_x = 0;

    const auto begin = zip::make_iterator(zip::offset_iterator_tag{}, std::begin(x));
    const auto end = zip::make_iterator(zip::offset_iterator_tag{}, std::end(x));

    // CHECK: PASSED(loop-vectorize) Point1dZipSum.cpp:20 VectorizationFactor=8 InterleaveCount=4
    for(auto it = begin; it != end; ++it) {
        const auto [value_x] = *it;
        sum_x += value_x;
    }

    return {sum_x};
}
