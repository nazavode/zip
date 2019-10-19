#include <gtest/gtest.h>
#include <zip.h>

#include <algorithm>
#include <array>
#include <type_traits>

class ZipIteratorTest : public ::testing::Test {
   protected:
    using x_type = std::array<int, 10>;
    using y_type = std::array<long long, 10>;
    using z_type = std::array<signed char, 10>;
    using zip_iterator_type =
        zip::zip_iterator<x_type::iterator, y_type::iterator, z_type::iterator>;

    auto begin() noexcept {
        return zip_iterator_type{std::begin(x), std::begin(y), std::begin(z)};
    }

    auto end() noexcept {
        return zip_iterator_type{std::end(x), std::end(y), std::end(z)};
    }

    auto size() noexcept {
        return std::size(x);
    }

    template<std::size_t I>
    auto sentinel() {
        if constexpr(I == 0) return 11;
        else if constexpr(I == 1) return static_cast<long long>(11);
        else return static_cast<signed char>(-10);
    }

    x_type x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    y_type y{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    z_type z{0, -1, -2, -3, -4, -5, -6, -7, -8, -9};
};

TEST_F(ZipIteratorTest, IsNotDefaultConstructible) {
    EXPECT_FALSE(std::is_default_constructible_v<zip_iterator_type>);
}

TEST_F(ZipIteratorTest, IsCopyConstructible) {
    EXPECT_TRUE(std::is_copy_constructible_v<zip_iterator_type>);
}

TEST_F(ZipIteratorTest, IsCopyAssignable) {
    EXPECT_TRUE(std::is_copy_assignable_v<zip_iterator_type>);
}

TEST_F(ZipIteratorTest, IsMoveConstructible) {
    EXPECT_TRUE(std::is_move_constructible_v<zip_iterator_type>);
}

TEST_F(ZipIteratorTest, IsMoveAssignable) {
    EXPECT_TRUE(std::is_move_assignable_v<zip_iterator_type>);
}

TEST_F(ZipIteratorTest, IsDestructible) {
    EXPECT_TRUE(std::is_destructible_v<zip_iterator_type>);
}

TEST_F(ZipIteratorTest, IsSwappable) {
    EXPECT_TRUE(std::is_swappable_v<zip_iterator_type>);
}

TEST_F(ZipIteratorTest, ConstnessPreservedConst) {
    auto it = zip::zip_iterator{std::cbegin(x), std::cbegin(y), std::cbegin(z)};
    auto [x_item, y_item, z_item] = *it;
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
}

TEST_F(ZipIteratorTest, ConstnessPreservedMut) {
    auto it = zip::zip_iterator{std::begin(x), std::begin(y), std::begin(z)};
    auto [x_item, y_item, z_item] = *it;
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
}

TEST_F(ZipIteratorTest, ConstnessPreservedMixed) {
    auto it = zip::zip_iterator{std::begin(x), std::cbegin(y), std::begin(z)};
    auto [x_item, y_item, z_item] = *it;
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(x_item)>>);
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(y_item)>>);
    EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(z_item)>>);
}

TEST_F(ZipIteratorTest, OperatorDereferenceBegin) {
    // operator*()
    auto value = *begin();
    auto [x_item, y_item, z_item] = value;
    EXPECT_EQ(x_item, *(std::begin(x)));
    EXPECT_EQ(y_item, *(std::begin(y)));
    EXPECT_EQ(z_item, *(std::begin(z)));
}

TEST_F(ZipIteratorTest, OperatorSubscript) {
    // operator[](difference_type)
    auto value = begin()[0];
    auto [x_item, y_item, z_item] = value;
    EXPECT_EQ(x_item, *(std::begin(x)));
    EXPECT_EQ(y_item, *(std::begin(y)));
    EXPECT_EQ(z_item, *(std::begin(z)));
}

TEST_F(ZipIteratorTest, OperatorMinusIterator) {
    // operator-(const zip_iterator&)
    EXPECT_EQ(end() - begin(), size());
}

TEST_F(ZipIteratorTest, OperatorPlusIntegral) {
    // operator+(difference_type)
    auto it = begin() + 1;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::begin(x) + 1));
    EXPECT_EQ(y_item, *(std::begin(y) + 1));
    EXPECT_EQ(z_item, *(std::begin(z) + 1));
}

TEST_F(ZipIteratorTest, OperatorPlusEqualIntegral) {
    // operator+=(difference_type)
    auto it = begin();
    it += 1;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::begin(x) + 1));
    EXPECT_EQ(y_item, *(std::begin(y) + 1));
    EXPECT_EQ(z_item, *(std::begin(z) + 1));
}

TEST_F(ZipIteratorTest, OperatorIncrementPrefix) {
    // operator++()
    auto it = begin();
    auto inc = ++it;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(it, inc);
    EXPECT_EQ(x_item, *(std::begin(x) + 1));
    EXPECT_EQ(y_item, *(std::begin(y) + 1));
    EXPECT_EQ(z_item, *(std::begin(z) + 1));
}

TEST_F(ZipIteratorTest, OperatorIncrementPostfix) {
    // operator++(int)
    auto it = begin();
    auto inc = it++;
    EXPECT_NE(it, inc);
    EXPECT_EQ(inc, begin());
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::begin(x) + 1));
    EXPECT_EQ(y_item, *(std::begin(y) + 1));
    EXPECT_EQ(z_item, *(std::begin(z) + 1));
}

TEST_F(ZipIteratorTest, OperatorMinusIntegral) {
    // operator-(difference_type)
    auto it = end() - 3;
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::end(x) - 3));
    EXPECT_EQ(y_item, *(std::end(y) - 3));
    EXPECT_EQ(z_item, *(std::end(z) - 3));
}

