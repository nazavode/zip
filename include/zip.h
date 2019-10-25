#ifndef ZIP_H_INCLUDED_20191008
#define ZIP_H_INCLUDED_20191008

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

// TODO
#define ADD_BASE_ACCESSOR(BASE_TYPE)                                                \
    constexpr auto& base() & noexcept { return static_cast<BASE_TYPE&>(*this); }    \
    constexpr auto const& base() const& noexcept {                                  \
        return static_cast<BASE_TYPE const&>(*this);                                \
    }                                                                               \
    constexpr auto&& base() && noexcept { return static_cast<BASE_TYPE&&>(*this); } \
    constexpr auto const&& base() const&& noexcept {                                \
        return static_cast<BASE_TYPE const&&>(*this);                               \
    }

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

namespace detail {

template <typename IteratorPack, template <typename, typename> typename... Policies>
class iterator : public IteratorPack,
                 public Policies<iterator<IteratorPack, Policies...>, IteratorPack>... {
   public:
    using IteratorPack::IteratorPack;
};

/////////////////////////////////////////////////////////////////////////////////////
// Pack
/////////////////////////////////////////////////////////////////////////////////////

template <typename... Iterators>
class iterator_pack {
   public:
    using value_type =
        std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using iterator_pack_type = std::tuple<std::remove_reference_t<Iterators>...>;
    using iterator_category = std::common_type_t<
        typename std::iterator_traits<Iterators>::iterator_category...>;
    using difference_type =
        std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;

    explicit constexpr iterator_pack(Iterators... iterators) noexcept
        : m_iterators{std::move(iterators)...} {}

    constexpr auto& iterators() & noexcept { return m_iterators; }

    constexpr auto const& iterators() const& noexcept { return m_iterators; }

    constexpr auto&& iterators() && noexcept { return std::move(m_iterators); }

    constexpr auto const&& iterators() const&& noexcept { return std::move(m_iterators); }

   private:
    iterator_pack_type m_iterators;
};

/////////////////////////////////////////////////////////////////////////////////////
// Policies
/////////////////////////////////////////////////////////////////////////////////////

template <typename IteratorBase, typename IteratorPack>
class dereference {
    using base_type = IteratorBase;
    using value_type = typename IteratorPack::value_type;

    ADD_BASE_ACCESSOR(base_type)

   public:
    constexpr value_type operator*() const {
        return ttl::transform<value_type>(
            base().iterators(), [](auto&& it) -> decltype(auto) { return *it; });
    }
};

template <typename IteratorBase, typename IteratorPack>
class subscript {
    using base_type = IteratorBase;
    using value_type = typename IteratorPack::value_type;
    using difference_type = typename IteratorPack::difference_type;

    ADD_BASE_ACCESSOR(base_type)

   public:
    constexpr value_type operator[](difference_type rhs) const {
        return ttl::transform<value_type>(
            base().iterators(), [rhs](auto&& it) -> decltype(auto) { return it[rhs]; });
    }
};

template <typename IteratorBase, typename IteratorPack>
class comparison {
    using base_type = IteratorBase;
    ADD_BASE_ACCESSOR(base_type)

   public:
    constexpr bool operator<(const base_type& rhs) const {
        return ttl::any(base().iterators(), rhs.iterators(), std::less{});
    }

    constexpr bool operator<=(const base_type& rhs) const {
        return ttl::any(base().iterators(), rhs.iterators(), std::less_equal{});
    }

    constexpr bool operator>(const base_type& rhs) const {
        return ttl::any(base().iterators(), rhs.iterators(), std::greater{});
    }

    constexpr bool operator>=(const base_type& rhs) const {
        return ttl::any(base().iterators(), rhs.iterators(), std::greater_equal{});
    }

    constexpr bool operator==(const base_type& rhs) const {
        // Equivalent to:
        // !(a != a' && b != b' && ...)
        return ttl::any(base().iterators(), rhs.iterators(), std::equal_to{});
    }

    constexpr bool operator!=(const base_type& rhs) const {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return ttl::all(base().iterators(), rhs.iterators(), std::not_equal_to{});
    }
};

template <typename IteratorBase, typename IteratorPack>
class forward {
    using base_type = IteratorBase;
    using difference_type = typename IteratorPack::difference_type;
    using iterator_pack_type = typename IteratorPack::iterator_pack_type;

