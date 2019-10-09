#ifndef ZIPIT_H_INCLUDED_20191008
#define ZIPIT_H_INCLUDED_20191008

#include <cassert>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <algorithm>

namespace zipit {

namespace ttl {

namespace detail {

// template <typename Return, typename Tuple, typename UnaryOp, std::size_t... Indexes>
// constexpr Return map_impl(Tuple&& t, UnaryOp&& f, std::index_sequence<Indexes...>) {
//     return {std::forward<UnaryOp>(f)(std::get<Indexes>(std::forward<Tuple>(t)))...};
// }

template <typename Tuple, typename UnaryOp, std::size_t... Indexes>
constexpr void for_each_impl(Tuple&& t, UnaryOp&& f, std::index_sequence<Indexes...>) {
    (std::forward<UnaryOp>(f)(std::get<Indexes>(std::forward<Tuple>(t))), ...);
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate,
          std::size_t... Indexes>
constexpr bool all_impl(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op,
                        std::index_sequence<Indexes...>) {
    return (std::forward<BinaryPredicate>(op)(
                std::get<Indexes>(std::forward<TupleLHS>(lhs)),
                std::get<Indexes>(std::forward<TupleRHS>(rhs))) &&
            ...);
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate,
          std::size_t... Indexes>
constexpr bool any_impl(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op,
                        std::index_sequence<Indexes...>) {
    return (std::forward<BinaryPredicate>(op)(
                std::get<Indexes>(std::forward<TupleLHS>(lhs)),
                std::get<Indexes>(std::forward<TupleRHS>(rhs))) ||
            ...);
}

template <typename Tuple, typename UnaryPredicate, std::size_t... Indexes>
constexpr bool any_impl(Tuple&& t, UnaryPredicate&& op, std::index_sequence<Indexes...>) {
    return (std::forward<UnaryPredicate>(op)(std::get<Indexes>(std::forward<Tuple>(t))) ||
            ...);
}

}  // namespace detail

template <typename Tuple, typename UnaryOp>
constexpr void for_each(Tuple&& t, UnaryOp&& f) {
    using Indexes =
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>;
    detail::for_each_impl(std::forward<Tuple>(t), std::forward<UnaryOp>(f), Indexes{});
}

// template <typename Return, typename Tuple, typename UnaryOp>
// constexpr Return map(Tuple&& t, UnaryOp&& f) {
//     using Indexes =
//         std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>;
//     return detail::map_impl(std::forward<Tuple>(t), std::forward<UnaryOp>(f),
//     Indexes{});
// }

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
bool all(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    using Indexes =
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<TupleLHS>>>;
    return detail::all_impl(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                            std::forward<BinaryPredicate>(op), Indexes{});
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
bool any(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    using Indexes =
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<TupleLHS>>>;
    return detail::any_impl(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                            std::forward<BinaryPredicate>(op), Indexes{});
}

template <typename Tuple, typename UnaryPredicate>
bool any(Tuple&& t, UnaryPredicate&& op) {
    using Indexes =
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>;
    return detail::any_impl(std::forward<Tuple>(t), std::forward<UnaryPredicate>(op),
                            Indexes{});
}

}  // namespace ttl

template<typename ...Iterators/*, typename Indexes = std::make_index_sequence<sizeof...(Iterators)>*/>
struct zip_iterator {
    static constexpr auto arity = sizeof...(Iterators);
    // using value_type = std::tuple<std::reference_wrapper<
    //    std::remove_reference_t<decltype(*std::declval<Iterators>())>>...>;
    using value_type =
        std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using zip_iterator_type = std::tuple<Iterators...>;

    constexpr zip_iterator(
        Iterators... iterators) /*noexcept TODO fix noexcept and const*/
        : m_it{iterators...} {}

    constexpr zip_iterator& operator++() {
        ttl::for_each(m_it, [](auto&& element) { ++element; });
        return *this;
    }

    constexpr bool operator==(const zip_iterator& other) const {
        // return !operator!=(other);
        // Equivalent to:
        // !(a != a' && b != b' && ...)
        return ttl::any(m_it, other.m_it, std::equal_to{});
    }

    constexpr bool operator!=(const zip_iterator& other) const {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return ttl::all(m_it, other.m_it, std::not_equal_to{});
    }

    constexpr value_type operator*() const {
        return dereference_impl(std::make_index_sequence<arity>{});
    }

    constexpr value_type operator[](std::size_t i) const {
        return subscript_impl(i, std::make_index_sequence<arity>{});
    }

   private:
    zip_iterator_type m_it;

    template <std::size_t... Indexes>
    constexpr value_type dereference_impl(std::index_sequence<Indexes...>) const {
        return {*std::get<Indexes>(m_it)...};
    }

    template <std::size_t... Indexes>
    constexpr value_type subscript_impl(std::size_t i,
                                        std::index_sequence<Indexes...>) const {
        return {std::get<Indexes>(m_it)[i]...};
    }
};

template <typename... Sequences>
struct zip {
    static constexpr auto arity = sizeof...(Sequences);
    using iterator =
        // Why the & is needed here:
        // https://stackoverflow.com/questions/42580761/why-does-stdbegin-always-return-const-iterator-in-such-a-case
        zip_iterator<decltype(std::begin(std::declval<Sequences&>()))...>;
    // This type wraps a reference to each sequence on which has been
    // instantiated:
    using sequences = std::tuple<Sequences&...>;

    // using size_type = std::common_type_t<decltype(std::size(std::declval<Sequences&>()))...>;
    using size_type = std::size_t;

    constexpr zip(Sequences&... sqs) noexcept : m_seq{sqs...} {}

    constexpr iterator begin() { return begin_impl(std::make_index_sequence<arity>{}); }

    constexpr iterator end() { return end_impl(std::make_index_sequence<arity>{}); }

    constexpr size_type size() const noexcept {
        return size_impl(std::make_index_sequence<arity>{});
    }

    // template<std::size_t N>
    // friend constexpr decltype(auto) get() noexcept {
    //     return std::get<N>(m_seq);
    // }

   private:   
    template <std::size_t... Indexes>
    constexpr iterator begin_impl(std::index_sequence<Indexes...>) {
        return {std::begin(std::get<Indexes>(m_seq))...};
    }

    template <std::size_t... Indexes>
    constexpr iterator end_impl(std::index_sequence<Indexes...>) {
        return {std::end(std::get<Indexes>(m_seq))...};
    }

    template <std::size_t... Indexes>
    constexpr size_type size_impl(std::index_sequence<Indexes...>) const noexcept {
        return std::min(std::size(std::get<Indexes>(m_seq))...);
    }

    sequences m_seq;
};

// template <typename Zip>
// void print(Zip z) {
//   std::cout << "a|b|c" << std::endl;
//   for (auto [a, b, c] : z) {
//     std::cout << a << "|" << b << "|" << c << std::endl;
//   }
// }

}  // namespace zipit

#endif