#include <zip.h>

#include "catch.hpp"

#include <array>
#include <type_traits>
#include <tuple>

template<typename T>
constexpr bool is_const = std::is_const_v<std::remove_reference_t<T>>;

TEST_CASE("zip_iterator has reference semantics", "[zip_iterator]") {
    std::array<int, 0> a;
    std::array<long long, 0> b;
    auto it = zip::zip_iterator{std::begin(a), std::begin(b)};

    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<decltype(it)>);
    STATIC_REQUIRE(std::is_copy_constructible_v<decltype(it)>);
    STATIC_REQUIRE(std::is_copy_assignable_v<decltype(it)>);
    STATIC_REQUIRE(std::is_move_constructible_v<decltype(it)>);
    STATIC_REQUIRE(std::is_move_assignable_v<decltype(it)>);
    STATIC_REQUIRE(std::is_destructible_v<decltype(it)>);
    STATIC_REQUIRE(std::is_swappable_v<decltype(it)>);
}

TEST_CASE("zip_iterator has tuple-like semantics", "[zip_iterator]") {
    std::array<int, 0> a;
    std::array<long long, 0> b;

    auto it = zip::zip_iterator{std::begin(a), std::begin(b)};

    SECTION("std::tuple_size") {
        REQUIRE(std::tuple_size_v<decltype(it)> == 2);
    }

    SECTION("std::tuple_element") {
        STATIC_REQUIRE(
            std::is_same_v<
                std::tuple_element_t<0, decltype(it)>,
                decltype(std::begin(a))>);
        STATIC_REQUIRE(
            std::is_same_v<
                std::tuple_element_t<1, decltype(it)>,
                decltype(std::begin(b))>);
    }

    SECTION("element access") {
        SECTION("via qualified std::get") {
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(std::get<0>(it))>,
                    decltype(std::begin(a))>);
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(std::get<1>(it))>,
                    decltype(std::begin(b))>);
        }

        SECTION("via unqualified get") {
            using std::get;
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(get<0>(it))>,
                    decltype(std::begin(a))>);
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(get<1>(it))>,
                    decltype(std::begin(b))>);
        }

        SECTION("via member get") {
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(it.get<0>())>,
                    decltype(std::begin(a))>);
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(it.get<1>())>,
                    decltype(std::begin(b))>);
        }

        SECTION("via structured binding") {
            auto&& [a_bind, b_bind] = it;
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(a_bind)>,
                    decltype(std::begin(a))>);
            STATIC_REQUIRE(
                std::is_same_v<
                    std::remove_reference_t<decltype(b_bind)>,
                    decltype(std::begin(b))>);
        }
    }
}

TEST_CASE("zipped iterators constness is preserved", "[zip_iterator]") {
    std::array<int, 0> a;
    std::array<long long, 0> b;

    auto const_const = zip::zip_iterator{std::cbegin(a), std::cbegin(b)};
    STATIC_REQUIRE(is_const<decltype(*std::get<0>(const_const))>);
    STATIC_REQUIRE(is_const<decltype(*std::get<1>(const_const))>);

    auto mut_const = zip::zip_iterator{std::begin(a), std::cbegin(b)};
    STATIC_REQUIRE_FALSE(is_const<decltype(*std::get<0>(mut_const))>);
    STATIC_REQUIRE(is_const<decltype(*std::get<1>(mut_const))>);

    auto const_mut = zip::zip_iterator{std::cbegin(a), std::begin(b)};
    STATIC_REQUIRE(is_const<decltype(*std::get<0>(const_mut))>);
    STATIC_REQUIRE_FALSE(is_const<decltype(*std::get<1>(const_mut))>);

    auto mut_mut = zip::zip_iterator{std::begin(a), std::begin(b)};
    STATIC_REQUIRE_FALSE(is_const<decltype(*std::get<0>(mut_mut))>);
    STATIC_REQUIRE_FALSE(is_const<decltype(*std::get<1>(mut_mut))>);
}

