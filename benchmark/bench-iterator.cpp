#include <benchmark/benchmark.h>

#include <zip.h>

#include <vector>

static void BM_1DSubscriptSumVectorNoSIMD(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;

        for(std::size_t i = 0; i < std::size(x); ++i) {
            sum_x += x[i];
        }
        benchmark::DoNotOptimize(sum_x);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int)));
}
BENCHMARK(BM_1DSubscriptSumVectorNoSIMD)->Range(1<<0, 1<<10);


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

static void BM_2DSubscriptSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));
    const std::vector<int> y(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;
        int sum_y = 0;

        for(std::size_t i = 0; i < std::size(x); ++i) {
            sum_x += x[i];
            sum_y += y[i];
        }

        int return_sum_x = sum_x;
        int return_sum_y = sum_y;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int) * 2));
}
BENCHMARK(BM_2DSubscriptSumVector)->Range(1<<0, 1<<10);

static void BM_2DIteratorSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));
    const std::vector<int> y(state.range(0), state.range(0));

    for (auto _ : state) {
        int sum_x = 0;
        int sum_y = 0;

        auto it_x = std::cbegin(x);
        auto it_y = std::cbegin(y);
        const auto end = std::cend(x);

        while(it_x != end) {
            sum_x += *it_x++;
            sum_y += *it_y++;
        }

        int return_sum_x = sum_x;
        int return_sum_y = sum_y;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int) * 2));
}
BENCHMARK(BM_2DIteratorSumVector)->Range(1<<0, 1<<10);

static void BM_2DZipSumVector(benchmark::State& state) {
    const std::vector<int> x(state.range(0), state.range(0));
    const std::vector<int> y(state.range(0), state.range(0));

    for (auto _ : state) {

        int sum_x = 0;
        int sum_y = 0;

        const auto end = zip::zip_iterator{std::cend(x), std::cend(y)};

        for(auto it = zip::zip_iterator{std::cbegin(x), std::cbegin(y)}; it != end; ++it) {
            const auto [value_x, value_y] = *it; 
            sum_x += value_x;
            sum_y += value_y;
        }

        int return_sum_x = sum_x;
        int return_sum_y = sum_y;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
    }
    
    state.SetBytesProcessed(
        static_cast<int64_t>(state.iterations() * state.range(0) * sizeof(int) * 2));
}
BENCHMARK(BM_2DZipSumVector)->Range(1<<0, 1<<10);

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

static void BM_3DSubscriptSumVectorAoS(benchmark::State& state) {
    struct Item {
        int x;
        int y;
        int z;
    };
    int val = state.range(0);
    const std::vector<Item> v(state.range(0), {val, val, val});

    for (auto _ : state) {
        int sum_x = 0;
        int sum_y = 0;
        int sum_z = 0;

        for(std::size_t i = 0; i < std::size(v); ++i) {
            sum_x += v[i].x;
            sum_y += v[i].y;
            sum_z += v[i].z;
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
BENCHMARK(BM_3DSubscriptSumVectorAoS)->Range(1<<0, 1<<10);
