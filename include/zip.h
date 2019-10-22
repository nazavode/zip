#ifndef ZIP_H_INCLUDED_20191008
#define ZIP_H_INCLUDED_20191008

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace zip {

namespace ttl {

// clang-format off

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

// clang-format on
}  // namespace ttl

// Additional iterator categories

// struct unsafe_forward_iterator_tag : public std::forward_iterator_tag {};
// struct unsafe_bidirectional_iterator_tag : public std::bidirectional_iterator_tag {};
// struct unsafe_random_access_iterator_tag : public std::random_access_iterator_tag {};

struct unsafe_forward_iterator_tag {};
struct unsafe_bidirectional_iterator_tag : public unsafe_forward_iterator_tag {};
struct unsafe_random_access_iterator_tag : public unsafe_bidirectional_iterator_tag {};

// Type trait:
// unsafe_iterator_tag

template <typename T>
struct unsafe_iterator_tag;

template <>
struct unsafe_iterator_tag<std::forward_iterator_tag> {
    using type = unsafe_forward_iterator_tag;
};

template <>
struct unsafe_iterator_tag<std::bidirectional_iterator_tag> {
    using type = unsafe_bidirectional_iterator_tag;
};

template <>
struct unsafe_iterator_tag<std::random_access_iterator_tag> {
    using type = unsafe_random_access_iterator_tag;
};

template <typename T>
using unsafe_iterator_tag_t = typename unsafe_iterator_tag<T>::type;

// Type trait:
// safe_iterator_tag

template <typename T>
struct safe_iterator_tag;

template <>
struct safe_iterator_tag<unsafe_forward_iterator_tag> {
    using type = std::forward_iterator_tag;
};

template <>
struct safe_iterator_tag<unsafe_bidirectional_iterator_tag> {
    using type = std::bidirectional_iterator_tag;
};

template <>
struct safe_iterator_tag<unsafe_random_access_iterator_tag> {
    using type = std::random_access_iterator_tag;
};

template <typename T>
using safe_iterator_tag_t = typename safe_iterator_tag<T>::type;

// Iterator traits

template <typename... Iterators>
using common_iterator_category_t =
    std::common_type_t<typename std::iterator_traits<Iterators>::iterator_category...>;

template <typename... Iterators>
using common_difference_type_t =
    std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;

namespace detail {

// TODO safe/unsafe

// struct safe_iteration_t {};
// struct unsafe_iteration_t {};

// inline constexpr auto safe_iteration = safe_iteration_t{};
// inline constexpr auto unsafe_iteration = safe_iteration_t{};

// template <typename IterationPolicy>
// struct predication_policy;

// template <>
// struct predication_policy<safe_iteration_t> {
//     using iteration_policy = safe_iteration_t;

//     template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
//     static constexpr bool any(TupleLHS&& lhs, TupleRHS&& rhs,
//                               BinaryPredicate&& op) noexcept;

//     template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
//     static constexpr bool all(TupleLHS&& lhs, TupleRHS&& rhs,
//                               BinaryPredicate&& op) noexcept;
// };

// template <>
// struct predication_policy<unsafe_iteration_t> {
//     using iteration_policy = unsafe_iteration_t;

//     template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
//     static constexpr bool any(TupleLHS&& lhs, TupleRHS&& rhs,
//                               BinaryPredicate&& op) noexcept;

//     template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
//     static constexpr bool all(TupleLHS&& lhs, TupleRHS&& rhs,
//                               BinaryPredicate&& op) noexcept;
// };

template <typename... Iterators>
class iterator_base {
   public:
    using value_type =
        std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using difference_type = common_difference_type_t<Iterators...>;
    using reference = value_type;
    using pointer = value_type;

    explicit constexpr iterator_base(Iterators... iterators) noexcept
        : m_iterators{std::move(iterators)...} {}

   protected:
    using iterator_tuple_type = std::tuple<std::remove_reference_t<Iterators>...>;

    constexpr auto& iterators() & noexcept { return m_iterators; }

    constexpr auto const& iterators() const& noexcept { return m_iterators; }

    constexpr auto&& iterators() && noexcept { return std::move(m_iterators); }

    constexpr auto const&& iterators() const&& noexcept { return std::move(m_iterators); }

   private:
    iterator_tuple_type m_iterators;
};

template <typename IteratorCategory, typename... Iterators>
class iterator_impl;

template <typename... Iterators>
class iterator_impl<std::forward_iterator_tag, Iterators...>
    : public iterator_base<Iterators...> {
   protected:
    using base = iterator_base<Iterators...>;
    using iterator_tuple_type = typename base::iterator_tuple_type;
    using base::iterators;

   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;

    // Construction

    using base::base;

    // Forward interface

    constexpr iterator_impl operator+(difference_type rhs) const {
        return std::make_from_tuple<iterator_impl>(ttl::transform<iterator_tuple_type>(
            iterators(), [rhs](auto&& it) { return it + rhs; }));
    }

    constexpr iterator_impl& operator++() {
        ttl::for_each(iterators(), [](auto&& it) { ++it; });
        return *this;
    }

    constexpr iterator_impl& operator++(int) {
        iterator_impl prev{*this};
        ttl::for_each(iterators(), [](auto&& it) { ++it; });
        return prev;
    }

    constexpr iterator_impl& operator+=(difference_type rhs) {
        ttl::for_each(iterators(), [rhs](auto&& it) { it += rhs; });
        return *this;
    }

    // Relational interface

    constexpr bool operator<(const iterator_impl& rhs) const {  // Safe
        return ttl::any(iterators(), rhs.iterators(), std::less{});
    }

    constexpr bool operator<=(const iterator_impl& rhs) const {  // Safe
        return ttl::any(iterators(), rhs.iterators(), std::less_equal{});
    }

    constexpr bool operator>(const iterator_impl& rhs) const {  // Safe
        return ttl::any(iterators(), rhs.iterators(), std::greater{});
    }

    constexpr bool operator>=(const iterator_impl& rhs) const {  // Safe
        return ttl::any(iterators(), rhs.iterators(), std::greater_equal{});
    }

    constexpr bool operator==(const iterator_impl& rhs) const {  // Safe
        // Equivalent to:
        // !(a != a' && b != b' && ...)
        return ttl::any(iterators(), rhs.iterators(), std::equal_to{});
    }

    constexpr bool operator!=(const iterator_impl& rhs) const {  // Safe
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return ttl::all(iterators(), rhs.iterators(), std::not_equal_to{});
    }

    // Dereference

    constexpr value_type operator*() const {
        // WARNING: decltype(auto) is vital here, otherwise ttl::transform
        // is going to construct and return a tuple of values (the ones
        // returned by this lambda) preventing the caller from modifying
        // actual values underlying the zipped iterators
        return ttl::transform<value_type>(
            iterators(), [](auto&& it) -> decltype(auto) { return *it; });
    }
};

template <typename... Iterators>
class iterator_impl<std::bidirectional_iterator_tag, Iterators...>
    : public iterator_impl<std::forward_iterator_tag, Iterators...> {
   protected:
    using base = iterator_impl<std::forward_iterator_tag, Iterators...>;
    using iterator_tuple_type = typename base::iterator_tuple_type;
    using base::iterators;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;

    // Construction

    using base::base;

    // Backward interface

    constexpr iterator_impl operator-(difference_type rhs) const {
        return std::make_from_tuple<iterator_impl>(ttl::transform<iterator_tuple_type>(
            iterators(), [rhs](auto&& it) { return it - rhs; }));
    }

    constexpr difference_type operator-(const iterator_impl& other) const {  // Safe
        return ttl::inner_product(iterators(), other.iterators(),
                                  [](auto&&... prods) { return std::min({prods...}); },
                                  [](auto&& lhs, auto&& rhs) { return lhs - rhs; });
    }

    constexpr iterator_impl& operator--() {
        ttl::for_each(iterators(), [](auto&& it) { --it; });
        return *this;
    }

    constexpr iterator_impl& operator--(int) {
        iterator_impl prev{*this};
        ttl::for_each(iterators(), [](auto&& it) { --it; });
        return prev;
    }

    constexpr iterator_impl& operator-=(difference_type rhs) {
        ttl::for_each(iterators(), [rhs](auto&& it) { it -= rhs; });
        return *this;
    }
};

template <typename... Iterators>
class iterator_impl<std::random_access_iterator_tag, Iterators...>
    : public iterator_base<Iterators...> {
   protected:
    using base = iterator_base<Iterators...>;
    using iterator_tuple_type = typename base::iterator_tuple_type;
    using base::iterators;

    constexpr auto& offset() & noexcept { return m_offset; }

    constexpr auto const& offset() const& noexcept { return m_offset; }

    constexpr auto&& offset() && noexcept { return std::move(m_offset); }

    constexpr auto const&& offset() const&& noexcept { return std::move(m_offset); }

   public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;

    // Construction

    using base::base;

    // Relational interface

    constexpr bool operator<(const iterator_impl& rhs) const {
        return ttl::any(iterators(), rhs.iterators(),
                        [lhs_offset = m_offset, rhs_offset = rhs.m_offset](
                            auto&& lhs_it, auto&& rhs_it) {
                            return (lhs_it + lhs_offset) < (rhs_it + rhs_offset);
                        });
    }

    constexpr bool operator<=(const iterator_impl& rhs) const {
        return ttl::any(iterators(), rhs.iterators(),
                        [lhs_offset = m_offset, rhs_offset = rhs.m_offset](
                            auto&& lhs_it, auto&& rhs_it) {
                            return (lhs_it + lhs_offset) <= (rhs_it + rhs_offset);
                        });
    }

    constexpr bool operator>(const iterator_impl& rhs) const {
        return ttl::any(iterators(), rhs.iterators(),
                        [lhs_offset = m_offset, rhs_offset = rhs.m_offset](
                            auto&& lhs_it, auto&& rhs_it) {
                            return (lhs_it + lhs_offset) > (rhs_it + rhs_offset);
                        });
    }

    constexpr bool operator>=(const iterator_impl& rhs) const {
        return ttl::any(iterators(), rhs.iterators(),
                        [lhs_offset = m_offset, rhs_offset = rhs.m_offset](
                            auto&& lhs_it, auto&& rhs_it) {
                            return (lhs_it + lhs_offset) >= (rhs_it + rhs_offset);
                        });
    }

    constexpr bool operator==(const iterator_impl& rhs) const {
        // Equivalent to:
        // !(a != a' && b != b' && ...) ==
        //   a == a' || b == b' || ...
        return ttl::any(iterators(), rhs.iterators(),
                        [lhs_offset = m_offset, rhs_offset = rhs.m_offset](
                            auto&& lhs_it, auto&& rhs_it) {
                            return (lhs_it + lhs_offset) == (rhs_it + rhs_offset);
                        });
    }

    constexpr bool operator!=(const iterator_impl& rhs) const {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return ttl::all(iterators(), rhs.iterators(),
                        [lhs_offset = m_offset, rhs_offset = rhs.m_offset](
                            auto&& lhs_it, auto&& rhs_it) {
                            return (lhs_it + lhs_offset) != (rhs_it + rhs_offset);
                        });
    }

    // Forward interface

    constexpr iterator_impl operator+(difference_type rhs) const {
        iterator_impl ret{*this};
        ret.m_offset += rhs;
        return ret;
    }

    constexpr iterator_impl& operator++() {
        ++m_offset;
        return *this;
    }

    constexpr iterator_impl operator++(int) {
        iterator_impl prev{*this};
        ++m_offset;
        return prev;
    }

    constexpr iterator_impl& operator+=(difference_type rhs) {
        m_offset += rhs;
        return *this;
    }

    // Backward interface

    constexpr iterator_impl operator-(difference_type rhs) const {
        iterator_impl ret{*this};
        ret.m_offset -= rhs;
        return ret;
    }

    constexpr difference_type operator-(const iterator_impl& other) const {
        return ttl::inner_product(
            iterators(), other.iterators(),
            [](auto&&... prods) { return std::min({prods...}); },
            [lhs_offset = m_offset, rhs_offset = other.m_offset](auto&& lhs, auto&& rhs) {
                return (lhs + lhs_offset) - (rhs + rhs_offset);
            });
    }

    constexpr iterator_impl& operator--() noexcept {
        --m_offset;
        return *this;
    }

    constexpr iterator_impl operator--(int) noexcept {
        iterator_impl ret{*this};
        --m_offset;
        return ret;
    }

    constexpr iterator_impl& operator-=(difference_type rhs) noexcept {
        m_offset -= rhs;
        return *this;
    }

    // Dereference

    constexpr value_type operator*() const noexcept { return operator[](0); }

    constexpr value_type operator[](difference_type rhs) const noexcept {
        rhs += m_offset;
        // WARNING: decltype(auto) is vital here, otherwise ttl::transform
        // is going to construct and return a tuple of values (the ones
        // returned by this lambda) preventing the caller from modifying
        // actual values underlying the zipped iterators
        return ttl::transform<value_type>(
            iterators(), [rhs](auto&& it) -> decltype(auto) { return it[rhs]; });
    }

   private:
    difference_type m_offset{0};
};

template <typename... Iterators>
class iterator_impl<unsafe_random_access_iterator_tag, Iterators...>
    : public iterator_impl<std::random_access_iterator_tag, Iterators...> {
   protected:
    using base = iterator_impl<std::random_access_iterator_tag, Iterators...>;
    using base::iterators;
    using base::offset;

   public:
    // using iterator_category = unsafe_random_access_iterator_tag; // TODO don't override
    // parent's category?
    using difference_type = typename base::difference_type;

    // Construction

    using base::base;

    // Relational interface

    constexpr bool operator<(const iterator_impl& rhs) const noexcept {
        return (std::get<0>(iterators()) + offset()) <
               (std::get<0>(rhs.iterators()) + rhs.offset());
    }

    constexpr bool operator<=(const iterator_impl& rhs) const noexcept {
        return (std::get<0>(iterators()) + offset()) <=
               (std::get<0>(rhs.iterators()) + rhs.offset());
    }

    constexpr bool operator>(const iterator_impl& rhs) const noexcept {
        return (std::get<0>(iterators()) + offset()) >
               (std::get<0>(rhs.iterators()) + rhs.offset());
    }

    constexpr bool operator>=(const iterator_impl& rhs) const noexcept {
        return (std::get<0>(iterators()) + offset()) >=
               (std::get<0>(rhs.iterators()) + rhs.offset());
    }

    constexpr bool operator==(const iterator_impl& rhs) const noexcept {
        return (std::get<0>(iterators()) + offset()) ==
               (std::get<0>(rhs.iterators()) + rhs.offset());
    }

    constexpr bool operator!=(const iterator_impl& rhs) const noexcept {
        return (std::get<0>(iterators()) + offset()) !=
               (std::get<0>(rhs.iterators()) + rhs.offset());
    }

    constexpr difference_type operator-(const iterator_impl& rhs) const noexcept {
        return (std::get<0>(iterators()) + offset()) -
               (std::get<0>(rhs.iterators()) + rhs.offset());
    }
};

}  // namespace detail

template <typename... Iterators>
using iterator_type_t =
    detail::iterator_impl<common_iterator_category_t<Iterators...>, Iterators...>;

// No tmp argument deduction for type aliases, just add the usual factory:

template <typename... Iterators>
constexpr auto make_iterator(Iterators&&... args) noexcept {
    return iterator_type_t<Iterators...>{std::forward<Iterators>(args)...};
}

// TODO safe/unsafe
// template <typename... Iterators>
// constexpr auto make_iterator(safe_iteration_t, Iterators&&... args) noexcept {
//     return iterator_type_t<Iterators...>{std::forward<Iterators>(args)...};
// }

// template <typename... Iterators>
// constexpr auto make_iterator(unsafe_iteration_t, Iterators&&... args) noexcept {
//     return iterator_type_t<Iterators...>{std::forward<Iterators>(args)...};
// }

}  // namespace zip

#endif