    ADD_BASE_ACCESSOR(base_type)

   public:
    constexpr base_type operator+(difference_type rhs) const {
        return std::make_from_tuple<base_type>(ttl::transform<iterator_pack_type>(
            base().iterators(), [rhs](auto&& it) { return it + rhs; }));
    }

    constexpr base_type& operator++() {
        ttl::for_each(base().iterators(), [](auto&& it) { ++it; });
        return base();
    }

    constexpr base_type operator++(int) {
        base_type prev{base()};
        ttl::for_each(base().iterators(), [](auto&& it) { ++it; });
        return prev;
    }

    constexpr base_type& operator+=(difference_type rhs) {
        ttl::for_each(base().iterators(), [rhs](auto&& it) { it += rhs; });
        return base();
    }
};

template <typename IteratorBase, typename IteratorPack>
class backward {
    using base_type = IteratorBase;
    using difference_type = typename IteratorPack::difference_type;
    using iterator_pack_type = typename IteratorPack::iterator_pack_type;

    ADD_BASE_ACCESSOR(base_type)

   public:
    constexpr base_type operator-(difference_type rhs) const {
        return std::make_from_tuple<base_type>(ttl::transform<iterator_pack_type>(
            base().iterators(), [rhs](auto&& it) { return it - rhs; }));
    }

    constexpr difference_type operator-(const base_type& other) const {
        return ttl::inner_product(
            base().iterators(), other.iterators(),
            [](auto&&... prods) { return std::min({prods...}); },  // TODO
            [](auto&& lhs, auto&& rhs) { return lhs - rhs; });
        // return ttl::inner_product(base().iterators(), other.iterators(), std::min,
        // std::minus);
    }

    constexpr base_type& operator--() {
        ttl::for_each(base().iterators(), [](auto&& it) { --it; });
        return base();
    }

    constexpr base_type operator--(int) {
        base_type prev{base()};
        ttl::for_each(base().iterators(), [](auto&& it) { --it; });
        return prev;
    }

    constexpr base_type& operator-=(difference_type rhs) {
        ttl::for_each(base().iterators(), [rhs](auto&& it) { it -= rhs; });
        return base();
    }
};

template <typename IteratorBase, typename IteratorPack>
struct offset {
    using base_type = IteratorBase;
    using difference_type = typename IteratorPack::difference_type;
    using iterator_pack_type = typename IteratorPack::iterator_pack_type;

    ADD_BASE_ACCESSOR(base_type)

    difference_type m_offset{};

