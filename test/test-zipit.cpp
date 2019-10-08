#include <zipit.h>

#include "catch.hpp"

using Catch::Matchers::Approx;
using Catch::Matchers::Equals;

TEST_CASE("zip works on empty iteration space", "[zip]") {
    std::array<int, 0> a;
    std::array<float, 0> b;
    auto z = zipit::zip{a, b};
    REQUIRE(std::begin(z) == std::end(z));
}

TEST_CASE("zip stops stops on shortest sequence", "[zip]") {
    std::vector<int> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
    auto z = zipit::zip{a, b};

    SECTION("on std::distance") {
        REQUIRE(std::distance(std::begin(z), std::end(z)) ==
                std::min(std::size(a), std::size(b)));
    }

    SECTION("on std::size") {
        REQUIRE(std::size(z) == std::min(std::size(a), std::size(b)));
    }

    SECTION("on std::end") {
        REQUIRE(get<1>(*(std::end(z) - 1)) == *(std::end(b) - 1));
    }
}

TEST_CASE("zip works with stl algorithms", "[zip]") {
    std::vector<int> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
    auto z = zipit::zip{a, b};

    SECTION(
        "lambda takes zipped tuple by value and modify components via reference via "
        "get") {
        std::for_each(std::begin(z), std::end(z), [](auto e) {
            get<0>(e) += 1;
            get<1>(e) += 2;
        });
        REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
        REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
    }

    SECTION(
        "lambda takes zipped tuple by value modify components via non-const reference "
        "obtained via structured binding") {
        std::for_each(std::begin(z), std::end(z), [](auto e) {
            auto [x, y] = e;
            x += 1;
            y += 2;
        });
        REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
        REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
    }

    SECTION(
        "lambda takes zipped tuple by const reference and modifies components via "
        "non-const reference obtained via get") {
        std::for_each(std::begin(z), std::end(z), [](const auto& e) {
            get<0>(e) += 1;
            get<1>(e) += 2;
        });
        REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
        REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
    }

    SECTION(
        "lambda takes zipped tuple by const reference and modifies components via "
        "non-const reference obtained via structured binding") {
        std::for_each(std::begin(z), std::end(z), [](const auto& e) {
            auto [x, y] = e;
            x += 1;
            y += 2;
        });
        REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
        REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
    }
}

TEST_CASE("zip works with range-based for loops") {
    std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
    std::vector<long long> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    decltype(a) expected_a;
    decltype(b) expected_b;
    decltype(c) expected_c;
    for (auto [aa, bb, cc] : zipit::zip{a, b, c}) {
        
    }
}

TEST_CASE("zip is random access") {
    std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
    std::vector<long long> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for(int i = 0; i < std::size(z); ++i){
        const auto e = z[i];
        REQUIRE(get<0>(e) == a[i]);
        REQUIRE(get<1>(e) == b[i]);
        REQUIRE(get<2>(e) == c[i]);
    }    
}


TEST_CASE("components can be accessed", "[zip]") {
    SECTION("via qualified std::get") {}
    SECTION("via unqualified get") {}
    SECTION("via structured binding") {}
} 

// TEST_CASE("zip forward iteration stops on the shortest sequence") {
//     std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//   std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
//   std::vector<long long> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// }

// int main() {
//   std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//   std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
//   std::vector<long long> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//   // auto zipped = zip<decltype(a), decltype(b), decltype(c)>{a, b, c};
//   auto zipped = zip{a, b, c};
//   // Verify class types deduction (refs to sequences):
//   static_assert(
//       std::is_same_v<decltype(a)&, decltype(get<0>(zipped.m_seq))>);
//   static_assert(
//       std::is_same_v<decltype(b)&, decltype(get<1>(zipped.m_seq))>);
//   static_assert(
//       std::is_same_v<decltype(c)&, decltype(get<2>(zipped.m_seq))>);
//   //
//   // Range based for loop:
//   //
//   for (auto [a1, b1, c1] : zip{a, b, c}) {
//     for (auto [a2, b2, c2] : zip{a, b, c}) {
//     }
//   }
//   print(zip{a, b, c});

//   //
//   // STL algorithms:
//   //
//   std::for_each(std::begin(zipped), std::end(zipped), [](auto e) {
//     // Lambda takes zipped tuple *by value*
//     // Modify components via reference:
//     get<0>(e)++;
//     get<1>(e)++;
//     get<2>(e)++;
//   });

//   std::for_each(std::begin(zipped), std::end(zipped), [](auto e) {
//     // Lambda takes zipped tuple *by value*
//     // Modify components via non-const reference obtained via structured
//     // binding:
//     auto [x, y, z] = e;
//     x++;
//     y++;
//     z++;
//   });

//   std::for_each(std::begin(zipped), std::end(zipped), [](const auto& e) {
//     // Lambda takes zipped tuple *by const reference*
//     // Modify components via non-const reference:
//     get<0>(e)++;
//     get<1>(e)++;
//     get<2>(e)++;
//   });

//   std::for_each(std::begin(zipped), std::end(zipped), [](const auto& e) {
//     // Lambda takes zipped tuple *by const reference*
//     // Modify components via non-const reference obtained via structured
//     // binding:
//     auto [x, y, z] = e;
//     x++;
//     y++;
//     z++;
//   });

//   std::cout << "v' = v + 4" << std::endl;
//   print(zipped);

//   auto it = zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
//   ++it;
//   ++it;

//   auto value = *it;
//   get<0>(value) = 0;
//   auto [x, y, z] = value;
//   x = 3;

//   int xx = 1;
//   float yy = 8;
//   long long zz = 2;
//   value = {xx, yy, zz};

//   const auto nth = it[9];
//   // it[22] = {0, 0, 0};

//   return static_cast<long long>(get<0>(value) + get<1>(value) +
//                                 get<2>(value));

//   /*

//   auto value = *it;
//   std::cout << get<0>(value) << get<1>(value) << get<2>(value) <<
//   std::endl;

//   ++it;
//   value = *it;
//   std::cout << get<0>(value) << get<1>(value) << get<2>(value) <<
//   std::endl;

//   const auto it2 = it;
//   const auto eq = (it == it2);

//   // Print<decltype(value)> ppp{};
//   return std::tuple_size_v<decltype(it)::zip_iterator_type>;

//   // Print<decltype(it)::zip_iterator_type> p;

//   */
// }