#ifndef ZIP_H_INCLUDED_20191008
#define ZIP_H_INCLUDED_20191008

#include <cassert>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <iterator>

namespace zip {

// clang-format off

namespace ttl {

namespace detail {

template<typename T>
using indexes_t = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>;

template <typename Return, typename Tuple, typename UnaryOp, std::size_t... Indexes>
constexpr Return transform_impl(Tuple&& t, UnaryOp&& f, std::index_sequence<Indexes...>) {
    return {
        std::forward<UnaryOp>(f)(std::get<Indexes>(std::forward<Tuple>(t)))...};
}

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
                std::get<Indexes>(std::forward<TupleRHS>(rhs))) && ...);
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate,
          std::size_t... Indexes>
constexpr bool any_impl(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op,
                        std::index_sequence<Indexes...>) {
    return (std::forward<BinaryPredicate>(op)(
                std::get<Indexes>(std::forward<TupleLHS>(lhs)),
                std::get<Indexes>(std::forward<TupleRHS>(rhs))) || ...);
}

template <typename Tuple, typename UnaryPredicate, std::size_t... Indexes>
constexpr bool any_impl(Tuple&& t, UnaryPredicate&& op, std::index_sequence<Indexes...>) {
    return (std::forward<UnaryPredicate>(op)(
                std::get<Indexes>(std::forward<Tuple>(t))) || ...);
}

template<typename TupleLHS, typename TupleRHS,
         typename SumNaryOp, typename ProdBinaryOp, std::size_t... Indexes>
constexpr auto inner_product_impl(TupleLHS&& lhs, TupleRHS&& rhs,
                                  SumNaryOp&& sum, ProdBinaryOp&& prod, std::index_sequence<Indexes...>) {
    return std::forward<SumNaryOp>(sum)(
               std::forward<ProdBinaryOp>(prod)(
                   std::get<Indexes>(std::forward<TupleLHS>(lhs)),
                   std::get<Indexes>(std::forward<TupleRHS>(rhs)))...);
}

}  // namespace detail

template <typename Tuple, typename UnaryOp>
constexpr void for_each(Tuple&& t, UnaryOp&& f) {
    detail::for_each_impl(std::forward<Tuple>(t), std::forward<UnaryOp>(f),
                          detail::indexes_t<Tuple>{});
}

template <typename Return, typename Tuple, typename UnaryOp>
constexpr auto transform(Tuple&& t, UnaryOp&& f) {
    return detail::transform_impl<Return>(std::forward<Tuple>(t), std::forward<UnaryOp>(f),
                                          detail::indexes_t<Tuple>{});
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
constexpr bool all(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    return detail::all_impl(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                            std::forward<BinaryPredicate>(op), detail::indexes_t<TupleLHS>{});
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
constexpr bool any(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    return detail::any_impl(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                            std::forward<BinaryPredicate>(op), detail::indexes_t<TupleLHS>{});
}

template <typename Tuple, typename UnaryPredicate>
constexpr bool any(Tuple&& t, UnaryPredicate&& op) {
    return detail::any_impl(std::forward<Tuple>(t), std::forward<UnaryPredicate>(op),
                            detail::indexes_t<Tuple>{});
}

template<typename TupleLHS, typename TupleRHS,
         typename SumNaryOp, typename ProdBinaryOp>
constexpr auto inner_product(TupleLHS&& lhs, TupleRHS&& rhs, SumNaryOp&& sum, ProdBinaryOp&& prod) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    return detail::inner_product_impl(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                                      std::forward<SumNaryOp>(sum), std::forward<ProdBinaryOp>(prod),
                                      detail::indexes_t<TupleLHS>{});
}

}  // namespace ttl

template <typename... Iterators>
struct iterator_base {
    
    using iterators_tuple_type = std::tuple<std::remove_reference_t<Iterators>...>;

    using value_type = std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using difference_type = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
    using reference = value_type;
    using pointer = value_type;

    explicit constexpr iterator_base(Iterators... iterators) noexcept
        : m_iterators{iterators...} {}

    protected:

    constexpr iterator_tuple_type& iterators() & noexcept { return m_iterators; }

    constexpr iterator_tuple_type const& iterators() const& noexcept {
        return m_iterators;
    }

    private:
    iterator_tuple_type m_iterators;
};

struct FullRangePredication {

