#ifndef ZIP_H_INCLUDED_20191008
#define ZIP_H_INCLUDED_20191008

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

// CRTP helpers
// self(): casts this to a perfectly forwarded
// reference to the CRTP complete type.
// Note to self: the cost (in terms of type mess) of
// having *all* the policy classes derive from an actual
// class template providing this stuff is unsustainable
// compared to a neat old school macro.
// clang-format off
#define ZIP_ADD_CRTP_SELF_ACCESSOR(SELF_TYPE)           \
    constexpr auto& self() & noexcept {                 \
        return static_cast<SELF_TYPE&>(*this); }        \
    constexpr auto const& self() const& noexcept {      \
        return static_cast<SELF_TYPE const&>(*this); }  \
    constexpr auto&& self() && noexcept {               \
        return static_cast<SELF_TYPE&&>(*this); }       \
    constexpr auto const&& self() const&& noexcept {    \
        return static_cast<SELF_TYPE const&&>(*this); }
// clang-format on

namespace zip {

namespace ttl {

namespace impl {
// clang-format off

template<typename T>
using indexes = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>;

template <typename Return, typename Tuple, typename UnaryOp, std::size_t... Indexes>
constexpr Return transform(Tuple&& t, UnaryOp&& f, std::index_sequence<Indexes...>) {
    return {
        std::forward<UnaryOp>(f)(std::get<Indexes>(std::forward<Tuple>(t)))...};
}

template <typename Tuple, typename UnaryOp, std::size_t... Indexes>
constexpr void for_each(Tuple&& t, UnaryOp&& f, std::index_sequence<Indexes...>) {
    (std::forward<UnaryOp>(f)(std::get<Indexes>(std::forward<Tuple>(t))), ...);
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate,
          std::size_t... Indexes>
constexpr bool all(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op,
                   std::index_sequence<Indexes...>) {
    return (std::forward<BinaryPredicate>(op)(
                std::get<Indexes>(std::forward<TupleLHS>(lhs)),
                std::get<Indexes>(std::forward<TupleRHS>(rhs))) && ...);
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate,
          std::size_t... Indexes>
constexpr bool any(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op,
                   std::index_sequence<Indexes...>) {
    return (std::forward<BinaryPredicate>(op)(
                std::get<Indexes>(std::forward<TupleLHS>(lhs)),
                std::get<Indexes>(std::forward<TupleRHS>(rhs))) || ...);
}

template <typename Tuple, typename UnaryPredicate, std::size_t... Indexes>
constexpr bool any(Tuple&& t, UnaryPredicate&& op, std::index_sequence<Indexes...>) {
    return (std::forward<UnaryPredicate>(op)(
                std::get<Indexes>(std::forward<Tuple>(t))) || ...);
}

template<typename TupleLHS, typename TupleRHS,
         typename SumNaryOp, typename ProdBinaryOp, std::size_t... Indexes>
constexpr auto inner_product(TupleLHS&& lhs, TupleRHS&& rhs,
                             SumNaryOp&& sum, ProdBinaryOp&& prod, std::index_sequence<Indexes...>) {
    return std::forward<SumNaryOp>(sum)(
               std::forward<ProdBinaryOp>(prod)(
                   std::get<Indexes>(std::forward<TupleLHS>(lhs)),
                   std::get<Indexes>(std::forward<TupleRHS>(rhs)))...);
}

}  // namespace impl

template <typename Tuple, typename UnaryOp>
constexpr void for_each(Tuple&& t, UnaryOp&& f) {
    impl::for_each(std::forward<Tuple>(t), std::forward<UnaryOp>(f),
                   impl::indexes<Tuple>{});
}

template <typename Return, typename Tuple, typename UnaryOp>
constexpr auto transform(Tuple&& t, UnaryOp&& f) {
    return impl::transform<Return>(std::forward<Tuple>(t), std::forward<UnaryOp>(f),
                                   impl::indexes<Tuple>{});
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
constexpr bool all(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    return impl::all(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                     std::forward<BinaryPredicate>(op), impl::indexes<TupleLHS>{});
}

template <typename TupleLHS, typename TupleRHS, typename BinaryPredicate>
constexpr bool any(TupleLHS&& lhs, TupleRHS&& rhs, BinaryPredicate&& op) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    return impl::any(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                     std::forward<BinaryPredicate>(op), impl::indexes<TupleLHS>{});
}

template <typename Tuple, typename UnaryPredicate>
constexpr bool any(Tuple&& t, UnaryPredicate&& op) {
    return impl::any(std::forward<Tuple>(t), std::forward<UnaryPredicate>(op),
                     impl::indexes<Tuple>{});
}

template<typename TupleLHS, typename TupleRHS,
         typename SumNaryOp, typename ProdBinaryOp>
constexpr auto inner_product(TupleLHS&& lhs, TupleRHS&& rhs, SumNaryOp&& sum, ProdBinaryOp&& prod) {
    static_assert(std::tuple_size_v<std::remove_reference_t<TupleLHS>> ==
                  std::tuple_size_v<std::remove_reference_t<TupleRHS>>);
    return impl::inner_product(std::forward<TupleLHS>(lhs), std::forward<TupleRHS>(rhs),
                               std::forward<SumNaryOp>(sum), std::forward<ProdBinaryOp>(prod),
                               impl::indexes<TupleLHS>{});
}

// clang-format on
}  // namespace ttl

namespace policy {

template <typename... Iterators>
class pack {
   public:
    using value_type =
        std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using pointer = value_type;
    using reference = value_type;
    using pack_type = std::tuple<std::remove_reference_t<Iterators>...>;
    using iterator_category = std::common_type_t<
        typename std::iterator_traits<Iterators>::iterator_category...>;
    using difference_type =
        std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;

