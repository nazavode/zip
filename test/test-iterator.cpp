#include <zip.h>

#include "catch.hpp"

#include <array>
#include <type_traits>
#include <tuple>

// Modifica valori sequenze via tupla di refs

template<typename T>
constexpr bool is_const = std::is_const_v<std::remove_reference_t<T>>;

TEST_CASE("zip_iterator supports tuple-like semantics", "[zip_iterator]") {
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

TEST_CASE("zipped iterators constness is respected", "[zip_iterator]") {
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

TEST_CASE("random access iterator concept", "[zip_iterator]") {
    constexpr std::array<int, 6>   a{0,  1,  2,  3,  4,   5};
    constexpr std::array<long long, 6> b{4,  3,  2,  1,  0, 666};
    constexpr std::array<signed char, 10>  c{0, -1, -2, -3, -4,  -5, -6, -7, -8, -9};

    auto begin = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
    auto end = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};
    
    // // Iterators
    // for(auto it = begin; it != end; ++it) {
    //     const auto& cur = *it;
    //     std::cout << std::get<0>(cur) << " " << std::get<1>(cur) << " " << std::get<2>(cur) << std::endl;
    // }
    // std::cout << "-----------------" << std::endl;

    // // Iterators + unpack via structured binding
    // for(auto it = begin; it != end; ++it) {
    //     auto [aa, bb, cc] = *it;
    //     std::cout << aa << " " << bb << " " << cc << std::endl;
    // }
    // std::cout << "-----------------" << std::endl;
    
    // // Random access (subscript) + unpack via structured binding
    // for(decltype(begin)::difference_type i = 0; i < std::distance(begin, end); ++i) {
    //     auto [aa, bb, cc] = begin[i];
    //     std::cout << aa << " " << bb << " " << cc << std::endl;
    // }
    // std::cout << "-----------------" << std::endl;
    {
        // operator-(iterator)
        REQUIRE(end - begin == 6);
    }
    {
        // distance
        REQUIRE(std::distance(begin, end) == 6);
    }
    {
        // operator+(difference_type)
        auto it = begin + 1;
        REQUIRE(std::get<0>(it)[2] == 3);
        REQUIRE(std::get<1>(it)[2] == 1);
        REQUIRE(std::get<2>(it)[2] == -3);
        auto value_tuple = *it;
        REQUIRE(std::get<0>(value_tuple) == 1);
        REQUIRE(std::get<1>(value_tuple) == 3);
        REQUIRE(std::get<2>(value_tuple) == -1);
    }
    {
        // operator+=(difference_type)
        auto it = begin;
        it += 1;
        {   
            auto orig = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
            REQUIRE(begin == orig);
        }
        REQUIRE(std::get<0>(it)[2] == 3);
        REQUIRE(std::get<1>(it)[2] == 1);
        REQUIRE(std::get<2>(it)[2] == -3);
        auto value_tuple = *it;
        REQUIRE(std::get<0>(value_tuple) == 1);
        REQUIRE(std::get<1>(value_tuple) == 3);
        REQUIRE(std::get<2>(value_tuple) == -1);
    }
    {
        // operator-(difference_type)
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
    {
        // operator-=(difference_type)
        auto it = end;
        it -= 3;
        {
            auto orig = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};
            REQUIRE(end == orig);
        }
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
    {
        // Comparison operators
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
}
