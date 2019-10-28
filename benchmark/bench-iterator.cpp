#include <benchmark/benchmark.h>
#include <zip.h>

#include <algorithm>
#include <array>
#include <numeric>
#include <vector>

template <typename T, std::size_t N>
class SoA : public ::benchmark::Fixture {
   public:
    using value_type = T;
    using container_type = std::vector<T>;

    void SetUp(const ::benchmark::State& state) {
        const auto size = static_cast<std::size_t>(state.range(0));
        std::for_each(std::begin(data), std::end(data), [size](auto&& c) {
            c.resize(size);
            std::iota(std::begin(c), std::end(c), value_type{});
        });
    }

    void TearDown(::benchmark::State& state) {
        state.SetBytesProcessed(static_cast<std::int64_t>(
            state.iterations() * static_cast<std::uint64_t>(state.range(0)) *
            sizeof(value_type) * N));
        std::for_each(std::begin(data), std::end(data), [](auto&& c) { c.clear(); });
    }

    constexpr auto size() const noexcept { return std::size(data[0]); }

    constexpr auto cbegin() const noexcept {
        return cbegin_impl(std::make_index_sequence<N>{});
    }

    template <std::size_t... Indexes>
    constexpr auto cbegin_impl(std::index_sequence<Indexes...>) const noexcept {
        return zip::make_iterator(zip::offset_iterator_tag{}, std::cbegin(data[Indexes])...);
    }

    constexpr auto cend() const noexcept {
        return cend_impl(std::make_index_sequence<N>{});
    }

    template <std::size_t... Indexes>
    constexpr auto cend_impl(std::index_sequence<Indexes...>) const noexcept {
        return zip::make_iterator(zip::offset_iterator_tag{}, std::cend(data[Indexes])...);
    }

    std::array<container_type, N> data;
};

///////////////////////////////////////////////////////////////////////////////
using SoA_Int32_1D = SoA<std::int32_t, 1>;
///////////////////////////////////////////////////////////////////////////////

BENCHMARK_DEFINE_F(SoA_Int32_1D, SumBaseline)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};

        for (decltype(size()) i = 0; i < size(); ++i) {
            sum_x += data[0][i];
        }

        benchmark::DoNotOptimize(sum_x);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_1D, SumBaseline)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_1D, SumSubscript)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};

        for (decltype(size()) i = 0; i < size(); ++i) {
            sum_x += data[0][i];
        }

        auto return_sum_x = sum_x;
        benchmark::DoNotOptimize(return_sum_x);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_1D, SumSubscript)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_1D, SumIterator)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};

        auto it_x = std::cbegin(data[0]);
        const auto end = std::cend(data[0]);

        while (it_x != end) {
            sum_x += *it_x++;
        }

        auto return_sum_x = sum_x;
        benchmark::DoNotOptimize(return_sum_x);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_1D, SumIterator)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_1D, SumZip)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};

        for (auto it = cbegin(); it != cend(); ++it) {
            const auto [value_x] = *it;
            sum_x += value_x;
        }

        auto return_sum_x = sum_x;
        benchmark::DoNotOptimize(return_sum_x);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_1D, SumZip)->Range(1 << 0, 1 << 10);

///////////////////////////////////////////////////////////////////////////////
using SoA_Int32_2D = SoA<std::int32_t, 2>;
///////////////////////////////////////////////////////////////////////////////

