#include <benchmark/benchmark.h>

#include <zip.h>

#include <vector>

static void BM_1DSubscriptSumVector_BaselineNoSIMD(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;

        #pragma clang loop vectorize(disable)
        for(std::size_t i = 0; i < std::size(x); ++i) {
            sum_x += x[i];
        }
        int return_sum_x = sum_x;
        benchmark::DoNotOptimize(return_sum_x);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int)));
}
BENCHMARK(BM_1DSubscriptSumVector_BaselineNoSIMD)->Range(1<<0, 1<<10);


static void BM_1DSubscriptSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;

        for(std::size_t i = 0; i < std::size(x); ++i) {
            sum_x += x[i];
        }
        int return_sum_x = sum_x;
        benchmark::DoNotOptimize(return_sum_x);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int)));
}
BENCHMARK(BM_1DSubscriptSumVector)->Range(1<<0, 1<<10);

static void BM_1DIteratorSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;

        auto it_x = std::cbegin(x);
        
        const auto end = std::cend(x);

        while(it_x != end) {
            sum_x += *it_x++;
        }

        int return_sum_x = sum_x;
        benchmark::DoNotOptimize(return_sum_x);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int)));
}
BENCHMARK(BM_1DIteratorSumVector)->Range(1<<0, 1<<10);


static void BM_1DZipSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));

    for (auto _ : state) {

        int sum_x = 0;

        const auto end = zip::zip_iterator{std::cend(x)};

        for(auto it = zip::zip_iterator{std::cbegin(x)}; it != end; ++it) {
            const auto [value_x] = *it; 
            sum_x += value_x;
        }

        int return_sum_x = sum_x;
        benchmark::DoNotOptimize(return_sum_x);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int)));
}
BENCHMARK(BM_1DZipSumVector)->Range(1<<0, 1<<10);

static void BM_3DSubscriptSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));
    const std::vector<int> y(state.range(0), state.range(0));
    const std::vector<int> z(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;
        int sum_y = 0;
        int sum_z = 0;

        for(std::size_t i = 0; i < std::size(x); ++i) {
            sum_x += x[i];
            sum_y += y[i];
            sum_z += z[i];
        }

        int return_sum_x = sum_x;
        int return_sum_y = sum_y;
        int return_sum_z = sum_z;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
        benchmark::DoNotOptimize(return_sum_z);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int) * 3));
}
BENCHMARK(BM_3DSubscriptSumVector)->Range(1<<0, 1<<10);

static void BM_3DIteratorSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));
    const std::vector<int> y(state.range(0), state.range(0));
    const std::vector<int> z(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;
        int sum_y = 0;
        int sum_z = 0;

        auto it_x = std::cbegin(x);
        auto it_y = std::cbegin(y);
        auto it_z = std::cbegin(z);
        const auto end = std::cend(x);

        while(it_x != end) {
            sum_x += *it_x++;
            sum_y += *it_y++;
            sum_z += *it_z++;
        }

        int return_sum_x = sum_x;
        int return_sum_y = sum_y;
        int return_sum_z = sum_z;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
        benchmark::DoNotOptimize(return_sum_z);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int) * 3));
}
BENCHMARK(BM_3DIteratorSumVector)->Range(1<<0, 1<<10);

static void BM_3DZipSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));
    const std::vector<int> y(state.range(0), state.range(0));
    const std::vector<int> z(state.range(0), state.range(0));

    for (auto _ : state) {

        int sum_x = 0;
        int sum_y = 0;
        int sum_z = 0;

        const auto end = zip::zip_iterator{std::cend(x), std::cend(y), std::cend(z)};

        for(auto it = zip::zip_iterator{std::cbegin(x), std::cbegin(y), std::cbegin(z)}; it != end; ++it) {
            const auto [value_x, value_y, value_z] = *it; 
            sum_x += value_x;
            sum_y += value_y;
            sum_z += value_z;
        }

        int return_sum_x = sum_x;
        int return_sum_y = sum_y;
        int return_sum_z = sum_z;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
        benchmark::DoNotOptimize(return_sum_z);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int) * 3));
}
BENCHMARK(BM_3DZipSumVector)->Range(1<<0, 1<<10);