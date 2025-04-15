#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <zip.h>

#include <algorithm>
#include <array>
#include <forward_list>
#include <iterator>
#include <limits>
#include <list>
#include <type_traits>
#include <vector>

#include "test-helpers.h"

// Matchers
using ::testing::Each;

// Helpers
using zip::test::begin;
using zip::test::containers;
using zip::test::end;
using zip::test::size;

TEST(MakeIterator, IteratorCategoryRandomAccess) {
    std::array<int, 10> a;
    std::vector<long long> b;
    std::array<signed char, 10> c;
    int d[20];
    auto it =
        zip::make_iterator(std::begin(a), std::begin(b), std::begin(c), std::begin(d));
    EXPECT_TRUE((std::is_same_v<decltype(it)::iterator_category,
                                std::random_access_iterator_tag>));
}

TEST(MakeIterator, IteratorCategoryBidirectional) {
    std::array<int, 10> a;
    std::list<int> b;
    std::vector<long long> c;
    auto it = zip::make_iterator(std::begin(a), std::begin(b), std::begin(c));
    EXPECT_TRUE((std::is_same_v<decltype(it)::iterator_category,
                                std::bidirectional_iterator_tag>));
}

TEST(MakeIterator, IteratorCategoryForward) {
    std::array<int, 10> a;
    std::forward_list<int> b;
    std::vector<long long> c;
    std::list<int> d;
    auto it =
        zip::make_iterator(std::begin(a), std::begin(b), std::begin(c), std::begin(d));
    EXPECT_TRUE(
        (std::is_same_v<decltype(it)::iterator_category, std::forward_iterator_tag>));
}

TEST(MakeIterator, ConstnessPreserved) {
    auto data = containers(std::forward_iterator_tag{});
    auto [x, y, z] = data;
    auto it = zip::make_iterator(std::begin(x), std::cbegin(y), std::begin(z));
    auto [x_item, y_item, z_item] = *it;
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
}

TEST(MakeIterator, ConstnessPreservedConst) {
    auto data = containers(std::forward_iterator_tag{});
    auto [x, y, z] = data;
    auto it = zip::make_iterator(std::cbegin(x), std::cbegin(y), std::cbegin(z));
    auto [x_item, y_item, z_item] = *it;
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
}

TEST(MakeIterator, ConstnessPreservedMut) {
    auto data = containers(std::forward_iterator_tag{});
    auto [x, y, z] = data;
    auto it = zip::make_iterator(std::begin(x), std::begin(y), std::begin(z));
    auto [x_item, y_item, z_item] = *it;
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
}

///////////////////////////////////////////////////////////
// ForwardIterator Concept
// https://en.cppreference.com/w/cpp/named_req/ForwardIterator
///////////////////////////////////////////////////////////

TYPED_TEST_P(ForwardInterface, IsNotDefaultConstructible) {
    using iterator_type = decltype(begin(TypeParam{}, containers(TypeParam{})));
    EXPECT_FALSE(std::is_default_constructible_v<iterator_type>);
}

TYPED_TEST_P(ForwardInterface, IsCopyConstructible) {
    using iterator_type = decltype(begin(TypeParam{}, containers(TypeParam{})));
    EXPECT_TRUE(std::is_copy_constructible_v<iterator_type>);
}

TYPED_TEST_P(ForwardInterface, IsCopyAssignable) {
    using iterator_type = decltype(begin(TypeParam{}, containers(TypeParam{})));
    EXPECT_TRUE(std::is_copy_assignable_v<iterator_type>);
}

TYPED_TEST_P(ForwardInterface, IsMoveConstructible) {
    using iterator_type = decltype(begin(TypeParam{}, containers(TypeParam{})));
    EXPECT_TRUE(std::is_move_constructible_v<iterator_type>);
}

TYPED_TEST_P(ForwardInterface, IsMoveAssignable) {
    using iterator_type = decltype(begin(TypeParam{}, containers(TypeParam{})));
    EXPECT_TRUE(std::is_move_assignable_v<iterator_type>);
}

TYPED_TEST_P(ForwardInterface, IsDestructible) {
    using iterator_type = decltype(begin(TypeParam{}, containers(TypeParam{})));
    EXPECT_TRUE(std::is_destructible_v<iterator_type>);
}

TYPED_TEST_P(ForwardInterface, IsSwappable) {
    using iterator_type = decltype(begin(TypeParam{}, containers(TypeParam{})));
    EXPECT_TRUE(std::is_swappable_v<iterator_type>);
}

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