BENCHMARK_DEFINE_F(SoA_Int32_2D, SumBaseline)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};

        for (decltype(size()) i = 0; i < size(); ++i) {
            sum_x += data[0][i];
            sum_y += data[1][i];
        }

        benchmark::DoNotOptimize(sum_x);
        benchmark::DoNotOptimize(sum_y);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_2D, SumBaseline)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_2D, SumSubscript)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};

        for (decltype(size()) i = 0; i < size(); ++i) {
            sum_x += data[0][i];
            sum_y += data[1][i];
        }

        auto return_sum_x = sum_x;
        auto return_sum_y = sum_y;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_2D, SumSubscript)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_2D, SumIterator)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};

        auto it_x = std::cbegin(data[0]);
        auto it_y = std::cbegin(data[1]);
        const auto end = std::cend(data[0]);

        while (it_x != end) {
            sum_x += *it_x++;
            sum_y += *it_y++;
        }

        auto return_sum_x = sum_x;
        auto return_sum_y = sum_y;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_2D, SumIterator)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_2D, SumZip)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};

        for (auto it = cbegin(); it != cend(); ++it) {
            const auto [value_x, value_y] = *it;
            sum_x += value_x;
            sum_y += value_y;
        }

        auto return_sum_x = sum_x;
        auto return_sum_y = sum_y;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_2D, SumZip)->Range(1 << 0, 1 << 10);

///////////////////////////////////////////////////////////////////////////////
using SoA_Int32_3D = SoA<std::int32_t, 3>;
///////////////////////////////////////////////////////////////////////////////

// Baseline for 3d points summation:
// accumulator variables have been clobberedd to prevent
// any kind of vectorization
BENCHMARK_DEFINE_F(SoA_Int32_3D, SumBaseline)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        for (decltype(size()) i = 0; i < size(); ++i) {
            sum_x += data[0][i];
            sum_y += data[1][i];
            sum_z += data[2][i];
        }

        benchmark::DoNotOptimize(sum_x);
        benchmark::DoNotOptimize(sum_y);
        benchmark::DoNotOptimize(sum_z);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_3D, SumBaseline)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_3D, SumSubscript)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        for (decltype(size()) i = 0; i < size(); ++i) {
            sum_x += data[0][i];
            sum_y += data[1][i];
            sum_z += data[2][i];
        }

        auto return_sum_x = sum_x;
        auto return_sum_y = sum_y;
        auto return_sum_z = sum_z;
        benchmark::DoNotOptimize(return_sum_x);
        benchmark::DoNotOptimize(return_sum_y);
        benchmark::DoNotOptimize(return_sum_z);
    }
}
BENCHMARK_REGISTER_F(SoA_Int32_3D, SumSubscript)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_3D, SumIterator)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        auto it_x = std::cbegin(data[0]);
        auto it_y = std::cbegin(data[1]);
        auto it_z = std::cbegin(data[2]);
        const auto end = std::cend(data[0]);

        while (it_x != end) {
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
BENCHMARK_REGISTER_F(SoA_Int32_3D, SumIterator)->Range(1 << 0, 1 << 10);

BENCHMARK_DEFINE_F(SoA_Int32_3D, SumZip)(benchmark::State& state) {
    for (auto _ : state) {
        auto sum_x = value_type{};
        auto sum_y = value_type{};
        auto sum_z = value_type{};

        for (auto it = cbegin(); it != cend(); ++it) {
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
BENCHMARK_REGISTER_F(SoA_Int32_3D, SumZip)->Range(1 << 0, 1 << 10);

////////////////////////////////////////////////////////////////////

class AoS_Int32_3D : public ::benchmark::Fixture {};

BENCHMARK_DEFINE_F(AoS_Int32_3D, SumSubscript)(benchmark::State& state) {
    struct Item {
        int x;
        int y;
        int z;
    };
    const auto val = static_cast<int>(state.range(0));
    const std::vector<Item> v(static_cast<std::size_t>(state.range(0)), {val, val, val});

    for (auto _ : state) {
        int sum_x = 0;
        int sum_y = 0;
        int sum_z = 0;

        for (std::size_t i = 0; i < std::size(v); ++i) {
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

    state.SetBytesProcessed(static_cast<std::int64_t>(
        state.iterations() * static_cast<std::uint64_t>(state.range(0)) * sizeof(int) *
        3U));
}
BENCHMARK_REGISTER_F(AoS_Int32_3D, SumSubscript)->Range(1 << 0, 1 << 10);
