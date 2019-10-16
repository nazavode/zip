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

TEST_CASE("zipped iterators constness is preserved", "[zip_iterator]") {
    std::array<int, 1> a;
    std::array<long long, 1> b;

    {
        auto it = zip::zip_iterator{std::cbegin(a), std::cbegin(b)};
        auto [a_item, b_item] = *it;
        STATIC_REQUIRE(is_const<decltype(a_item)>);
        STATIC_REQUIRE(is_const<decltype(b_item)>);
    }

    {
        auto it = zip::zip_iterator{std::begin(a), std::cbegin(b)};
        auto [a_item, b_item] = *it;
        STATIC_REQUIRE_FALSE(is_const<decltype(a_item)>);
        STATIC_REQUIRE(is_const<decltype(b_item)>);
    }

    {
        auto it = zip::zip_iterator{std::cbegin(a), std::begin(b)};
        auto [a_item, b_item] = *it;
        STATIC_REQUIRE(is_const<decltype(a_item)>);
        STATIC_REQUIRE_FALSE(is_const<decltype(b_item)>);
    }

    {
        auto it = zip::zip_iterator{std::begin(a), std::begin(b)};
        auto [a_item, b_item] = *it;
        STATIC_REQUIRE_FALSE(is_const<decltype(a_item)>);
        STATIC_REQUIRE_FALSE(is_const<decltype(b_item)>);
    }
}

TEST_CASE("zip_iterator abides by random access iterator contract", "[zip_iterator]") {
    std::array<int, 6>          a{0,  1,  2,  3,  4,   5};
    std::array<long long, 6>    b{4,  3,  2,  1,  0, 666};
    std::array<signed char, 10> c{0, -1, -2, -3, -4,  -5, -6, -7, -8, -9};

    auto begin = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
    auto end = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};

    SECTION("operator-(const zip_iterator&)") {
        REQUIRE(end - begin == 6);
    }

    SECTION("std::distance(const zip_iterator&, const zip_iterator&)") {
        REQUIRE(std::distance(begin, end)
             == std::distance(std::begin(a), std::end(a)));
    }

    SECTION("operator+(difference_type)") {
        auto it = begin + 1;
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::begin(a) + 1));
        REQUIRE(b_item == *(std::begin(b) + 1));
        REQUIRE(c_item == *(std::begin(c) + 1));
    }

    SECTION("operator+=(difference_type)") {
        auto it = begin;
        it += 1; 
        auto orig = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
        REQUIRE(begin == orig);
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::begin(a) + 1));
        REQUIRE(b_item == *(std::begin(b) + 1));
        REQUIRE(c_item == *(std::begin(c) + 1));
    }

    SECTION("operator++() [prefix]") {
        auto it = begin;
        auto inc = ++it;
        auto orig = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
        REQUIRE(begin == orig);
        REQUIRE(it == inc);
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::begin(a) + 1));
        REQUIRE(b_item == *(std::begin(b) + 1));
        REQUIRE(c_item == *(std::begin(c) + 1));
    }

    SECTION("operator++(int) [postfix]") {
        auto it = begin;
        auto inc = it++;
        auto orig = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
        REQUIRE(begin == orig);
        REQUIRE(inc == orig);
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::begin(a) + 1));
        REQUIRE(b_item == *(std::begin(b) + 1));
        REQUIRE(c_item == *(std::begin(c) + 1));
    }

    SECTION("operator-(difference_type)") {
        auto it = end - 3;
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::end(a)-3));
        REQUIRE(b_item == *(std::end(b)-3));
        // TODO should be -9, original iterator views the original sequence
        REQUIRE(c_item == *(std::end(c)-3));
    }

    SECTION("operator-=(difference_type)") {
        auto it = end;
        it -= 3;
        auto orig = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};
        REQUIRE(end == orig);
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::end(a)-3));
        REQUIRE(b_item == *(std::end(b)-3));
        // TODO should be -9, original iterator views the original sequence
        REQUIRE(c_item == *(std::end(c)-3));
    }

    SECTION("operator--() [prefix]") {
        auto it = end;
        auto dec = --it;
        auto orig = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};
        REQUIRE(end == orig);
        REQUIRE(it == dec);
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::end(a)-1));
        REQUIRE(b_item == *(std::end(b)-1));
        // TODO should be -5, original iterator views the original sequence
        REQUIRE(c_item == *(std::end(c)-1));
    }

    SECTION("operator--(int) [postfix]") {
        auto it = end;
        auto dec = it--;
        auto orig = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};
        REQUIRE(end == orig);
        REQUIRE(dec == orig);
        auto [a_item, b_item, c_item] = *it;
        REQUIRE(a_item == *(std::end(a)-1));
        REQUIRE(b_item == *(std::end(b)-1));
        // TODO should be -5, original iterator views the original sequence
        REQUIRE(c_item == *(std::end(c)-1));
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