    template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
    static constexpr bool any(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) noexcept {
        static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                      std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
        return ttl::detail::any_impl(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                                std::forward<BinaryPredicate>(op), ttl::detail::indexes_t<TupleLHS>{});
    }

    template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
    static constexpr bool all(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) {
        static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                      std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
        return ttl::detail::all_impl(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                                std::forward<BinaryPredicate>(op), ttl::detail::indexes_t<TupleLHS>{});
    }
};

template<std::size_t N>
struct PartialRangePredication {

    template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
    static constexpr bool any(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) noexcept {
        static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                    std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
        return std::forward<BinaryPredicate>(op)(
            std::get<N>(std::forward<TupleLHS>(lhs)),
            std::get<N>(std::forward<TupleRHS>(rhs)));
    }

    template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
    static constexpr bool all(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) noexept {
        static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                    std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
        return std::forward<BinaryPredicate>(op)(
            std::get<N>(std::forward<TupleLHS>(lhs)),
            std::get<N>(std::forward<TupleRHS>(rhs)));
    }
};

template<typename PredicationPolicy, typename ...Iterators>
struct bidirectional_iterator : public iterator_base<Iterators...> {

    using iterator_category = std::bidirectional_iterator_tag;
    using predication_policy = PredicationPolicy;
    
    using iterator_base<Iterators...>::iterator_base;
    
    constexpr bidirectional_iterator operator+(difference_type rhs) const {
        return std::make_from_tuple<bidirectional_iterator>(
            ttl::transform<iterators_tuple_type>(iterators(), [rhs](auto&& it) {
                return it + rhs;
            }));
    }

    constexpr bidirectional_iterator operator-(difference_type rhs) const {
        return std::make_from_tuple<bidirectional_iterator>(
            ttl::transform<iterators_tuple_type>(iterators(), [rhs](auto&& it) {
                return it - rhs;
            }));
    }

    // TODO substitute lambdas with std function objects

    constexpr difference_type operator+(const bidirectional_iterator& other) const {
        return ttl::inner_product(iterators(), other.iterators(),
                                  [](auto&& ...prods) { return std::min({prods...}); },
                                  [](auto&& lhs, auto&& rhs) { return lhs + rhs; });
    }

    constexpr difference_type operator-(const bidirectional_iterator& other) const {
        return ttl::inner_product(iterators(), other.iterators(),
                                  [](auto&& ...prods) { return std::min({prods...}); },
                                  [](auto&& lhs, auto&& rhs) { return lhs - rhs; });
    }

    constexpr bidirectional_iterator& operator++() {
        ttl::for_each(iterators(), [](auto&& it) { ++it; });
        return *this;
    }

    constexpr bidirectional_iterator& operator++(int) {
        bidirectional_iterator prev{*this};
        ttl::for_each(iterators(), [](auto&& it) { ++it; });
        return prev;
    }

    constexpr bidirectional_iterator& operator--() {
        ttl::for_each(iterators(), [](auto&& it) { --it; });
        return *this;
    }

    constexpr bidirectional_iterator& operator--(int) {
        bidirectional_iterator prev{*this};
        ttl::for_each(iterators(), [](auto&& it) { --it; });
        return prev;
    }

    constexpr bidirectional_iterator& operator+=(difference_type rhs) {
        ttl::for_each(iterators(), [rhs](auto&& it) { it += rhs; });
        return *this;
    }

    constexpr bidirectional_iterator& operator-=(difference_type rhs) {
        ttl::for_each(iterators(), [rhs](auto&& it) { it -= rhs; });
        return *this;
    }

    constexpr bool operator<(const bidirectional_iterator& rhs) const {
        return predication_policy::any(iterators(), rhs.iterators(), std::less{});
    }

    constexpr bool operator<=(const bidirectional_iterator& rhs) const {
        return predication_policy::any(iterators(), rhs.iterators(), std::less_equal{});
    }

    constexpr bool operator>(const bidirectional_iterator& rhs) const {
        return predication_policy::any(iterators(), rhs.iterators(), std::greater{});
    }

    constexpr bool operator>=(const bidirectional_iterator& rhs) const {
        return predication_policy::any(iterators(), rhs.iterators(), std::greater_equal{});
    }

    constexpr bool operator==(const bidirectional_iterator& rhs) const {
        // Equivalent to:
        // !(a != a' && b != b' && ...)
        return predication_policy::any(iterators(), rhs.iterators(), std::equal_to{});
    }