TEST_F(ZipIteratorTest, OperatorMinusEqualIntegral) {
    // operator-=(difference_type)
    auto it = end();
    it -= 3;
    EXPECT_NE(it, end());
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::end(x) - 3));
    EXPECT_EQ(y_item, *(std::end(y) - 3));
    EXPECT_EQ(z_item, *(std::end(z) - 3));
}

TEST_F(ZipIteratorTest, OperatorDecrementPrefix) {
    // operator--()
    auto it = end();
    auto dec = --it;
    EXPECT_EQ(it, dec);
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::end(x) - 1));
    EXPECT_EQ(y_item, *(std::end(y) - 1));
    EXPECT_EQ(z_item, *(std::end(z) - 1));
}

TEST_F(ZipIteratorTest, OperatorDecrementPostfix) {
    // operator--(int)
    auto it = end();
    auto dec = it--;
    EXPECT_NE(it, dec);
    EXPECT_EQ(end(), dec);
    auto [x_item, y_item, z_item] = *it;
    EXPECT_EQ(x_item, *(std::end(x) - 1));
    EXPECT_EQ(y_item, *(std::end(y) - 1));
    EXPECT_EQ(z_item, *(std::end(z) - 1));
}

TEST_F(ZipIteratorTest, OperatorLT) {
    EXPECT_LT(begin(), end());
}

TEST_F(ZipIteratorTest, OperatorLE) {
    EXPECT_LE(begin(), end());
    EXPECT_LE(begin(), begin());
    EXPECT_LE(end(), end());
}

TEST_F(ZipIteratorTest, OperatorGT) {
    EXPECT_GT(end(), begin());
}

TEST_F(ZipIteratorTest, OperatorGE) {
    EXPECT_GE(end(), begin());
    EXPECT_GE(begin(), begin());
    EXPECT_GE(end(), end());
}

TEST_F(ZipIteratorTest, OperatorNE) {
    EXPECT_NE(begin(), end());
}

TEST_F(ZipIteratorTest, OperatorEQ) {
    EXPECT_EQ(begin(), begin());
    EXPECT_EQ(end(), end());
}

TEST_F(ZipIteratorTest, ModifyStdGet) {
    auto value = *begin();
    ASSERT_TRUE(std::is_reference_v<decltype(std::get<0>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(std::get<1>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(std::get<2>(value))>);
    std::get<0>(value) = sentinel<0>();
    EXPECT_EQ(*std::begin(x), sentinel<0>());
    std::get<1>(value) = sentinel<1>();
    EXPECT_EQ(*std::begin(y), sentinel<1>());
    std::get<2>(value) = sentinel<2>();
    EXPECT_EQ(*std::begin(z), sentinel<2>());
}

TEST_F(ZipIteratorTest, ModifyUnqualifiedGet) {
    using std::get;
    auto value = *begin();
    ASSERT_TRUE(std::is_reference_v<decltype(get<0>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(get<1>(value))>);
    ASSERT_TRUE(std::is_reference_v<decltype(get<2>(value))>);
    get<0>(value) = sentinel<0>();
    EXPECT_EQ(*std::begin(x), sentinel<0>());
    get<1>(value) = sentinel<1>();
    EXPECT_EQ(*std::begin(y), sentinel<1>());
    get<2>(value) = sentinel<2>();
    EXPECT_EQ(*std::begin(z), sentinel<2>());
}

TEST_F(ZipIteratorTest, ModifyStructuredBinding) {
    auto [x_bind, y_bind, z_bind] = *begin();
    ASSERT_TRUE(std::is_reference_v<decltype(x_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(y_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(z_bind)>);
    x_bind = sentinel<0>();
    EXPECT_EQ(*std::begin(x), sentinel<0>());
    y_bind = sentinel<1>();
    EXPECT_EQ(*std::begin(y), sentinel<1>());
    z_bind = sentinel<2>();
    EXPECT_EQ(*std::begin(z), sentinel<2>());
}

TEST_F(ZipIteratorTest, ModifyRefStructuredBinding) {
    auto&& [x_bind, y_bind, z_bind] = *begin();
    ASSERT_TRUE(std::is_reference_v<decltype(x_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(y_bind)>);
    ASSERT_TRUE(std::is_reference_v<decltype(z_bind)>);
    x_bind = sentinel<0>();
    EXPECT_EQ(*std::begin(x), sentinel<0>());
    y_bind = sentinel<1>();
    EXPECT_EQ(*std::begin(y), sentinel<1>());
    z_bind = sentinel<2>();
    EXPECT_EQ(*std::begin(z), sentinel<2>());
}

TEST_F(ZipIteratorTest, StdDistance) {
    EXPECT_EQ(std::distance(begin(), end()), size());
}

// TODO

// TEST_F(ZipIteratorTest, StdForEach) {}
// TEST_F(ZipIteratorTest, StdTransform) {}
// TEST_F(ZipIteratorTest, StdFind) {}
// TEST_F(ZipIteratorTest, StdIsSorted) {}
// TEST_F(ZipIteratorTest, StdSort) {}
// TEST_F(ZipIteratorTest, StdNthElement) {}

// TEST_CASE("zip_iterator works with std algorithms", "[zip_iterator]") {
//     std::array<int, 10>         a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     std::array<long long, 10>   b{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     std::array<signed char, 10> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//     auto begin = zip::zip_iterator{std::begin(a), std::begin(b), std::begin(c)};
//     auto end = zip::zip_iterator{std::end(a), std::end(b), std::end(c)};

//     auto offset = GENERATE_COPY(range(as<std::size_t>{}, 0, std::distance(begin,
//     end)));

//     SECTION("std::find") {
//         auto target = begin + offset;
//         auto result = std::find(begin, end, *target);
//         REQUIRE(result == target);
//     }
// }