TYPED_TEST_P(ForwardInterface, OperatorIncrementPostfix) {
    // operator++(int)
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = begin(tag, data);
    auto inc = it++;
    EXPECT_NE(it, inc);
    EXPECT_EQ(inc, begin(tag, data));
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(++std::begin(std::get<0>(data))));
    EXPECT_EQ(y_item, *(++std::begin(std::get<1>(data))));
    EXPECT_EQ(z_item, *(++std::begin(std::get<2>(data))));
}

TYPED_TEST_P(ForwardInterface, OperatorNE) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it_begin = begin(tag, data);
    auto it_end = end(tag, data);
    EXPECT_NE(it_begin, it_end);
}

TYPED_TEST_P(ForwardInterface, OperatorEQ) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it_begin = begin(tag, data);
    auto it_end = end(tag, data);
    EXPECT_EQ(it_begin, it_begin);
    EXPECT_EQ(it_end, it_end);
}

TYPED_TEST_P(ForwardInterface, ModifyStdGet) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto value = *begin(tag, data);

    ASSERT_TRUE(std::is_reference_v<decltype(std::get<0>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(std::get<1>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(std::get<2>(value))>);

    auto x_sentinel = std::numeric_limits<
        std::remove_reference_t<std::tuple_element_t<0, decltype(value)>>>::max();
    auto y_sentinel = std::numeric_limits<
        std::remove_reference_t<std::tuple_element_t<1, decltype(value)>>>::max();
    auto z_sentinel = std::numeric_limits<
        std::remove_reference_t<std::tuple_element_t<2, decltype(value)>>>::max();

    std::get<0>(value) = x_sentinel;
    EXPECT_EQ(*std::begin(std::get<0>(data)), x_sentinel);
    std::get<1>(value) = y_sentinel;
    EXPECT_EQ(*std::begin(std::get<1>(data)), y_sentinel);
    std::get<2>(value) = z_sentinel;
    EXPECT_EQ(*std::begin(std::get<2>(data)), z_sentinel);
}

TYPED_TEST_P(ForwardInterface, ModifyUnqualifiedGet) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto value = *begin(tag, data);

    using std::get;

    ASSERT_TRUE(std::is_reference_v<decltype(get<0>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(get<1>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(get<2>(value))>);

    auto x_sentinel = std::numeric_limits<
        std::remove_reference_t<std::tuple_element_t<0, decltype(value)>>>::max();
    auto y_sentinel = std::numeric_limits<
        std::remove_reference_t<std::tuple_element_t<1, decltype(value)>>>::max();
    auto z_sentinel = std::numeric_limits<
        std::remove_reference_t<std::tuple_element_t<2, decltype(value)>>>::max();

    get<0>(value) = x_sentinel;
    EXPECT_EQ(*std::begin(std::get<0>(data)), x_sentinel);
    get<1>(value) = y_sentinel;
    EXPECT_EQ(*std::begin(std::get<1>(data)), y_sentinel);
    get<2>(value) = z_sentinel;
    EXPECT_EQ(*std::begin(std::get<2>(data)), z_sentinel);
}

TYPED_TEST_P(ForwardInterface, ModifyStructuredBinding) {
    auto tag = TypeParam{};
    auto data = containers(tag);

    auto [x_bind, y_bind, z_bind] = *begin(tag, data);
    ASSERT_TRUE(std::is_reference_v<decltype(x_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(y_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(z_bind)>);

    auto x_sentinel =
        std::numeric_limits<std::remove_reference_t<decltype(x_bind)>>::max();
    auto y_sentinel =
        std::numeric_limits<std::remove_reference_t<decltype(y_bind)>>::max();
    auto z_sentinel =
        std::numeric_limits<std::remove_reference_t<decltype(z_bind)>>::max();

    x_bind = x_sentinel;
    EXPECT_EQ(*std::begin(std::get<0>(data)), x_sentinel);
    y_bind = y_sentinel;
    EXPECT_EQ(*std::begin(std::get<1>(data)), y_sentinel);
    z_bind = z_sentinel;
    EXPECT_EQ(*std::begin(std::get<2>(data)), z_sentinel);
}

TYPED_TEST_P(ForwardInterface, ModifyRefStructuredBinding) {
    auto tag = TypeParam{};
    auto data = containers(tag);

    auto&& [x_bind, y_bind, z_bind] = *begin(tag, data);
    ASSERT_TRUE(std::is_reference_v<decltype(x_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(y_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(z_bind)>);

    auto x_sentinel =
        std::numeric_limits<std::remove_reference_t<decltype(x_bind)>>::max();
    auto y_sentinel =
        std::numeric_limits<std::remove_reference_t<decltype(y_bind)>>::max();
    auto z_sentinel =
        std::numeric_limits<std::remove_reference_t<decltype(z_bind)>>::max();

    x_bind = x_sentinel;
    EXPECT_EQ(*std::begin(std::get<0>(data)), x_sentinel);
    y_bind = y_sentinel;
    EXPECT_EQ(*std::begin(std::get<1>(data)), y_sentinel);
    z_bind = z_sentinel;
    EXPECT_EQ(*std::begin(std::get<2>(data)), z_sentinel);
}

TYPED_TEST_P(ForwardInterface, StdForEach) {
    auto tag = TypeParam{};
    auto data = containers(tag);

    auto& [x, y, z] = data;
    using x_value_type = std::remove_reference_t<decltype(*std::begin(x))>;
    using y_value_type = std::remove_reference_t<decltype(*std::begin(y))>;
    using z_value_type = std::remove_reference_t<decltype(*std::begin(z))>;

    auto x_sentinel = std::numeric_limits<x_value_type>::max();
    auto y_sentinel = std::numeric_limits<y_value_type>::max();
    auto z_sentinel = std::numeric_limits<z_value_type>::max();

    std::for_each(begin(tag, data), end(tag, data), [=](auto&& e) {
        std::get<0>(e) = x_sentinel;
        std::get<1>(e) = y_sentinel;
        std::get<2>(e) = z_sentinel;
    });
    ASSERT_THAT(x, Each(x_sentinel));
    ASSERT_THAT(y, Each(y_sentinel));
    ASSERT_THAT(z, Each(z_sentinel));
}

TYPED_TEST_P(ForwardInterface, StdForEachStructuredBinding) {
    auto tag = TypeParam{};
    auto data = containers(tag);

    auto& [x, y, z] = data;
    using x_value_type = std::remove_reference_t<decltype(*std::begin(x))>;
    using y_value_type = std::remove_reference_t<decltype(*std::begin(y))>;
    using z_value_type = std::remove_reference_t<decltype(*std::begin(z))>;

    auto x_sentinel = std::numeric_limits<x_value_type>::max();
    auto y_sentinel = std::numeric_limits<y_value_type>::max();
    auto z_sentinel = std::numeric_limits<z_value_type>::max();

    std::for_each(begin(tag, data), end(tag, data), [=](auto&& e) {
        auto [xx, yy, zz] = e;
        xx = x_sentinel;
        yy = y_sentinel;
        zz = z_sentinel;
    });
    ASSERT_THAT(x, Each(x_sentinel));
    ASSERT_THAT(y, Each(y_sentinel));
    ASSERT_THAT(z, Each(z_sentinel));
}

// clang-format off
REGISTER_TYPED_TEST_SUITE_P(ForwardInterface,
    IsNotDefaultConstructible,
    IsCopyConstructible,
    IsCopyAssignable,
    IsMoveConstructible,
    IsMoveAssignable,
    IsDestructible,
    IsSwappable,
    OperatorDereference,
    OperatorIncrementPrefix,
    OperatorIncrementPostfix,
    OperatorNE,
    OperatorEQ,
    ModifyStdGet,
    ModifyUnqualifiedGet,
    ModifyStructuredBinding,
    ModifyRefStructuredBinding,
    StdForEach,
    StdForEachStructuredBinding);
// clang-format on

///////////////////////////////////////////////////////////
// BidirectionalIterator Concept
// https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
///////////////////////////////////////////////////////////

TYPED_TEST_P(BidirectionalInterface, OperatorDecrementPrefix) {
    // operator--()
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = end(tag, data);
    auto dec = --it;
    EXPECT_EQ(it, dec);
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(--std::end(std::get<0>(data))));
    EXPECT_EQ(y_item, *(--std::end(std::get<1>(data))));
    EXPECT_EQ(z_item, *(--std::end(std::get<2>(data))));
}

TYPED_TEST_P(BidirectionalInterface, OperatorDecrementPostfix) {
    // operator--(int)
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = end(tag, data);
    auto dec = it--;
    EXPECT_NE(it, dec);
    EXPECT_EQ(end(tag, data), dec);
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(--std::end(std::get<0>(data))));
    EXPECT_EQ(y_item, *(--std::end(std::get<1>(data))));
    EXPECT_EQ(z_item, *(--std::end(std::get<2>(data))));
}

// clang-format off
REGISTER_TYPED_TEST_SUITE_P(BidirectionalInterface,
    OperatorDecrementPrefix,
    OperatorDecrementPostfix);
// clang-format on

///////////////////////////////////////////////////////////
// RandomAccessIterator Concept
// https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
///////////////////////////////////////////////////////////

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

TYPED_TEST_P(RandomAccessInterface, OperatorMinusIntegral) {
    // operator-(difference_type)
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = end(tag, data) - 3;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::end(std::get<0>(data)) - 3));
    EXPECT_EQ(y_item, *(std::end(std::get<1>(data)) - 3));
    EXPECT_EQ(z_item, *(std::end(std::get<2>(data)) - 3));
}

TYPED_TEST_P(RandomAccessInterface, OperatorMinusEqualIntegral) {
    // operator-=(difference_type)
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it = end(tag, data);
    it -= 3;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::end(std::get<0>(data)) - 3));
    EXPECT_EQ(y_item, *(std::end(std::get<1>(data)) - 3));
    EXPECT_EQ(z_item, *(std::end(std::get<2>(data)) - 3));
}