    constexpr bool operator!=(const bidirectional_iterator& rhs) const {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return predication_policy::all(iterators(), rhs.iterators(), std::not_equal_to{});
    }

    constexpr value_type operator*() const {
        // WARNING: decltype(auto) is vital here, otherwise ttl::transform
        // is going to construct and return a tuple of values (the ones
        // returned by this lambda) preventing the caller from modifying
        // actual values underlying the zipped iterators
        return ttl::transform<value_type>(iterators(), [](auto it) -> decltype(auto) {
            return *it;
        });
    }
};

template<typename PredicationPolicy, typename ...Iterators>
struct random_access_iterator : public iterator_base<Iterators...> {

    using iterator_category = std::random_access_iterator_tag;
    using predication_policy = PredicationPolicy;
    
    using iterator_base<Iterators...>::iterator_base;

    constexpr iterator operator+(difference_type rhs) const noexcept {
        iterator ret{*this};
        ret.m_offset += rhs;
        return ret;
    }

    constexpr iterator operator-(difference_type rhs) const noexcept {
        iterator ret{*this};
        ret.m_offset -= rhs;
        return ret;
    }

    constexpr difference_type operator-(const iterator& other) const noexcept {
        return ttl::inner_product(iterators(), other.iterators(),
                                  [](auto&& ...prods) { return std::min({prods...}); },
                                  [lhs_offset=m_offset, rhs_offset=other.m_offset](auto&& lhs, auto&& rhs) {
                                      return (lhs + lhs_offset) - (rhs + rhs_offset); });
    }

    constexpr iterator& operator++() noexcept {
        ++m_offset;
        return *this;
    }

    constexpr iterator operator++(int) noexcept {
        iterator ret{*this};
        ++m_offset;
        return ret;
    }

    constexpr iterator& operator--() noexcept {
        --m_offset;
        return *this;
    }

    constexpr iterator operator--(int) noexcept {
        iterator ret{*this};
        --m_offset;
        return ret;
    }

    constexpr iterator& operator+=(difference_type rhs) noexcept {
        m_offset += rhs;
        return *this;
    }

    constexpr iterator& operator-=(difference_type rhs) noexcept {
        m_offset -= rhs;
        return *this;
    }

    constexpr bool operator<(const iterator& rhs) const noexcept {
        return predication_policy::any(iterators(), rhs.iterators(),
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) < (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator<=(const iterator& rhs) const noexcept {
        return predication_policy::any(iterators(), rhs.iterators(),
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) <= (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator>(const iterator& rhs) const noexcept {
        return predication_policy::any(iterators(), rhs.iterators(),
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) > (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator>=(const iterator& rhs) const noexcept {
        return predication_policy::any(iterators(), rhs.iterators(),
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) >= (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator==(const iterator& rhs) const noexcept {
        // Equivalent to:
        // !(a != a' && b != b' && ...) ==
        //   a == a' || b == b' || ...
        return predication_policy::any(iterators(), rhs.iterators(),
                        [lhs_offset=m_offset, rhs_offset=rhs.m_offset](auto&& lhs_it, auto&& rhs_it){
                            return (lhs_it + lhs_offset) == (rhs_it + rhs_offset);
                        });
    }

    constexpr bool operator!=(const iterator& rhs) const noexcept {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return predication_policy::all(iterators(), rhs.iterators(),
                        [lhs_offset=m_offset, rhs_offset=rhs.m_offset](auto&& lhs_it, auto&& rhs_it){
                            return (lhs_it + lhs_offset) != (rhs_it + rhs_offset);
                        });
    }

    constexpr value_type operator*() const noexcept {
        return operator[](difference_type{0});
    }

    constexpr value_type operator[](difference_type rhs) const noexcept {
        rhs += m_offset;
        return ttl::transform<value_type>(iterators(), [rhs](auto&& it) -> decltype(auto) {
            return it[rhs];
        });
    }

   private:

    difference_type m_offset{0};
};

// clang-format on

template<typename PredicationPolicy, typename... Iterators>
struct iterator : public std::conditional_t<
    std::conjunction_v<std::is_same_v<
        typename std::iterator_traits<Iterators>::iterator_category,
        std::random_access_iterator_tag>...>,
    random_access_iterator<PredicationPolicy, Iterators...>,
    bidirectional_iterator<PredicationPolicy, Iterators...>> {};

}  // namespace zip

#endif
