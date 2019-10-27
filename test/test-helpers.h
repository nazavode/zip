#ifndef ZIP_TEST_HELPERS_H_INCLUDED_20191008
#define ZIP_TEST_HELPERS_H_INCLUDED_20191008

#include <vector>
#include <cstdint>
#include <iterator>
#include <tuple>
#include <utility>
#include <type_traits>
#include <forward_list>
#include <list>

#include <zip.h>
#include <gtest/gtest.h>

// Googletest templated test suites

// ForwardIterator Concept
// https://en.cppreference.com/w/cpp/named_req/ForwardIterator
template <typename IteratorCategory>
struct ForwardInterface : public ::testing::Test {};
TYPED_TEST_SUITE_P(ForwardInterface);

// BidirectionalIterator Concept
// https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
template <typename IteratorCategory>
struct BidirectionalInterface : public ::testing::Test {};
TYPED_TEST_SUITE_P(BidirectionalInterface);

// RandomAccessIterator Concept
// https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
template <typename IteratorCategory>
struct RandomAccessInterface : public ::testing::Test {};
TYPED_TEST_SUITE_P(RandomAccessInterface);

namespace zip::test {

//
// Helper: containers()
//

auto containers();

inline auto containers(std::forward_iterator_tag) {
    using ret = std::tuple<std::vector<std::int32_t>, std::forward_list<std::uint64_t>,
                           std::vector<std::int8_t>>;
    return ret{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
               {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
               {0, -1, -2, -3, -4, -5, -6, -7, -8, -9}};
}

inline auto containers(std::bidirectional_iterator_tag) {
    using ret = std::tuple<std::vector<std::int32_t>, std::list<std::uint64_t>,
                           std::vector<std::int8_t>>;
    return ret{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
               {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
               {0, -1, -2, -3, -4, -5, -6, -7, -8, -9}};
}

inline auto containers(std::random_access_iterator_tag) {
    using ret = std::tuple<std::vector<std::int32_t>, std::vector<std::uint64_t>,
                           std::vector<std::int8_t>>;
    return ret{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
               {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
               {0, -1, -2, -3, -4, -5, -6, -7, -8, -9}};
}

inline auto containers(offset_iterator_tag) {
    return containers(std::random_access_iterator_tag{});
}

//
// Helper: begin(ContainerTuple&&)
//

template <typename IteratorCategory, typename ContainerTuple, std::size_t... Indexes>
auto begin_impl(IteratorCategory tag, ContainerTuple&& data,
                std::index_sequence<Indexes...>) {
    return make_iterator(
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
    return make_iterator(
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

}

#endif