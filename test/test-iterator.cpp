#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <zip.h>

#include <algorithm>
#include <array>
#include <forward_list>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

// template<template <typename...> typename Iterator, typename ...Containers>
// struct Scenario {

//     using iterator = Iterator<typename Containers::iterator...>;

//     auto begin() {
//         return zip::ttl::transform<iterator>(containers, [](auto&& c){
//             using std::begin;
//             return begin(c);
//         });
//     }

//     auto end() {
//         return zip::ttl::transform<iterator>(containers, [](auto&& c){
//             using std::end;
//             return end(c);
//         });
//     }

//     auto size() const { return std::size(std::get<0>(containers)); }

//     std::tuple<Containers...> containers;
// };

// 1. Define template fixture

// 2. Register class as template fixture
// TYPED_TEST_SUITE_P(IteratorTest);

// 3. Define type-parametrized tests
// TYPED_TEST_P(IteratorTest, DoesBlah) {
//   // Inside a test, refer to TypeParam to get the type parameter.
//   [[maybe_unused]] auto&& p = TypeParam{};
// }

// TYPED_TEST_P(IteratorTest, DoesDiop) {
//   // Inside a test, refer to TypeParam to get the type parameter.
//   [[maybe_unused]] auto&& p = TypeParam{};
// }

// 4. Register the template test suite
// REGISTER_TYPED_TEST_SUITE_P(IteratorTest,
//                             DoesBlah, DoesDiop);

// 5. Instantiate
// Googletest macros doesn't support -Wall -Werror (wtf?),
// so users should expect breaks:
// https://github.com/google/googletest/pull/2316#issuecomment-518269259
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

// typedef ::testing::Types<char, int, unsigned int> MyTypes;
// INSTANTIATE_TYPED_TEST_SUITE_P(My, IteratorTest, MyTypes);

// #pragma GCC diagnostic pop

// Matchers
using ::testing::Each;

template <typename IteratorCategory>
struct ForwardInterface : public ::testing::Test {
    // using x_type = std::array<int, 10>;
    // using y_type = std::array<long long, 10>;
    // using z_type = std::array<signed char, 10>;
    // using iterator_category = IteratorCategory;
    // using iterator =
    //     typename zip::iterator_type<iterator_category, x_type::iterator,
    //     y_type::iterator,
    //                                 z_type::iterator>::type;

    // static iterator begin() { return {std::begin(x), std::begin(y), std::begin(z)}; }

    // static iterator end() { return {std::end(x), std::end(y), std::end(z)}; }

    // static auto size() noexcept { return std::size(x); }

    // template <std::size_t I>
    // static auto sentinel() {
    //     if constexpr (I == 0)
    //         return 11;
    //     else if constexpr (I == 1)
    //         return static_cast<long long>(11);
    //     else
    //         return static_cast<signed char>(-10);
    // }

    // static x_type x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // static y_type y{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    // static z_type z{0, -1, -2, -3, -4, -5, -6, -7, -8, -9};
};

TYPED_TEST_SUITE_P(ForwardInterface);

template <typename IteratorCategory>
struct RandomAccessInterface : public ::testing::Test {};
TYPED_TEST_SUITE_P(RandomAccessInterface);

template <typename IteratorCategory>
struct BidirectionalInterfaceTest : public ::testing::Test {};
TYPED_TEST_SUITE_P(BidirectionalInterfaceTest);

//
// Helper: containers()
//

auto containers();

