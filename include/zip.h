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

template<typename ...Iterators>
struct zip_iterator {

    static_assert((
        std::is_same_v<
            typename std::iterator_traits<Iterators>::iterator_category,
            std::random_access_iterator_tag> && ...),
        "zip_iterator supports only random access iterators");

    using iterators_tuple_type = std::tuple<std::remove_reference_t<Iterators>...>;

    using value_type = std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using difference_type = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
    using reference = value_type;
    using pointer = value_type;
    using iterator_category = std::random_access_iterator_tag;
    
    explicit constexpr zip_iterator(Iterators... iterators) noexcept
        : m_it{iterators...} {}

    constexpr zip_iterator operator+(difference_type rhs) const {
        return std::make_from_tuple<zip_iterator>(
            ttl::transform<iterators_tuple_type>(m_it, [rhs](auto&& it) {
                return it + rhs;
            }));
    }

    constexpr zip_iterator operator-(difference_type rhs) const {
        return std::make_from_tuple<zip_iterator>(
            ttl::transform<iterators_tuple_type>(m_it, [rhs](auto&& it) {
                return it - rhs;
            }));
    }
    
    constexpr difference_type operator+(zip_iterator other) const {
        return ttl::inner_product(m_it, other.m_it,
                                  [](auto&& ...prods) { return std::min({prods...}); },
                                  [](auto&& lhs, auto&& rhs) { return lhs + rhs; });
    }

    constexpr difference_type operator-(zip_iterator other) const {
        return ttl::inner_product(m_it, other.m_it,
                                  [](auto&& ...prods) { return std::min({prods...}); },
                                  [](auto&& lhs, auto&& rhs) { return lhs - rhs; });
    }

    constexpr zip_iterator& operator++() {
        ttl::for_each(m_it, [](auto&& it) { ++it; });
        return *this;
    }

    constexpr zip_iterator& operator--() {
        ttl::for_each(m_it, [](auto&& it) { --it; });
        return *this;
    }

    constexpr zip_iterator& operator+=(difference_type rhs) {
        ttl::for_each(m_it, [rhs](auto&& it) { it += rhs; });
        return *this;
    }

    constexpr zip_iterator& operator-=(difference_type rhs) {
        ttl::for_each(m_it, [rhs](auto&& it) { it -= rhs; });
        return *this;
    }

    constexpr bool operator<(const zip_iterator& rhs) const {
        return ttl::any(m_it, rhs.m_it, std::less{});
    }

    constexpr bool operator<=(const zip_iterator& rhs) const {
        return ttl::any(m_it, rhs.m_it, std::less_equal{});
    }

    constexpr bool operator>(const zip_iterator& rhs) const {
        return ttl::any(m_it, rhs.m_it, std::greater{});
    }

    constexpr bool operator>=(const zip_iterator& rhs) const {
        return ttl::any(m_it, rhs.m_it, std::greater_equal{});
    }

    constexpr bool operator==(const zip_iterator& rhs) const {
        // Equivalent to:
        // !(a != a' && b != b' && ...)
        return ttl::any(m_it, rhs.m_it, std::equal_to{});
    }

    constexpr bool operator!=(const zip_iterator& rhs) const {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return ttl::all(m_it, rhs.m_it, std::not_equal_to{});
    }

    constexpr value_type operator*() const {
        // WARNING: decltype(auto) is vital here, otherwise ttl::transform
        // is going to construct and return a tuple of values (the ones
        // returned by this lambda) preventing the caller from modifying
        // actual values underlying the zipped iterators
        return ttl::transform<value_type>(m_it, [](auto it) -> decltype(auto) {
            return *it;
        });
    }

    constexpr value_type operator[](difference_type rhs) const {
        // WARNING: decltype(auto) is vital here, otherwise ttl::transform
        // is going to construct and return a tuple of values (the ones
        // returned by this lambda) preventing the caller from modifying
        // actual values underlying the zipped iterators
        return ttl::transform<value_type>(m_it, [rhs](auto&& it) -> decltype(auto) {
            return it[rhs];
        });
    }

    // Tuple-like semantics support
    using tuple_size = std::tuple_size<iterators_tuple_type>;

    template<std::size_t I>
    struct tuple_element {
        using type = std::tuple_element_t<I, iterators_tuple_type>;
    };

    template <std::size_t I>
    constexpr auto& get() & noexcept {
        return std::get<I>(m_it);
    }

    template <std::size_t I>
    constexpr auto const& get() const& noexcept {
        return std::get<I>(m_it);
    }

    template <std::size_t I>
    constexpr auto&& get() && noexcept {
        return std::get<I>(std::move(m_it));
    }

    template <std::size_t I>
    constexpr auto const&& get() const&& noexcept {
        return std::get<I>(std::move(m_it));
    }

   private:
    iterators_tuple_type m_it;
};

// clang-format on
}  // namespace zip

namespace std {
// Enable tuple-like semantics on zip::zip_iterator
// Injecting stuff into std is unfortunately required

template <typename... Ts>
struct tuple_size<::zip::zip_iterator<Ts...>> : ::zip::zip_iterator<Ts...>::tuple_size { };

template <std::size_t I, typename... Ts>
struct tuple_element<I, ::zip::zip_iterator<Ts...>> {
    using type = typename ::zip::zip_iterator<Ts...>::template tuple_element<I>::type;
};

template <std::size_t I, typename... Ts>
constexpr auto& get(::zip::zip_iterator<Ts...>& a) noexcept {
    return a.template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr auto const& get(::zip::zip_iterator<Ts...> const& a) noexcept {
    return a.template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr auto&& get(::zip::zip_iterator<Ts...>&& a) noexcept {
    return std::move(a).template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr auto const&& get(::zip::zip_iterator<Ts...> const&& a) noexcept {
    return std::move(a).template get<I>();
}
}

#endif