    constexpr pack(Iterators... iterators) noexcept
        : m_iterators{std::move(iterators)...} {}

    constexpr auto& iterators() & noexcept { return m_iterators; }

    constexpr auto const& iterators() const& noexcept { return m_iterators; }

    constexpr auto&& iterators() && noexcept { return std::move(m_iterators); }

    constexpr auto const&& iterators() const&& noexcept { return std::move(m_iterators); }

   private:
    pack_type m_iterators;
};

template <typename IteratorBase, typename IteratorPack>
class dereference {
    using self_type = IteratorBase;
    ZIP_ADD_CRTP_SELF_ACCESSOR(self_type)

   public:
    constexpr typename IteratorPack::value_type operator*() const {
        return ttl::transform<typename IteratorPack::value_type>(
            self().iterators(), [](auto&& it) -> decltype(auto) { return *it; });
    }
};

template <typename IteratorBase, typename IteratorPack>
class equality_comparable {
    using self_type = IteratorBase;
    ZIP_ADD_CRTP_SELF_ACCESSOR(self_type)

   public:
    constexpr bool operator==(const self_type& rhs) const {
        // Equivalent to:
        // !(a != a' && b != b' && ...)
        return ttl::any(self().iterators(), rhs.iterators(), std::equal_to{});
    }

    constexpr bool operator!=(const self_type& rhs) const {
        // a != a' && b != b' && ...
        // This is needed to stop on the first sequence that hits its own std::end()
        return !(self() == rhs);
    }
};

template <typename IteratorBase, typename IteratorPack>
class totally_ordered {
    using self_type = IteratorBase;
    ZIP_ADD_CRTP_SELF_ACCESSOR(self_type)

   public:
    constexpr bool operator<(const self_type& rhs) const {
        return ttl::any(self().iterators(), rhs.iterators(), std::less{});
    }

    constexpr bool operator<=(const self_type& rhs) const {
        return ttl::any(self().iterators(), rhs.iterators(), std::less_equal{});
    }

    constexpr bool operator>(const self_type& rhs) const {
        return ttl::any(self().iterators(), rhs.iterators(), std::greater{});
    }

    constexpr bool operator>=(const self_type& rhs) const {
        return ttl::any(self().iterators(), rhs.iterators(), std::greater_equal{});
    }
};

template <typename IteratorBase, typename IteratorPack>
class incrementable {
    using self_type = IteratorBase;
    ZIP_ADD_CRTP_SELF_ACCESSOR(self_type)

   public:
    constexpr self_type& operator++() {
        ttl::for_each(self().iterators(), [](auto&& it) { ++it; });
        return self();
    }