auto containers(std::forward_iterator_tag) {
    using ret = std::tuple<std::vector<std::int32_t>, std::forward_list<std::uint64_t>,
                           std::vector<std::int8_t>>;
    return ret{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
               {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
               {0, -1, -2, -3, -4, -5, -6, -7, -8, -9}};
}

auto containers(std::bidirectional_iterator_tag) {
    using ret = std::tuple<std::vector<std::int32_t>, std::list<std::uint64_t>,
                           std::vector<std::int8_t>>;
    return ret{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
               {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
               {0, -1, -2, -3, -4, -5, -6, -7, -8, -9}};
}

auto containers(std::random_access_iterator_tag) {
    using ret = std::tuple<std::vector<std::int32_t>, std::vector<std::uint64_t>,
                           std::vector<std::int8_t>>;
    return ret{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
               {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
               {0, -1, -2, -3, -4, -5, -6, -7, -8, -9}};
}

auto containers(zip::offset_iterator_tag) {
    return containers(std::random_access_iterator_tag{});
}

//
// Helper: begin(ContainerTuple&&)
//

template <typename IteratorCategory, typename ContainerTuple, std::size_t... Indexes>
auto begin_impl(IteratorCategory tag, ContainerTuple&& data,
                std::index_sequence<Indexes...>) {
    return zip::make_iterator(
        tag, std::begin(std::get<Indexes>(std::forward<ContainerTuple>(data)))...);
}

template <typename IteratorCategory, typename ContainerTuple>
auto begin(IteratorCategory tag, ContainerTuple&& data) {
    using indexes = std::make_index_sequence<
        std::tuple_size_v<std::remove_reference_t<ContainerTuple>>>;
    return begin_impl(tag, std::forward<ContainerTuple>(data), indexes{});
}

//
// Helper: end(ContainerTuple&&)
//

template <typename IteratorCategory, typename ContainerTuple, std::size_t... Indexes>
auto end_impl(IteratorCategory tag, ContainerTuple&& data,
              std::index_sequence<Indexes...>) {
    return zip::make_iterator(
        tag, std::end(std::get<Indexes>(std::forward<ContainerTuple>(data)))...);
}

template <typename IteratorCategory, typename ContainerTuple>
auto end(IteratorCategory tag, ContainerTuple&& data) {
    using indexes = std::make_index_sequence<
        std::tuple_size_v<std::remove_reference_t<ContainerTuple>>>;
    return end_impl(tag, std::forward<ContainerTuple>(data), indexes{});
}

//
// Helper: size(ContainerTuple&&)
//

template <typename ContainerTuple>
auto size(ContainerTuple&& data) {
    return std::size(std::get<0>(std::forward<ContainerTuple>(data)));
}

// TEST_F(ZipIteratorTest, IteratorCategoryRandomAccess) {
//     EXPECT_TRUE((std::is_same_v<typename zip_iterator_type::iterator_category,
//                                 std::random_access_iterator_tag>));
//     std::array<int, 10> a;
//     std::vector<long long> b;
//     std::array<signed char, 10> c;
//     int d[20];
//     auto it =
//         zip::make_iterator(std::begin(a), std::begin(b), std::begin(c), std::begin(d));
//     EXPECT_TRUE((std::is_same_v<decltype(it)::iterator_category,
//                                 std::random_access_iterator_tag>));
// }

// TEST_F(ZipIteratorTest, IteratorCategoryBidirectional) {
//     std::array<int, 10> a;
//     std::list<int> b;
//     std::vector<long long> c;
//     auto it = zip::make_iterator(std::begin(a), std::begin(b), std::begin(c));
//     EXPECT_TRUE((std::is_same_v<decltype(it)::iterator_category,
//                                 std::bidirectional_iterator_tag>));
// }

// TEST_F(ZipIteratorTest, IteratorCategoryForward) {
//     std::array<int, 10> a;
//     std::forward_list<int> b;
//     std::vector<long long> c;
//     std::list<int> d;
//     auto it =
//         zip::make_iterator(std::begin(a), std::begin(b), std::begin(c), std::begin(d));
//     EXPECT_TRUE(
//         (std::is_same_v<decltype(it)::iterator_category, std::forward_iterator_tag>));
// }

// TYPED_TEST_P(ZipIteratorTest, IsNotDefaultConstructible) {
//     EXPECT_FALSE(
//         std::is_default_constructible_v<typename
//         ZipIteratorTest<TypeParam>::iterator>);
// }

// TYPED_TEST_P(ZipIteratorTest, IsCopyConstructible) {
//     EXPECT_TRUE(
//         std::is_copy_constructible_v<typename ZipIteratorTest<TypeParam>::iterator>);
// }

// TYPED_TEST_P(ZipIteratorTest, IsCopyAssignable) {
//     EXPECT_TRUE(std::is_copy_assignable_v<typename
//     ZipIteratorTest<TypeParam>::iterator>);
// }

// TYPED_TEST_P(ZipIteratorTest, IsMoveConstructible) {
//     EXPECT_TRUE(
//         std::is_move_constructible_v<typename ZipIteratorTest<TypeParam>::iterator>);
// }

// TYPED_TEST_P(ZipIteratorTest, IsMoveAssignable) {
//     EXPECT_TRUE(std::is_move_assignable_v<typename
//     ZipIteratorTest<TypeParam>::iterator>);
// }

// TYPED_TEST_P(ZipIteratorTest, IsDestructible) {
//     EXPECT_TRUE(std::is_destructible_v<typename ZipIteratorTest<TypeParam>::iterator>);
// }

// TYPED_TEST_P(ZipIteratorTest, IsSwappable) {
//     EXPECT_TRUE(std::is_swappable_v<typename ZipIteratorTest<TypeParam>::iterator>);
// }

// TEST_F(ZipIteratorTest, ConstnessPreservedConst) {
//     auto it = zip::make_iterator(std::cbegin(x), std::cbegin(y), std::cbegin(z));
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
//     EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
//     EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
// }

// TEST_F(ZipIteratorTest, ConstnessPreservedMut) {
//     auto it = zip::make_iterator(std::begin(x), std::begin(y), std::begin(z));
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
//     EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
//     EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
// }

// TEST_F(ZipIteratorTest, ConstnessPreservedMixed) {
//     auto it = zip::make_iterator(std::begin(x), std::cbegin(y), std::begin(z));
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
//     EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
//     EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
// }

TYPED_TEST_P(ForwardInterface, OperatorDereference) {
    // operator*()
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto value = *begin(tag, data);
    auto [x_item, y_item, z_item] = value;
    EXPECT_EQ(x_item, *(std::begin(std::get<0>(data))));
    EXPECT_EQ(y_item, *(std::begin(std::get<1>(data))));
    EXPECT_EQ(z_item, *(std::begin(std::get<2>(data))));
}

TYPED_TEST_P(RandomAccessInterface, OperatorSubscript) {
    // operator[](difference_type)
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto value = begin(tag, data)[0];
    auto [x_item, y_item, z_item] = value;
    EXPECT_EQ(x_item, *(std::begin(std::get<0>(data))));
    EXPECT_EQ(y_item, *(std::begin(std::get<1>(data))));
    EXPECT_EQ(z_item, *(std::begin(std::get<2>(data))));
}

TYPED_TEST_P(RandomAccessInterface, OperatorMinusIterator) {
    // operator-(const zip_iterator&)
    auto tag = TypeParam{};
    auto data = containers(tag);
    EXPECT_EQ(end(tag, data) - begin(tag, data), size(data));
}

TYPED_TEST_P(RandomAccessInterface, OperatorPlusIntegral) {
    // operator+(difference_type)
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = begin(tag, data) + 1;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::begin(std::get<0>(data)) + 1));
    EXPECT_EQ(y_item, *(std::begin(std::get<1>(data)) + 1));
    EXPECT_EQ(z_item, *(std::begin(std::get<2>(data)) + 1));
}

TYPED_TEST_P(RandomAccessInterface, OperatorPlusEqualIntegral) {
    // operator+=(difference_type)
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = begin(tag, data);
    it += 1;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::begin(std::get<0>(data)) + 1));
    EXPECT_EQ(y_item, *(std::begin(std::get<1>(data)) + 1));
    EXPECT_EQ(z_item, *(std::begin(std::get<2>(data)) + 1));
}