TYPED_TEST_P(RandomAccessInterface, OperatorLT) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it_begin = begin(tag, data);
    auto it_end = end(tag, data);
    EXPECT_LT(it_begin, it_end);
}

TYPED_TEST_P(RandomAccessInterface, OperatorLE) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it_begin = begin(tag, data);
    auto it_end = end(tag, data);
    EXPECT_LE(it_begin, it_end);
    EXPECT_LE(it_begin, it_begin);
    EXPECT_LE(it_end, it_end);
}

TYPED_TEST_P(RandomAccessInterface, OperatorGT) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it_begin = begin(tag, data);
    auto it_end = end(tag, data);
    EXPECT_GT(it_end, it_begin);
}

TYPED_TEST_P(RandomAccessInterface, OperatorGE) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it_begin = begin(tag, data);
    auto it_end = end(tag, data);
    EXPECT_GE(it_end, it_begin);
    EXPECT_GE(it_begin, it_begin);
    EXPECT_GE(it_end, it_end);
}

TYPED_TEST_P(RandomAccessInterface, StdDistance) {
    auto tag = TypeParam{};
    auto data = containers(tag);
    auto it_begin = begin(tag, data);
    auto it_end = end(tag, data);
    EXPECT_EQ(std::distance(it_begin, it_end), size(data));
}