    constexpr self_type operator++(int) {
        self_type prev{self()};
        ttl::for_each(self().iterators(), [](auto&& it) { ++it; });
        return prev;
    }
};

template <typename IteratorBase, typename IteratorPack>
class decrementable {
    using self_type = IteratorBase;
    ZIP_ADD_CRTP_SELF_ACCESSOR(self_type)

   public:
    constexpr self_type& operator--() {
        ttl::for_each(self().iterators(), [](auto&& it) { --it; });
        return self();
    }

    constexpr self_type operator--(int) {
        self_type prev{self()};
        ttl::for_each(self().iterators(), [](auto&& it) { --it; });
        return prev;
    }
};

template <typename IteratorBase, typename IteratorPack>
class random_access {
    using self_type = IteratorBase;
    ZIP_ADD_CRTP_SELF_ACCESSOR(self_type)

   public:
    constexpr self_type operator+(typename IteratorPack::difference_type rhs) const {
        return std::make_from_tuple<self_type>(
            ttl::transform<typename IteratorPack::pack_type>(
                self().iterators(), [rhs](auto&& it) { return it + rhs; }));
    }

    constexpr self_type& operator+=(typename IteratorPack::difference_type rhs) {
        ttl::for_each(self().iterators(), [rhs](auto&& it) { it += rhs; });
        return self();
    }

    constexpr self_type operator-(typename IteratorPack::difference_type rhs) const {
        return std::make_from_tuple<self_type>(
            ttl::transform<typename IteratorPack::pack_type>(
                self().iterators(), [rhs](auto&& it) { return it - rhs; }));
    }

    constexpr typename IteratorPack::difference_type operator-(
        const self_type& other) const {
        return ttl::inner_product(
            self().iterators(), other.iterators(),
            [](auto&&... prods) { return std::min({prods...}); }, std::minus{});
    }

    constexpr self_type& operator-=(typename IteratorPack::difference_type rhs) {
        ttl::for_each(self().iterators(), [rhs](auto&& it) { it -= rhs; });
        return self();
    }

    constexpr typename IteratorPack::value_type operator[](
        typename IteratorPack::difference_type rhs) const {
        return ttl::transform<typename IteratorPack::value_type>(
            self().iterators(), [rhs](auto&& it) -> decltype(auto) { return it[rhs]; });
    }
};

template <typename IteratorBase, typename IteratorPack>
struct offset {
    using self_type = IteratorBase;
    ZIP_ADD_CRTP_SELF_ACCESSOR(self_type)

    typename IteratorPack::difference_type m_offset{};