TYPED_TEST_P(ForwardInterface, OperatorIncrementPrefix) {
    // operator++()
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = begin(tag, data);
    auto inc = ++it;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(it, inc);
    EXPECT_EQ(x_item, *(++std::begin(std::get<0>(data))));
    EXPECT_EQ(y_item, *(++std::begin(std::get<1>(data))));
    EXPECT_EQ(z_item, *(++std::begin(std::get<2>(data))));
}

// TEST_F(ZipIteratorTest, OperatorIncrementPostfix) {
//     // operator++(int)
//     auto it = begin();
//     auto inc = it++;
//     EXPECT_NE(it, inc);
//     EXPECT_EQ(inc, begin());
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_EQ(x_item, *(std::begin(x) + 1));
//     EXPECT_EQ(y_item, *(std::begin(y) + 1));
//     EXPECT_EQ(z_item, *(std::begin(z) + 1));
// }

// TEST_F(ZipIteratorTest, OperatorMinusIntegral) {
//     // operator-(difference_type)
//     auto it = end() - 3;
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_EQ(x_item, *(std::end(x) - 3));
//     EXPECT_EQ(y_item, *(std::end(y) - 3));
//     EXPECT_EQ(z_item, *(std::end(z) - 3));
// }

// TEST_F(ZipIteratorTest, OperatorMinusEqualIntegral) {
//     // operator-=(difference_type)
//     auto it = end();
//     it -= 3;
//     EXPECT_NE(it, end());
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_EQ(x_item, *(std::end(x) - 3));
//     EXPECT_EQ(y_item, *(std::end(y) - 3));
//     EXPECT_EQ(z_item, *(std::end(z) - 3));
// }

