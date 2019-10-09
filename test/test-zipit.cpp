#include <zipit.h>

#include "catch.hpp"

#include <vector>
#include <array>
#include <iterator>
#include <type_traits>

TEST_CASE("zip provides access to stored references to sequences", "[zip]") {
    std::array<int, 0> a;
    std::vector<float> b;
    std::vector<long long> c;
    auto z = zipit::zip{a, b, c};

    SECTION("via unqualified get") {
        STATIC_REQUIRE(std::is_same_v<decltype(a)&, decltype(get<0>(z))>);
        STATIC_REQUIRE(std::is_same_v<decltype(b)&, decltype(get<1>(z))>);
        STATIC_REQUIRE(std::is_same_v<decltype(c)&, decltype(get<2>(z))>);
    }

    SECTION("via std::get") {
        STATIC_REQUIRE(std::is_same_v<decltype(a)&, decltype(std::get<0>(z))>);
        STATIC_REQUIRE(std::is_same_v<decltype(b)&, decltype(std::get<1>(z))>);
        STATIC_REQUIRE(std::is_same_v<decltype(c)&, decltype(std::get<2>(z))>);
    }
}

TEST_CASE("zip size matches the shortest sequence's size", "[zip]") {
    std::vector<float> a;
    std::vector<int> b{0, 0, 0};
    auto z = zipit::zip{a, b};

    REQUIRE(std::size(z) == 0);
    b.emplace_back(0);
    REQUIRE(std::size(z) == 0);

    a.emplace_back(0);
    REQUIRE(std::size(z) == 1);
    a.emplace_back(0);
    REQUIRE(std::size(z) == 2);
    a.emplace_back(0);
    REQUIRE(std::size(z) == 3);
    a.emplace_back(0);
    REQUIRE(std::size(z) == 4);
    a.emplace_back(0);
    REQUIRE(std::size(z) == 5);
    a.emplace_back(0);
    REQUIRE(std::size(z) == 6);

    b.emplace_back(0);
    REQUIRE(std::size(z) == 6);
}

// TEST_CASE("zip stores references to sequences", "[zip]") {
//     std::array<int, 0> a;
//     std::vector<float> b;
//     std::vector<long long> c;

//     SECTION("when explicitly instantiated") {
//         auto z = zipit::zip<decltype(a), decltype(b), decltype(c)>{a, b, c};
//         STATIC_REQUIRE(std::is_same_v<decltype(a)&, decltype(get<0>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(b)&, decltype(get<1>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(c)&, decltype(get<2>(z))>);
//     }

//     SECTION("when template arguments are deduced") {
//         auto z = zipit::zip{a, b, c};
//         STATIC_REQUIRE(std::is_same_v<decltype(a)&, decltype(get<0>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(b)&, decltype(get<1>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(c)&, decltype(get<2>(z))>);
//     }
// }

// int main() {
//   std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//   std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
//   std::vector<long long> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//   // auto zipped = zip<decltype(a), decltype(b), decltype(c)>{a, b, c};
//   auto zipped = zip{a, b, c};
//   // Verify class types deduction (refs to sequences):
//   static_assert(
//       std::is_same_v<decltype(a)&, decltype(std::get<0>(zipped.m_seq))>);
//   static_assert(
//       std::is_same_v<decltype(b)&, decltype(std::get<1>(zipped.m_seq))>);
//   static_assert(
//       std::is_same_v<decltype(c)&, decltype(std::get<2>(zipped.m_seq))>);
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
//     std::get<0>(e)++;
//     std::get<1>(e)++;
//     std::get<2>(e)++;
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
//     std::get<0>(e)++;
//     std::get<1>(e)++;
//     std::get<2>(e)++;
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
//   std::get<0>(value) = 0;
//   auto [x, y, z] = value;
//   x = 3;

//   int xx = 1;
//   float yy = 8;
//   long long zz = 2;
//   value = {xx, yy, zz};

//   const auto nth = it[9];
//   // it[22] = {0, 0, 0};

//   return static_cast<long long>(std::get<0>(value) + std::get<1>(value) +
//                                 std::get<2>(value));

//   /*

//   auto value = *it;
//   std::cout << std::get<0>(value) << std::get<1>(value) << std::get<2>(value) <<
//   std::endl;

//   ++it;
//   value = *it;
//   std::cout << std::get<0>(value) << std::get<1>(value) << std::get<2>(value) <<
//   std::endl;

//   const auto it2 = it;
//   const auto eq = (it == it2);

//   // Print<decltype(value)> ppp{};
//   return std::tuple_size_v<decltype(it)::zip_iterator_type>;

//   // Print<decltype(it)::zip_iterator_type> p;

//   */
// }