TEST_CASE("zip_iterator abides by random access iterator contract", "[zip_iterator]") {
    std::array<int, 6>   a{0,  1,  2,  3,  4,   5};
    std::array<long long, 6> b{4,  3,  2,  1,  0, 666};
    std::array<signed char, 10>  c{0, -1, -2, -3, -4,  -5, -6, -7, -8, -9};

    auto begin = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
    auto end = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};

    SECTION("operator-(const zip_iterator&)") {
        REQUIRE(end - begin == 6);
    }

    SECTION("std::distance(const zip_iterator&, const zip_iterator&)") {
        REQUIRE(std::distance(begin, end) == 6);
    }

    SECTION("operator+(difference_type)") {
        auto it = begin + 1;
        REQUIRE(std::get<0>(it)[2] == 3);
        REQUIRE(std::get<1>(it)[2] == 1);
        REQUIRE(std::get<2>(it)[2] == -3);
        auto value_tuple = *it;
        REQUIRE(std::get<0>(value_tuple) == 1);
        REQUIRE(std::get<1>(value_tuple) == 3);
        REQUIRE(std::get<2>(value_tuple) == -1);
    }

    SECTION("operator+=(difference_type)") {
        auto it = begin;
        it += 1; 
        auto orig = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
        REQUIRE(begin == orig);
        REQUIRE(std::get<0>(it)[2] == 3);
        REQUIRE(std::get<1>(it)[2] == 1);
        REQUIRE(std::get<2>(it)[2] == -3);
        auto value_tuple = *it;
        REQUIRE(std::get<0>(value_tuple) == 1);
        REQUIRE(std::get<1>(value_tuple) == 3);
        REQUIRE(std::get<2>(value_tuple) == -1);
    }

    SECTION("operator-(difference_type)") {
        auto it = end - 3;
        REQUIRE(std::get<0>(it)[2] == 5);
        REQUIRE(std::get<1>(it)[2] == 666);
        // TODO dovrebbe essere -5, l'iteratore
        // vede comunque la sequenza originale
        REQUIRE(std::get<2>(it)[2] == -9);
        auto value_tuple = *it;
        REQUIRE(std::get<0>(value_tuple) == 3);
        REQUIRE(std::get<1>(value_tuple) == 1);
        // TODO dovrebbe essere -9, l'iteratore
        // vede comunque la sequenza originale
        REQUIRE(std::get<2>(value_tuple) == -7);
    }

    SECTION("operator-=(difference_type)") {
        auto it = end;
        it -= 3;
        auto orig = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};
        REQUIRE(end == orig);
        REQUIRE(std::get<0>(it)[2] == 5);
        REQUIRE(std::get<1>(it)[2] == 666);
        // TODO dovrebbe essere -5, l'iteratore
        // vede comunque la sequenza originale
        REQUIRE(std::get<2>(it)[2] == -9);
        auto value_tuple = *it;
        REQUIRE(std::get<0>(value_tuple) == 3);
        REQUIRE(std::get<1>(value_tuple) == 1);
        // TODO dovrebbe essere -9, l'iteratore
        // vede comunque la sequenza originale
        REQUIRE(std::get<2>(value_tuple) == -7);
    }

    SECTION("relational operators") {
        REQUIRE(begin < end);
        REQUIRE(begin <= end);
        REQUIRE(end > begin);
        REQUIRE(end >= begin);
        REQUIRE(begin != end);
        REQUIRE(!(begin == end));
        REQUIRE(begin == begin);
        REQUIRE(!(begin != begin));
        REQUIRE(end == end);
        REQUIRE(!(end != end));
    }

    SECTION("iterator for loop, element access via operator*()") {
        for(auto it = begin; it != end; ++it) {
            const auto& cur = *it;
            REQUIRE(std::get<0>(cur) == a[std::distance(begin, it)]);
            REQUIRE(std::get<1>(cur) == b[std::distance(begin, it)]);
            REQUIRE(std::get<2>(cur) == c[std::distance(begin, it)]);
            std::get<0>(cur) = 777;
            std::get<1>(cur) = 888;
            std::get<2>(cur) = 'A';
            REQUIRE(a[std::distance(begin, it)] == 777);
            REQUIRE(b[std::distance(begin, it)] == 888);
            REQUIRE(c[std::distance(begin, it)] == 'A');
        }
    }

    SECTION("iterator for loop, element access via structured binding") {
        for(auto it = begin; it != end; ++it) {
            auto [a_bind, b_bind, c_bind] = *it;
            REQUIRE(a_bind == a[std::distance(begin, it)]);
            REQUIRE(b_bind == b[std::distance(begin, it)]);
            REQUIRE(c_bind == c[std::distance(begin, it)]);
            a_bind = 777;
            b_bind = 888;
            c_bind = 'A';
            REQUIRE(a[std::distance(begin, it)] == 777);
            REQUIRE(b[std::distance(begin, it)] == 888);
            REQUIRE(c[std::distance(begin, it)] == 'A');
        }
    }

    SECTION("operator[](difference_type) for loop, element access via structured binding") {
        for(decltype(begin)::difference_type i = 0; i < std::distance(begin, end); ++i) {
            auto [a_bind, b_bind, c_bind] = begin[i];
            REQUIRE(a_bind == a[i]);
            REQUIRE(b_bind == b[i]);
            REQUIRE(c_bind == c[i]);
            a_bind = 777;
            b_bind = 888;
            c_bind = 'A';
            REQUIRE(a[i] == 777);
            REQUIRE(b[i] == 888);
            REQUIRE(c[i] == 'A');
        }
    }
}