// TEST_F(ZipIteratorTest, OperatorDecrementPrefix) {
//     // operator--()
//     auto it = end();
//     auto dec = --it;
//     EXPECT_EQ(it, dec);
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_EQ(x_item, *(std::end(x) - 1));
//     EXPECT_EQ(y_item, *(std::end(y) - 1));
//     EXPECT_EQ(z_item, *(std::end(z) - 1));
// }

// TEST_F(ZipIteratorTest, OperatorDecrementPostfix) {
//     // operator--(int)
//     auto it = end();
//     auto dec = it--;
//     EXPECT_NE(it, dec);
//     EXPECT_EQ(end(), dec);
//     auto [x_item, y_item, z_item] = *it;
//     EXPECT_EQ(x_item, *(std::end(x) - 1));
//     EXPECT_EQ(y_item, *(std::end(y) - 1));
//     EXPECT_EQ(z_item, *(std::end(z) - 1));
// }

// TEST_F(ZipIteratorTest, OperatorLT) { EXPECT_LT(begin(), end()); }

// TEST_F(ZipIteratorTest, OperatorLE) {
//     EXPECT_LE(begin(), end());
//     EXPECT_LE(begin(), begin());
//     EXPECT_LE(end(), end());
// }

// TEST_F(ZipIteratorTest, OperatorGT) { EXPECT_GT(end(), begin()); }

// TEST_F(ZipIteratorTest, OperatorGE) {
//     EXPECT_GE(end(), begin());
//     EXPECT_GE(begin(), begin());
//     EXPECT_GE(end(), end());
// }

// TEST_F(ZipIteratorTest, OperatorNE) { EXPECT_NE(begin(), end()); }

// TEST_F(ZipIteratorTest, OperatorEQ) {
//     EXPECT_EQ(begin(), begin());
//     EXPECT_EQ(end(), end());
// }

// TEST_F(ZipIteratorTest, ModifyStdGet) {
//     auto value = *begin();
//     ASSERT_TRUE(std::is_reference_v<decltype(std::get<0>(value))>);
//     ASSERT_TRUE(std::is_reference_v<decltype(std::get<1>(value))>);
//     ASSERT_TRUE(std::is_reference_v<decltype(std::get<2>(value))>);
//     std::get<0>(value) = sentinel<0>();
//     EXPECT_EQ(*std::begin(x), sentinel<0>());
//     std::get<1>(value) = sentinel<1>();
//     EXPECT_EQ(*std::begin(y), sentinel<1>());
//     std::get<2>(value) = sentinel<2>();
//     EXPECT_EQ(*std::begin(z), sentinel<2>());
// }

// TEST_F(ZipIteratorTest, ModifyUnqualifiedGet) {
//     using std::get;
//     auto value = *begin();
//     ASSERT_TRUE(std::is_reference_v<decltype(get<0>(value))>);
//     ASSERT_TRUE(std::is_reference_v<decltype(get<1>(value))>);
//     ASSERT_TRUE(std::is_reference_v<decltype(get<2>(value))>);
//     get<0>(value) = sentinel<0>();
//     EXPECT_EQ(*std::begin(x), sentinel<0>());
//     get<1>(value) = sentinel<1>();
//     EXPECT_EQ(*std::begin(y), sentinel<1>());
//     get<2>(value) = sentinel<2>();
//     EXPECT_EQ(*std::begin(z), sentinel<2>());
// }

// TEST_F(ZipIteratorTest, ModifyStructuredBinding) {
//     auto [x_bind, y_bind, z_bind] = *begin();
//     ASSERT_TRUE(std::is_reference_v<decltype(x_bind)>);
//     ASSERT_TRUE(std::is_reference_v<decltype(y_bind)>);
//     ASSERT_TRUE(std::is_reference_v<decltype(z_bind)>);
//     x_bind = sentinel<0>();
//     EXPECT_EQ(*std::begin(x), sentinel<0>());
//     y_bind = sentinel<1>();
//     EXPECT_EQ(*std::begin(y), sentinel<1>());
//     z_bind = sentinel<2>();
//     EXPECT_EQ(*std::begin(z), sentinel<2>());
// }

