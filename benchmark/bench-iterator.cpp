#include <benchmark/benchmark.h>

#include <zip.h>

#include <numeric>
#include <vector>

class Point3DSoA : public ::benchmark::Fixture {
   public:
   
    using value_type = int;
    using container_type = std::vector<value_type>;

    void SetUp(const ::benchmark::State& state) {
        x.resize(state.range(0));
        y.resize(state.range(0));
        z.resize(state.range(0));
        std::iota(std::begin(x), std::end(x), value_type{});
        std::iota(std::begin(y), std::end(y), value_type{});
        std::iota(std::begin(z), std::end(z), value_type{});
    }

    void TearDown(::benchmark::State& state) {
        state.SetBytesProcessed(
            static_cast<int64_t>(
                state.iterations() * state.range(0) * sizeof(value_type) * 3));
        x.clear();
        y.clear();
        z.clear();
    }
    
    constexpr auto size() const noexcept {
        return std::size(x);
    }

    constexpr auto cbegin() const noexcept {
        return zip::zip_iterator{std::cbegin(x), std::cbegin(y), std::cbegin(z)};
    }

    constexpr auto cend() const noexcept {
        return zip::zip_iterator{std::cend(x), std::cend(y), std::cend(z)};
    }

    container_type x;
    container_type y;
    container_type z;
};

// Baseline for 3d points summation:
// accumulator variables have been clobberedd to prevent
// any kind of vectorization
BENCHMARK_DEFINE_F(Point3DSoA, Baseline)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        for(decltype(size()) i = 0; i < size(); ++i) {
            sum_x += x[i];
            sum_y += y[i];
            sum_z += z[i];
        }

        benchmark::DoNotOptimize(sum_x);
        benchmark::DoNotOptimize(sum_y);
        benchmark::DoNotOptimize(sum_z);
    }
}
BENCHMARK_REGISTER_F(Point3DSoA, Baseline)->Range(1<<0, 1<<10);

BENCHMARK_DEFINE_F(Point3DSoA, SubscriptSum)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        for(decltype(size()) i = 0; i < size(); ++i) {
            sum_x += x[i];
            sum_y += y[i];
            sum_z += z[i];
        }

        auto return_sum_x = sum_x;
        auto return_sum_y = sum_y;
        auto return_sum_z = sum_z;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
        benchmark::DoNotOptimize(return_sum_z);
    }
}
BENCHMARK_REGISTER_F(Point3DSoA, SubscriptSum)->Range(1<<0, 1<<10);

BENCHMARK_DEFINE_F(Point3DSoA, IteratorSum)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        auto it_x = std::cbegin(x);
        auto it_y = std::cbegin(y);
        auto it_z = std::cbegin(z);
        const auto end = std::cend(x);

        while(it_x != end) {
            sum_x += *it_x++;
            sum_y += *it_y++;
            sum_z += *it_z++;
        }

        auto return_sum_x = sum_x;
        auto return_sum_y = sum_y;
        auto return_sum_z = sum_z;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
        benchmark::DoNotOptimize(return_sum_z);
    }
}
BENCHMARK_REGISTER_F(Point3DSoA, IteratorSum)->Range(1<<0, 1<<10);

BENCHMARK_DEFINE_F(Point3DSoA, ZipSum)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        for(auto it = cbegin(); it != cend(); ++it) {
            const auto [value_x, value_y, value_z] = *it;
            sum_x += value_x;
            sum_y += value_y;
            sum_z += value_z;
        }

        auto return_sum_x = sum_x;
        auto return_sum_y = sum_y;
        auto return_sum_z = sum_z;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
        benchmark::DoNotOptimize(return_sum_z);
    }
}
BENCHMARK_REGISTER_F(Point3DSoA, ZipSum)->Range(1<<0, 1<<10);

////////////////////////////////////////////////////////////////////

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
