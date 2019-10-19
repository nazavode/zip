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
struct zip_safe_random_access_iterator {

    static_assert((
        std::is_same_v<
            typename std::iterator_traits<Iterators>::iterator_category,
            std::random_access_iterator_tag> && ...),
        "zip_safe_random_access_iterator supports only random access iterators");

    using iterators_tuple_type = std::tuple<std::remove_reference_t<Iterators>...>;

    using value_type = std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using difference_type = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
    using reference = value_type;
    using pointer = value_type;
    using iterator_category = std::random_access_iterator_tag;
    
    explicit constexpr zip_iterator(Iterators... iterators) noexcept
        : m_it{iterators...} {}

    constexpr zip_iterator operator+(difference_type rhs) const noexcept {
        zip_iterator ret{*this};
        ret.m_offset += rhs;
        return ret;
    }

    constexpr zip_iterator operator-(difference_type rhs) const noexcept {
        zip_iterator ret{*this};
        ret.m_offset -= rhs;
        return ret;
    }

    constexpr difference_type operator-(const zip_iterator& other) const noexcept {
        return ttl::inner_product(m_it, other.m_it,
                                  [](auto&& ...prods) { return std::min({prods...}); },
                                  [lhs_offset=m_offset, rhs_offset=other.m_offset](auto&& lhs, auto&& rhs) {
                                      return (lhs + lhs_offset) - (rhs + rhs_offset); });
    }

    constexpr zip_iterator& operator++() noexcept {
        ++m_offset;
        return *this;
    }

    constexpr zip_iterator operator++(int) noexcept {
        zip_iterator ret{*this};
        ++m_offset;
        return ret;
    }

    constexpr zip_iterator& operator--() noexcept {
        --m_offset;
        return *this;
    }

    constexpr zip_iterator operator--(int) noexcept {
        zip_iterator ret{*this};
        --m_offset;
        return ret;
    }

    constexpr zip_iterator& operator+=(difference_type rhs) noexcept {
        m_offset += rhs;
        return *this;
    }

    constexpr zip_iterator& operator-=(difference_type rhs) noexcept {
        m_offset -= rhs;
        return *this;
    }

    constexpr bool operator<(const zip_iterator& rhs) const noexcept {
        return ttl::any(m_it, rhs.m_it,
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) < (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator<=(const zip_iterator& rhs) const noexcept {
        return ttl::any(m_it, rhs.m_it,
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) <= (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator>(const zip_iterator& rhs) const noexcept {
        return ttl::any(m_it, rhs.m_it,
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) > (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator>=(const zip_iterator& rhs) const noexcept {
        return ttl::any(m_it, rhs.m_it,
            [lhs_offset=m_offset, rhs_offset=rhs.m_offset]
            (auto&& lhs_it, auto&& rhs_it) {
                return (lhs_it + lhs_offset) >= (rhs_it + rhs_offset);
            });
    }

    constexpr bool operator==(const zip_iterator& rhs) const noexcept {
        // Equivalent to:
        // !(a != a' && b != b' && ...) ==
        //   a == a' || b == b' || ...
        return ttl::any(m_it, rhs.m_it,
                        [lhs_offset=m_offset, rhs_offset=rhs.m_offset](auto&& lhs_it, auto&& rhs_it){
                            return (lhs_it + lhs_offset) == (rhs_it + rhs_offset);
                        });
    }

    constexpr bool operator!=(const zip_iterator& rhs) const noexcept {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return ttl::all(m_it, rhs.m_it,
                        [lhs_offset=m_offset, rhs_offset=rhs.m_offset](auto&& lhs_it, auto&& rhs_it){
                            return (lhs_it + lhs_offset) != (rhs_it + rhs_offset);
                        });
    }

    constexpr value_type operator*() const noexcept {
        return operator[](difference_type{0});
    }

    constexpr value_type operator[](difference_type rhs) const noexcept {
        rhs += m_offset;
        // WARNING: decltype(auto) is vital here, otherwise ttl::transform
        // is going to construct and return a tuple of values (the ones
        // returned by this lambda) preventing the caller from modifying
        // actual values underlying the zipped iterators
        return ttl::transform<value_type>(m_it, [rhs](auto&& it) -> decltype(auto) {
            return it[rhs];
        });
    }

   private:

    difference_type m_offset{0};
    iterators_tuple_type m_it;
};

template<typename ...Iterators>
struct zip_unsafe_random_access_iterator {

    static_assert((
        std::is_same_v<
            typename std::iterator_traits<Iterators>::iterator_category,
            std::random_access_iterator_tag> && ...),
        "zip_unsafe_random_access_iterator supports only random access iterators");

    using iterators_tuple_type = std::tuple<std::remove_reference_t<Iterators>...>;

    using value_type = std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using difference_type = std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
    using reference = value_type;
    using pointer = value_type;
    using iterator_category = std::random_access_iterator_tag;
    
    explicit constexpr zip_iterator(Iterators... iterators) noexcept
        : m_it{iterators...} {}

    constexpr zip_iterator operator+(difference_type rhs) const noexcept {
        zip_iterator ret{*this};
        ret.m_offset += rhs;
        return ret;
    }

    constexpr zip_iterator operator-(difference_type rhs) const noexcept {
        zip_iterator ret{*this};
        ret.m_offset -= rhs;
        return ret;
    }

    constexpr difference_type operator-(const zip_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset)
             - (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr zip_iterator& operator++() noexcept {
        ++m_offset;
        return *this;
    }

    constexpr zip_iterator operator++(int) noexcept {
        zip_iterator ret{*this};
        ++m_offset;
        return ret;
    }

    constexpr zip_iterator& operator--() noexcept {
        --m_offset;
        return *this;
    }

    constexpr zip_iterator operator--(int) noexcept {
        zip_iterator ret{*this};
        --m_offset;
        return ret;
    }

    constexpr zip_iterator& operator+=(difference_type rhs) noexcept {
        m_offset += rhs;
        return *this;
    }

    constexpr zip_iterator& operator-=(difference_type rhs) noexcept {
        m_offset -= rhs;
        return *this;
    }

    constexpr bool operator<(const zip_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset)
             < (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator<=(const zip_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset)
            <= (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator>(const zip_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset)
             > (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator>=(const zip_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset)
            >= (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator==(const zip_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset)
            == (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator!=(const zip_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset)
            != (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr value_type operator*() const noexcept {
        return operator[](difference_type{0});
    }

    constexpr value_type operator[](difference_type rhs) const noexcept {
        rhs += m_offset;
        // WARNING: decltype(auto) is vital here, otherwise ttl::transform
        // is going to construct and return a tuple of values (the ones
        // returned by this lambda) preventing the caller from modifying
        // actual values underlying the zipped iterators
        return ttl::transform<value_type>(m_it, [rhs](auto&& it) -> decltype(auto) {
            return it[rhs];
        });
    }

   private:

    difference_type m_offset{0};
    iterators_tuple_type m_it;
};

// clang-format on
}  // namespace zip

#endif