// clang-format off
REGISTER_TYPED_TEST_SUITE_P(RandomAccessInterface,
    OperatorMinusIterator,
    OperatorPlusIntegral,
    OperatorSubscript,
    OperatorPlusEqualIntegral,
    OperatorMinusIntegral,
    OperatorMinusEqualIntegral,
    OperatorLT,
    OperatorLE,
    OperatorGT,
    OperatorGE,
    StdDistance);
// clang-format on

// TODO

// TYPED_TEST_P(ZipIteratorTest, StdTransform) {}
// TYPED_TEST_P(ZipIteratorTest, StdFind) {}
// TYPED_TEST_P(ZipIteratorTest, StdIsSorted) {}
// TYPED_TEST_P(ZipIteratorTest, StdSort) {}
// TYPED_TEST_P(ZipIteratorTest, StdNthElement) {}

// clang-format off
using ForwardCategoryTypes =
    ::testing::Types<
        std::forward_iterator_tag,
        std::bidirectional_iterator_tag,
        std::random_access_iterator_tag,
        zip::offset_iterator_tag>;

using BidirectionalCategoryTypes =
    ::testing::Types<
        std::bidirectional_iterator_tag,
        std::random_access_iterator_tag,
        zip::offset_iterator_tag>;

using RandomAccessCategoryTypes =
    ::testing::Types<
        std::random_access_iterator_tag,
        zip::offset_iterator_tag>;

// Googletest macros doesn't support -Wall -Werror (wtf?),
// so users should expect breaks:
// https://github.com/google/googletest/pull/2316#issuecomment-518269259
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

INSTANTIATE_TYPED_TEST_SUITE_P(ZipIterator,
    ForwardInterface, ForwardCategoryTypes);

INSTANTIATE_TYPED_TEST_SUITE_P(ZipIterator, 
    BidirectionalInterface, BidirectionalCategoryTypes);

INSTANTIATE_TYPED_TEST_SUITE_P(ZipIterator,
    RandomAccessInterface, RandomAccessCategoryTypes);

#ifdef __clang__
#pragma clang diagnostic pop
#endif
// clang-format on