   public:
    constexpr bool operator<(const base_type& rhs) const noexcept {
        using std::get;
        return (get<0>(base().iterators()) + m_offset) <
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator<=(const base_type& rhs) const noexcept {
        using std::get;
        return (get<0>(base().iterators()) + m_offset) <=
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator>(const base_type& rhs) const noexcept {
        using std::get;
        return (get<0>(base().iterators()) + m_offset) >
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator>=(const base_type& rhs) const noexcept {
        using std::get;
        return (get<0>(base().iterators()) + m_offset) >=
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator==(const base_type& rhs) const noexcept {
        using std::get;
        return (get<0>(base().iterators()) + m_offset) ==
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator!=(const base_type& rhs) const noexcept {
        using std::get;
        return (get<0>(base().iterators()) + m_offset) !=
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    // Forward interface

    constexpr base_type operator+(difference_type rhs) const {
        base_type ret{base()};
        ret.m_offset += rhs;
        return ret;
    }

    constexpr base_type& operator++() {
        ++m_offset;
        return base();
    }

    constexpr base_type operator++(int) {
        base_type prev{base()};
        ++m_offset;
        return prev;
    }

    constexpr base_type& operator+=(difference_type rhs) {
        m_offset += rhs;
        return base();
    }

    // Backward interface

    constexpr base_type operator-(difference_type rhs) const {
        base_type ret{base()};
        ret.m_offset -= rhs;
        return ret;
    }

    constexpr difference_type operator-(const base_type& rhs) const {
        using std::get;
        return (get<0>(base().iterators()) + m_offset) -
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr base_type& operator--() noexcept {
        --m_offset;
        return base();
    }

    constexpr base_type operator--(int) noexcept {
        base_type ret{base()};
        --m_offset;
        return ret;
    }

    constexpr base_type& operator-=(difference_type rhs) noexcept {
        m_offset -= rhs;
        return base();
    }
};

}  // namespace detail

struct offset_iterator_tag {};

template <typename... Iterators>
using forward_iterator =
    detail::iterator<detail::iterator_pack<Iterators...>, detail::dereference,
                     detail::comparison, detail::forward>;

template <typename... Iterators>
using bidirectional_iterator =
    detail::iterator<detail::iterator_pack<Iterators...>, detail::dereference,
                     detail::comparison, detail::forward, detail::backward>;

template <typename... Iterators>
using random_access_iterator =
    detail::iterator<detail::iterator_pack<Iterators...>, detail::dereference,
                     detail::comparison, detail::forward, detail::backward,
                     detail::subscript>;

template <typename... Iterators>
using offset_iterator =
    detail::iterator<detail::iterator_pack<Iterators...>, detail::offset>;

template <typename IteratorCategory, typename FirstIterator, typename... Iterators>
struct iterator_type;

template <typename FirstIterator, typename... Iterators>
struct iterator_type<std::forward_iterator_tag, FirstIterator, Iterators...> {
    using type = forward_iterator<FirstIterator, Iterators...>;
};

template <typename FirstIterator, typename... Iterators>
struct iterator_type<std::bidirectional_iterator_tag, FirstIterator, Iterators...> {
    using type = bidirectional_iterator<FirstIterator, Iterators...>;
};

template <typename FirstIterator, typename... Iterators>
struct iterator_type<std::random_access_iterator_tag, FirstIterator, Iterators...> {
    using type = random_access_iterator<FirstIterator, Iterators...>;
};

template <typename FirstIterator, typename... Iterators>
struct iterator_type<offset_iterator_tag, FirstIterator, Iterators...> {
    using type = offset_iterator<FirstIterator, Iterators...>;
};

template <typename... Iterators>
using iterator_type_t =
    typename iterator_type<typename detail::iterator_pack<Iterators...>::iterator_category,
                           Iterators...>::type;

template <typename... Iterators>
constexpr auto make_iterator(Iterators&&... args) {
    return iterator_type_t<Iterators...>{std::forward<Iterators>(args)...};
}

// template <typename... Sequences>
// struct zip_view {
//     static constexpr auto arity = sizeof...(Sequences);
//     using iterator =
//         // Why the & is needed here:
//         //
//         https://stackoverflow.com/questions/42580761/why-does-stdbegin-always-return-const-iterator-in-such-a-case
//         zip_iterator<decltype(std::begin(std::declval<Sequences&>()))...>;
//     // This type wraps a reference to each sequence on which has been
//     // instantiated:
//     using sequences = std::tuple<Sequences&...>;
//
//     // using size_type =
//     std::common_type_t<decltype(std::size(std::declval<Sequences&>()))...>; using
//     size_type = std::size_t;
//
//     constexpr zip_view(Sequences&... sqs) noexcept : m_seq{sqs...} {}
//
//     constexpr iterator begin() { return begin_impl(std::make_index_sequence<arity>{});
//     }
//
//     constexpr iterator end() { return end_impl(std::make_index_sequence<arity>{}); }
//
//     constexpr size_type size() const noexcept {
//         return size_impl(std::make_index_sequence<arity>{});
//     }
//
//    private:
//     template <std::size_t... Indexes>
//     constexpr iterator begin_impl(std::index_sequence<Indexes...>) {
//         return {std::begin(std::get<Indexes>(m_seq))...};
//     }
//
//     template <std::size_t... Indexes>
//     constexpr iterator end_impl(std::index_sequence<Indexes...>) {
//         return {std::end(std::get<Indexes>(m_seq))...};
//     }
//
//     template <std::size_t... Indexes>
//     constexpr size_type size_impl(std::index_sequence<Indexes...>) const noexcept {
//         return std::min(std::size(std::get<Indexes>(m_seq))...);
//     }
//
//     sequences m_seq;
// };

}  // namespace zip

#endif