   public:
    constexpr bool operator<(const self_type& rhs) const noexcept {
        using std::get;
        return (get<0>(self().iterators()) + m_offset) <
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator<=(const self_type& rhs) const noexcept {
        using std::get;
        return (get<0>(self().iterators()) + m_offset) <=
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator>(const self_type& rhs) const noexcept {
        using std::get;
        return (get<0>(self().iterators()) + m_offset) >
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator>=(const self_type& rhs) const noexcept {
        using std::get;
        return (get<0>(self().iterators()) + m_offset) >=
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator==(const self_type& rhs) const noexcept {
        using std::get;
        return (get<0>(self().iterators()) + m_offset) ==
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr bool operator!=(const self_type& rhs) const noexcept {
        return !(self() == rhs);
    }

    // Forward interface

    constexpr self_type operator+(typename IteratorPack::difference_type rhs) const
        noexcept(std::is_nothrow_copy_constructible_v<self_type>) {
        self_type ret{self()};
        ret.m_offset += rhs;
        return ret;
    }

    constexpr self_type& operator++() noexcept {
        ++m_offset;
        return self();
    }

    constexpr self_type operator++(int) noexcept {
        self_type prev{self()};
        ++m_offset;
        return prev;
    }

    constexpr self_type& operator+=(typename IteratorPack::difference_type rhs) noexcept {
        m_offset += rhs;
        return self();
    }

    // Backward interface

    constexpr self_type operator-(typename IteratorPack::difference_type rhs) const
        noexcept(std::is_nothrow_copy_constructible_v<self_type>) {
        self_type ret{self()};
        ret.m_offset -= rhs;
        return ret;
    }

    constexpr typename IteratorPack::difference_type operator-(const self_type& rhs) const
        noexcept {
        using std::get;
        return (get<0>(self().iterators()) + m_offset) -
               (get<0>(rhs.iterators()) + rhs.m_offset);
    }

    constexpr self_type& operator--() noexcept {
        --m_offset;
        return self();
    }

    constexpr self_type operator--(int) noexcept {
        self_type ret{self()};
        --m_offset;
        return ret;
    }

    constexpr self_type& operator-=(typename IteratorPack::difference_type rhs) noexcept {
        m_offset -= rhs;
        return self();
    }

    // Dereference
    constexpr typename IteratorPack::value_type operator*() const noexcept {
        return operator[](0);
    }

    constexpr typename IteratorPack::value_type operator[](
        typename IteratorPack::difference_type rhs) const noexcept {
        rhs += m_offset;
        return ttl::transform<typename IteratorPack::value_type>(
            self().iterators(), [rhs](auto&& it) -> decltype(auto) { return it[rhs]; });
    }
};

}  // namespace policy

//
// Concrete iterators
//

struct offset_iterator_tag : public std::random_access_iterator_tag {};

template <typename IteratorPack,
          template <typename Pack, typename Self> typename... Policies>
class iterator : public IteratorPack,
                 public Policies<iterator<IteratorPack, Policies...>, IteratorPack>... {
   public:
    using IteratorPack::IteratorPack;
};

// clang-format off
template <typename... Iterators>
using forward_iterator =
    iterator<
        policy::pack<Iterators...>,
        policy::dereference,
        policy::equality_comparable,
        policy::incrementable>;

template <typename... Iterators>
using bidirectional_iterator =
    iterator<
        policy::pack<Iterators...>,
        policy::dereference,
        policy::equality_comparable,
        policy::incrementable,
        policy::decrementable>;

template <typename... Iterators>
using random_access_iterator =
    iterator<
        policy::pack<Iterators...>,
        policy::dereference,
        policy::equality_comparable,
        policy::incrementable,
        policy::decrementable,
        policy::totally_ordered,
        policy::random_access>;

template <typename... Iterators>
using offset_iterator =
    iterator<
        policy::pack<Iterators...>,
        policy::offset>;
// clang-format on

//
// Traits
//

template <std::size_t I, typename... Ts>
using nth_type_t = std::tuple_element_t<I, std::tuple<Ts...>>;

template <typename T>
using sequence_iterator_t = decltype(std::begin(std::declval<T&>()));

template <typename T>
inline constexpr bool is_iterator_category_v =
    std::is_convertible_v<std::add_lvalue_reference_t<T>,
                          std::add_lvalue_reference_t<std::forward_iterator_tag>>;

template <typename A, typename B>
inline constexpr bool is_compatible_iterator_category_v =
    is_iterator_category_v<A>&& is_iterator_category_v<B>&& std::is_convertible_v<
        std::add_lvalue_reference_t<A>, std::add_lvalue_reference_t<B>>;

// Metafunction that returns the iterator type built using
// IteratorCategory and a list of iterator types.
// The actual iterator type is selected according to IteratorCategory.
template <typename IteratorCategory, typename... Iterators>
struct iterator_type;

template <typename... Iterators>
struct iterator_type<std::forward_iterator_tag, Iterators...> {
    using type = forward_iterator<Iterators...>;
};

template <typename... Iterators>
struct iterator_type<std::bidirectional_iterator_tag, Iterators...> {
    using type = bidirectional_iterator<Iterators...>;
};

template <typename... Iterators>
struct iterator_type<std::random_access_iterator_tag, Iterators...> {
    using type = random_access_iterator<Iterators...>;
};

template <typename... Iterators>
struct iterator_type<offset_iterator_tag, Iterators...> {
    using type = offset_iterator<Iterators...>;
};

template <typename IteratorCategory, typename... Iterators>
using iterator_type_t = typename iterator_type<IteratorCategory, Iterators...>::type;

// Metafunction that returns the most specialized (given
// std::forward_iterator_tag as inheritance root) common iterator
// category tag among the iterator type list provided.
template <typename... Iterators>
using common_iterator_category_t = typename policy::pack<Iterators...>::iterator_category;

//
// Factory
//

template <typename IteratorCategory, typename... Iterators,
          typename = std::enable_if_t<is_iterator_category_v<IteratorCategory>>>
constexpr auto make_iterator(IteratorCategory, Iterators&&... args) {
    static_assert(
        is_compatible_iterator_category_v<offset_iterator_tag,
                                          common_iterator_category_t<Iterators...>>,
        "common iterator category is not compatible with requested iterator category");
    using return_type = iterator_type_t<IteratorCategory, Iterators...>;
    return return_type{std::forward<Iterators>(args)...};
}

template <
    typename... Iterators,
    typename = std::enable_if_t<!is_iterator_category_v<nth_type_t<0, Iterators...>>>>
constexpr auto make_iterator(Iterators&&... args) {
    using iterator_category = common_iterator_category_t<Iterators...>;
    return make_iterator(iterator_category{}, std::forward<Iterators>(args)...);
}

template <typename IteratorCategory, typename... Sequences>
struct zip_view {
    // Why the & is needed while declval-ing Sequences:
    // https://stackoverflow.com/questions/42580761/why-does-stdbegin-always-return-const-iterator-in-such-a-case
    // clang-format off
    using iterator_category = IteratorCategory;
    using iterator = 
        decltype(make_iterator(std::declval<iterator_category>(), std::begin(std::declval<Sequences&>())...));
    using const_iterator =
        decltype(make_iterator(std::declval<iterator_category>(), std::cbegin(std::declval<Sequences&>())...));

    using sequences = std::tuple<Sequences&...>;

    constexpr zip_view(Sequences&... sqs) noexcept : m_sequences{sqs...} {}

    constexpr iterator begin()
        noexcept((noexcept(std::begin(std::declval<Sequences&>())) && ...)) {
        return ttl::transform<iterator>(
            m_sequences, [](auto&& seq) { return std::begin(seq); });
    }

    constexpr iterator end() 
        noexcept((noexcept(std::end(std::declval<Sequences&>())) && ...)) {
        return ttl::transform<iterator>(
            m_sequences, [](auto&& seq) { return std::end(seq); });
    }

    constexpr const_iterator cbegin() const
        noexcept((noexcept(std::cbegin(std::declval<Sequences&>())) && ...)) {
        return ttl::transform<const_iterator>(
            m_sequences, [](auto&& seq) { return std::cbegin(seq); });
    }

    constexpr const_iterator cend() const
        noexcept((noexcept(std::cend(std::declval<Sequences&>())) && ...)) {
        return ttl::transform<const_iterator>(
            m_sequences, [](auto&& seq) { return std::cend(seq); });
    }
    // clang-format on

    // TODO how to reasonably SFINAE on this?
    // Cannot enable_if on:
    // * return type: without auto, it must be calculated anyway
    //   (would fail on decltype(std::declval<iterator>()[0]) when
    //   no iterator::operator[])
    // * extra function argument, operator[] *must* have exactly one
    // Just turn it into a function template for now. Ugly but works fine.
    template <typename T = typename iterator::difference_type,
              typename = std::enable_if_t<
                  (sizeof(T),
                   is_compatible_iterator_category_v<typename iterator::iterator_category,
                                                     std::random_access_iterator_tag>)>>
    constexpr auto operator[](T idx) {
        return begin()[idx];
    }

   private:
    sequences m_sequences;
};

template <typename IteratorCategory, typename... Sequences,
          typename = std::enable_if_t<is_iterator_category_v<IteratorCategory>>>
constexpr auto zip(IteratorCategory, Sequences&&... args) {
    static_assert(
        is_compatible_iterator_category_v<
            offset_iterator_tag,
            common_iterator_category_t<sequence_iterator_t<Sequences>...>>,
        "deduced common iterator category is not compatible with requested iterator "
        "category");
    using return_type = zip_view<IteratorCategory, Sequences...>;
    return return_type{std::forward<Sequences>(args)...};
}

template <
    typename... Sequences,
    typename = std::enable_if_t<!is_iterator_category_v<nth_type_t<0, Sequences...>>>>
constexpr auto zip(Sequences&&... args) {
    using iterator_category =
        common_iterator_category_t<sequence_iterator_t<Sequences>...>;
    return zip(iterator_category{}, std::forward<Sequences>(args)...);
}

}  // namespace zip

#ifdef ZIP_ADD_CRTP_SELF_ACCESSOR
#undef ZIP_ADD_CRTP_SELF_ACCESSOR
#endif

#endif
