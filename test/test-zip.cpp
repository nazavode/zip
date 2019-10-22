// #include <zip.h>

// #include <array>
// #include <iterator>
// #include <type_traits>
// #include <vector>

// #include "catch.hpp"

// using Catch::Matchers::Approx;
// using Catch::Matchers::Equals;

// TEST_CASE("empty zip", "[zip]") {
//     auto z = zip::zip{};
//     STATIC_REQUIRE(decltype(z)::arity == 0);
// }

// TEST_CASE("zip supports empty iteration space", "[zip]") {
//     std::array<int, 0> a;
//     std::array<float, 0> b;
//     auto z = zip::zip{a, b};
//     REQUIRE(std::begin(z) == std::end(z));
// }

// TODO
// Add tests for iterator concept constraints, e.g.:
// LegacyRandomAccessIterator =
//     LegacyIterator +
//     LegacyForwardIterator +
//     LegacyBidirectionalIterator +
//     random access operations

// TEST_CASE("zip works with stl algorithms", "[zip]") {
//     std::vector<int> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
//     auto z = zip::zip{a, b};

//     SECTION("op takes zipped tuple by value, modifies components through reference
//     obtained via std::get") {
//         std::for_each(std::begin(z), std::end(z), [](auto e) {
//             std::get<0>(e) += 1;
//             std::get<1>(e) += 2;
//         });
//         REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
//         REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
//     }

//     SECTION("op takes zipped tuple by value, modifies components through reference
//     obtained via structured binding") {
//         std::for_each(std::begin(z), std::end(z), [](auto e) {
//             auto [x, y] = e;
//             x += 1;
//             y += 2;
//         });
//         REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
//         REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
//     }

//     SECTION("op takes zipped tuple by const reference, modifies components through
//     reference obtained via get") {
//         std::for_each(std::begin(z), std::end(z), [](const auto& e) {
//             std::get<0>(e) += 1;
//             std::get<1>(e) += 2;
//         });
//         REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
//         REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
//     }

//     SECTION("op takes zipped tuple by const reference, modifies components through
//     non-const reference obtained via structured binding") {
//         std::for_each(std::begin(z), std::end(z), [](const auto& e) {
//             auto [x, y] = e;
//             x += 1;
//             y += 2;
//         });
//         REQUIRE_THAT(a, Equals(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 7, 8, 9}));
//         REQUIRE_THAT(b, Approx(std::vector<float>{11, 10, 9, 8, 7, 6, 5}));
//     }
// }

// TEST_CASE("zip supports range-based for loops") {
//     std::vector<int> a{9, 8, 7, 6, 5, 4, 3};
//     std::vector<long long> b{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//     for (auto [_a, _b] : zip::zip{a, b}) {
//         _a = -1;
//         _b = -1;
//     }

//     REQUIRE_THAT(a, Equals(std::vector<int>{-1, -1, -1, -1, -1, -1, -1}));
//     REQUIRE_THAT(b, Equals(std::vector<long long>{-1, -1, -1, -1, -1, -1, -1, 7, 8,
//     9}));
// }

// TEST_CASE("zip is random access") {
//     std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     std::vector<float> b{9, 8, 7, 6, 5, 4, 3};
//     std::vector<long long> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     auto z = zip::zip{a, b};

//     for(int i = 0; i < std::size(z); ++i){
//         const auto e = z[i];
//         REQUIRE(std::get<0>(e) == a[i]);
//         REQUIRE(std::get<1>(e) == b[i]);
//         REQUIRE(std::get<2>(e) == c[i]);
//     }
// }

// TEST_CASE("zip gives access to references to sequences", "[zip]") {
//     std::array<int, 0> a;
//     std::vector<float> b;
//     std::vector<long long> c;
//     auto z = zip::zip{a, b, c};

//     SECTION("via unqualified get") {
//         using std::get;
//         STATIC_REQUIRE(std::is_same_v<decltype(a)&, decltype(get<0>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(b)&, decltype(get<1>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(c)&, decltype(get<2>(z))>);
//     }

//     SECTION("via std::get") {
//         STATIC_REQUIRE(std::is_same_v<decltype(a)&, decltype(std::get<0>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(b)&, decltype(std::get<1>(z))>);
//         STATIC_REQUIRE(std::is_same_v<decltype(c)&, decltype(std::get<2>(z))>);
//     }

//     // SECTION("via structured binding") {
//     //     auto& [_a, _b, _c] = z;
//     //     STATIC_REQUIRE(std::is_same_v<decltype(a)&, decltype(_a));
//     //     STATIC_REQUIRE(std::is_same_v<decltype(b)&, decltype(_b));
//     //     STATIC_REQUIRE(std::is_same_v<decltype(c)&, decltype(_c));
//     // }
// }

// TEST_CASE("zip size matches the size of the shortest sequence", "[zip]") {
//     std::vector<float> a;
//     std::vector<int> b{0, 0, 0};
//     auto z = zip::zip{a, b};

//     REQUIRE(std::size(z) == 0);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 0);
//     REQUIRE(std::begin(z) == std::end(z));

//     b.push_back(0);
//     // a = {}
//     // b = {0, 0, 0, 0}
//     REQUIRE(std::size(z) == 0);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 0);
//     REQUIRE(std::begin(z) == std::end(z));

//     a.push_back(0);
//     // a = {0}
//     // b = {0, 0, 0, 0}
//     REQUIRE(std::size(z) == 1);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 1);

//     a.push_back(0);
//     // a = {0, 0}
//     // b = {0, 0, 0, 0}
//     REQUIRE(std::size(z) == 2);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 2);

//     a.push_back(0);
//     // a = {0, 0, 0}
//     // b = {0, 0, 0, 0}
//     REQUIRE(std::size(z) == 3);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 3;

//     a.push_back(0);
//     // a = {0, 0, 0, 0}
//     // b = {0, 0, 0, 0}
//     REQUIRE(std::size(z) == 4);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 4);

//     a.push_back(0);
//     // a = {0, 0, 0, 0, 0}
//     // b = {0, 0, 0, 0}
//     REQUIRE(std::size(z) == 4);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 4);

//     b.push_back(0);
//     // a = {0, 0, 0, 0, 0}
//     // b = {0, 0, 0, 0, 0}
//     REQUIRE(std::size(z) == 5);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 5);

//     b.clear();
//     // a = {0, 0, 0, 0, 0}
//     // b = {}
//     REQUIRE(std::size(z) == 0);
//     REQUIRE(std::distance(std::begin(z), std::end(z)) == 0);
//     REQUIRE(std::begin(z) == std::end(z));
// }

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

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