// TEST_F(ZipIteratorTest, ModifyRefStructuredBinding) {
//     auto&& [x_bind, y_bind, z_bind] = *begin();
//     ASSERT_TRUE(std::is_reference_v<decltype(x_bind)>);
//     ASSERT_TRUE(std::is_reference_v<decltype(y_bind)>);
//     ASSERT_TRUE(std::is_reference_v<decltype(z_bind)>);
//     x_bind = sentinel<0>();
//     EXPECT_EQ(*std::begin(x), sentinel<0>());
//     y_bind = sentinel<1>();
//     EXPECT_EQ(*std::begin(y), sentinel<1>());
//     z_bind = sentinel<2>();
//     EXPECT_EQ(*std::begin(z), sentinel<2>());
// }

// TEST_F(ZipIteratorTest, StdDistance) { EXPECT_EQ(std::distance(begin(), end()),
// size()); }

// TEST_F(ZipIteratorTest, StdForEach) {
//     const auto v_0 = sentinel<0>();
//     const auto v_1 = sentinel<1>();
//     const auto v_2 = sentinel<2>();
//     std::for_each(begin(), end(), [=](auto&& e) {
//         std::get<0>(e) = v_0;
//         std::get<1>(e) = v_1;
//         std::get<2>(e) = v_2;
//     });
//     ASSERT_THAT(x, Each(v_0));
//     ASSERT_THAT(y, Each(v_1));
//     ASSERT_THAT(z, Each(v_2));
// }

// TEST_F(ZipIteratorTest, StdForEachStructuredBinding) {
//     const auto v_0 = sentinel<0>();
//     const auto v_1 = sentinel<1>();
//     const auto v_2 = sentinel<2>();
//     std::for_each(begin(), end(), [=](auto&& e) {
//         auto [xx, yy, zz] = e;
//         xx = v_0;
//         yy = v_1;
//         zz = v_2;
//     });
//     ASSERT_THAT(x, Each(v_0));
//     ASSERT_THAT(y, Each(v_1));
//     ASSERT_THAT(z, Each(v_2));
// }

// // TODO

// // TEST_F(ZipIteratorTest, StdTransform) {}
// // TEST_F(ZipIteratorTest, StdFind) {}
// // TEST_F(ZipIteratorTest, StdIsSorted) {}
// // TEST_F(ZipIteratorTest, StdSort) {}
// // TEST_F(ZipIteratorTest, StdNthElement) {}

// // TEST_CASE("zip_iterator works with std algorithms", "[zip_iterator]") {
// //     std::array<int, 10>         a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
// //     std::array<long long, 10>   b{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
// //     std::array<signed char, 10> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// //     auto begin = zip::make_iterator(std::begin(a), std::begin(b), std::begin(c));
// //     auto end = zip::make_iterator(std::end(a), std::end(b), std::end(c));

// //     auto offset = GENERATE_COPY(range(as<std::size_t>{}, 0, std::distance(begin,
// //     end)));

// //     SECTION("std::find") {
// //         auto target = begin + offset;
// //         auto result = std::find(begin, end, *target);
// //         REQUIRE(result == target);
// //     }
// // }

// REGISTER_TYPED_TEST_SUITE_P(ZipIteratorTest, IsNotDefaultConstructible,
//                             IsCopyConstructible, IsCopyAssignable, IsMoveConstructible,
//                             IsMoveAssignable, IsDestructible, IsSwappable);

REGISTER_TYPED_TEST_SUITE_P(ForwardInterface, OperatorDereference,
                            OperatorIncrementPrefix);

// REGISTER_TYPED_TEST_SUITE_P(BidirectionalInterfaceTest,
//     );

REGISTER_TYPED_TEST_SUITE_P(RandomAccessInterface, OperatorMinusIterator,
                            OperatorPlusIntegral, OperatorSubscript,
                            // OperatorIncrementPrefix,
                            OperatorPlusEqualIntegral);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

using ForwardCategoryTypes =
    ::testing::Types<std::forward_iterator_tag, std::bidirectional_iterator_tag,
                     std::random_access_iterator_tag, zip::offset_iterator_tag>;
using BidirectionalCategoryTypes =
    ::testing::Types<std::bidirectional_iterator_tag, std::random_access_iterator_tag,
                     zip::offset_iterator_tag>;
using RandomAccessCategoryTypes =
    ::testing::Types<std::random_access_iterator_tag, zip::offset_iterator_tag>;

INSTANTIATE_TYPED_TEST_SUITE_P(ZipIterator, ForwardInterface, ForwardCategoryTypes);
// INSTANTIATE_TYPED_TEST_SUITE_P(ZipBidirectional, BidirectionalInterfaceTest,
// BidirectionalCategoryTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(ZipIterator, RandomAccessInterface,
                               RandomAccessCategoryTypes);

#pragma GCC diagnostic pop