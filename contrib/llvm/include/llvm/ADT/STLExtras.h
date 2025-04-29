// This header contains utilities for ranges iteration. It has been built by
// assembling several bits and pieces of LLVM's support library.
// It currently provides:
// * zip (a.k.a. zip_shortest)
// * zip_first
// * zip_equal
// * zip_longest
#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <cassert>

//===- iterator.h - Utilities for using and defining iterators --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

namespace llvm {

/// CRTP base class which implements the entire standard iterator facade
/// in terms of a minimal subset of the interface.
///
/// Use this when it is reasonable to implement most of the iterator
/// functionality in terms of a core subset. If you need special behavior or
/// there are performance implications for this, you may want to override the
/// relevant members instead.
///
/// Note, one abstraction that this does *not* provide is implementing
/// subtraction in terms of addition by negating the difference. Negation isn't
/// always information preserving, and I can see very reasonable iterator
/// designs where this doesn't work well. It doesn't really force much added
/// boilerplate anyways.
///
/// Another abstraction that this doesn't provide is implementing increment in
/// terms of addition of one. These aren't equivalent for all iterator
/// categories, and respecting that adds a lot of complexity for little gain.
///
/// Iterators are expected to have const rules analogous to pointers, with a
/// single, const-qualified operator*() that returns ReferenceT. This matches
/// the second and third pointers in the following example:
/// \code
///   int Value;
///   { int *I = &Value; }             // ReferenceT 'int&'
///   { int *const I = &Value; }       // ReferenceT 'int&'; const
///   { const int *I = &Value; }       // ReferenceT 'const int&'
///   { const int *const I = &Value; } // ReferenceT 'const int&'; const
/// \endcode
/// If an iterator facade returns a handle to its own state, then T (and
/// PointerT and ReferenceT) should usually be const-qualified. Otherwise, if
/// clients are expected to modify the handle itself, the field can be declared
/// mutable or use const_cast.
///
/// Classes wishing to use `iterator_facade_base` should implement the following
/// methods:
///
/// Forward Iterators:
///   (All of the following methods)
///   - DerivedT &operator=(const DerivedT &R);
///   - bool operator==(const DerivedT &R) const;
///   - T &operator*() const;
///   - DerivedT &operator++();
///
/// Bidirectional Iterators:
///   (All methods of forward iterators, plus the following)
///   - DerivedT &operator--();
///
/// Random-access Iterators:
///   (All methods of bidirectional iterators excluding the following)
///   - DerivedT &operator++();
///   - DerivedT &operator--();
///   (and plus the following)
///   - bool operator<(const DerivedT &RHS) const;
///   - DifferenceTypeT operator-(const DerivedT &R) const;
///   - DerivedT &operator+=(DifferenceTypeT N);
///   - DerivedT &operator-=(DifferenceTypeT N);
///
template <typename DerivedT, typename IteratorCategoryT, typename T,
          typename DifferenceTypeT = std::ptrdiff_t, typename PointerT = T *,
          typename ReferenceT = T &>
class iterator_facade_base {
   public:
    using iterator_category = IteratorCategoryT;
    using value_type = T;
    using difference_type = DifferenceTypeT;
    using pointer = PointerT;
    using reference = ReferenceT;

   protected:
    enum {
        IsRandomAccess =
            std::is_base_of<std::random_access_iterator_tag, IteratorCategoryT>::value,
        IsBidirectional =
            std::is_base_of<std::bidirectional_iterator_tag, IteratorCategoryT>::value,
    };

    /// A proxy object for computing a reference via indirecting a copy of an
    /// iterator. This is used in APIs which need to produce a reference via
    /// indirection but for which the iterator object might be a temporary. The
    /// proxy preserves the iterator internally and exposes the indirected
    /// reference via a conversion operator.
    class ReferenceProxy {
        friend iterator_facade_base;

        DerivedT I;

        ReferenceProxy(DerivedT I) : I(std::move(I)) {}

       public:
        operator ReferenceT() const { return *I; }
    };

    /// A proxy object for computing a pointer via indirecting a copy of a
    /// reference. This is used in APIs which need to produce a pointer but for
    /// which the reference might be a temporary. The proxy preserves the
    /// reference internally and exposes the pointer via a arrow operator.
    class PointerProxy {
        friend iterator_facade_base;

        ReferenceT R;

        template <typename RefT>
        PointerProxy(RefT &&R) : R(std::forward<RefT>(R)) {}

       public:
        PointerT operator->() const { return &R; }
    };

   public:
    DerivedT operator+(DifferenceTypeT n) const {
        static_assert(std::is_base_of<iterator_facade_base, DerivedT>::value,
                      "Must pass the derived type to this template!");
        static_assert(IsRandomAccess,
                      "The '+' operator is only defined for random access iterators.");
        DerivedT tmp = *static_cast<const DerivedT *>(this);
        tmp += n;
        return tmp;
    }
    friend DerivedT operator+(DifferenceTypeT n, const DerivedT &i) {
        static_assert(IsRandomAccess,
                      "The '+' operator is only defined for random access iterators.");
        return i + n;
    }
    DerivedT operator-(DifferenceTypeT n) const {
        static_assert(IsRandomAccess,
                      "The '-' operator is only defined for random access iterators.");
        DerivedT tmp = *static_cast<const DerivedT *>(this);
        tmp -= n;
        return tmp;
    }

    DerivedT &operator++() {
        static_assert(std::is_base_of<iterator_facade_base, DerivedT>::value,
                      "Must pass the derived type to this template!");
        return static_cast<DerivedT *>(this)->operator+=(1);
    }
    DerivedT operator++(int) {
        DerivedT tmp = *static_cast<DerivedT *>(this);
        ++*static_cast<DerivedT *>(this);
        return tmp;
    }
    DerivedT &operator--() {
        static_assert(
            IsBidirectional,
            "The decrement operator is only defined for bidirectional iterators.");
        return static_cast<DerivedT *>(this)->operator-=(1);
    }
    DerivedT operator--(int) {
        static_assert(
            IsBidirectional,
            "The decrement operator is only defined for bidirectional iterators.");
        DerivedT tmp = *static_cast<DerivedT *>(this);
        --*static_cast<DerivedT *>(this);
        return tmp;
    }

#ifndef __cpp_impl_three_way_comparison
    bool operator!=(const DerivedT &RHS) const {
        return !(static_cast<const DerivedT &>(*this) == RHS);
    }
#endif

    bool operator>(const DerivedT &RHS) const {
        static_assert(
            IsRandomAccess,
            "Relational operators are only defined for random access iterators.");
        return !(static_cast<const DerivedT &>(*this) < RHS) &&
               !(static_cast<const DerivedT &>(*this) == RHS);
    }
    bool operator<=(const DerivedT &RHS) const {
        static_assert(
            IsRandomAccess,
            "Relational operators are only defined for random access iterators.");
        return !(static_cast<const DerivedT &>(*this) > RHS);
    }
    bool operator>=(const DerivedT &RHS) const {
        static_assert(
            IsRandomAccess,
            "Relational operators are only defined for random access iterators.");
        return !(static_cast<const DerivedT &>(*this) < RHS);
    }

    PointerProxy operator->() const {
        return static_cast<const DerivedT *>(this)->operator*();
    }
    ReferenceProxy operator[](DifferenceTypeT n) const {
        static_assert(IsRandomAccess,
                      "Subscripting is only defined for random access iterators.");
        return static_cast<const DerivedT *>(this)->operator+(n);
    }
};

/// CRTP base class for adapting an iterator to a different type.
///
/// This class can be used through CRTP to adapt one iterator into another.
/// Typically this is done through providing in the derived class a custom \c
/// operator* implementation. Other methods can be overridden as well.
template <typename DerivedT, typename WrappedIteratorT,
          typename IteratorCategoryT =
              typename std::iterator_traits<WrappedIteratorT>::iterator_category,
          typename T = typename std::iterator_traits<WrappedIteratorT>::value_type,
          typename DifferenceTypeT =
              typename std::iterator_traits<WrappedIteratorT>::difference_type,
          typename PointerT = std::conditional_t<
              std::is_same<
                  T, typename std::iterator_traits<WrappedIteratorT>::value_type>::value,
              typename std::iterator_traits<WrappedIteratorT>::pointer, T *>,
          typename ReferenceT = std::conditional_t<
              std::is_same<
                  T, typename std::iterator_traits<WrappedIteratorT>::value_type>::value,
              typename std::iterator_traits<WrappedIteratorT>::reference, T &>>
class iterator_adaptor_base
    : public iterator_facade_base<DerivedT, IteratorCategoryT, T, DifferenceTypeT,
                                  PointerT, ReferenceT> {
    using BaseT = typename iterator_adaptor_base::iterator_facade_base;

   protected:
    WrappedIteratorT I;

    iterator_adaptor_base() = default;

    explicit iterator_adaptor_base(WrappedIteratorT u) : I(std::move(u)) {
        static_assert(std::is_base_of<iterator_adaptor_base, DerivedT>::value,
                      "Must pass the derived type to this template!");
    }

    const WrappedIteratorT &wrapped() const { return I; }

   public:
    using difference_type = DifferenceTypeT;

    DerivedT &operator+=(difference_type n) {
        static_assert(BaseT::IsRandomAccess,
                      "The '+=' operator is only defined for random access iterators.");
        I += n;
        return *static_cast<DerivedT *>(this);
    }
    DerivedT &operator-=(difference_type n) {
        static_assert(BaseT::IsRandomAccess,
                      "The '-=' operator is only defined for random access iterators.");
        I -= n;
        return *static_cast<DerivedT *>(this);
    }
    using BaseT::operator-;
    difference_type operator-(const DerivedT &RHS) const {
        static_assert(BaseT::IsRandomAccess,
                      "The '-' operator is only defined for random access iterators.");
        return I - RHS.I;
    }

    // We have to explicitly provide ++ and -- rather than letting the facade
    // forward to += because WrappedIteratorT might not support +=.
    using BaseT::operator++;
    DerivedT &operator++() {
        ++I;
        return *static_cast<DerivedT *>(this);
    }
    using BaseT::operator--;
    DerivedT &operator--() {
        static_assert(
            BaseT::IsBidirectional,
            "The decrement operator is only defined for bidirectional iterators.");
        --I;
        return *static_cast<DerivedT *>(this);
    }

    friend bool operator==(const iterator_adaptor_base &LHS,
                           const iterator_adaptor_base &RHS) {
        return LHS.I == RHS.I;
    }
    friend bool operator<(const iterator_adaptor_base &LHS,
                          const iterator_adaptor_base &RHS) {
        static_assert(
            BaseT::IsRandomAccess,
            "Relational operators are only defined for random access iterators.");
        return LHS.I < RHS.I;
    }

    ReferenceT operator*() const { return *I; }
};

}  // namespace llvm

//===- llvm/ADT/STLExtras.h - Useful STL related functions ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains some templates that are useful if you are working with
/// the STL at all.
///
/// No library is required when using these functions.
///
//===----------------------------------------------------------------------===//

namespace llvm {

namespace adl_detail {

using std::begin;

template <typename ContainerTy>
decltype(auto) adl_begin(ContainerTy &&container) {
    return begin(std::forward<ContainerTy>(container));
}

using std::end;

template <typename ContainerTy>
decltype(auto) adl_end(ContainerTy &&container) {
    return end(std::forward<ContainerTy>(container));
}

}  // end namespace adl_detail

template <typename ContainerTy>
decltype(auto) adl_begin(ContainerTy &&container) {
    return adl_detail::adl_begin(std::forward<ContainerTy>(container));
}

template <typename ContainerTy>
decltype(auto) adl_end(ContainerTy &&container) {
    return adl_detail::adl_end(std::forward<ContainerTy>(container));
}

//===----------------------------------------------------------------------===//
//     zip
//===----------------------------------------------------------------------===//

// Forward declarations required by zip_shortest/zip_equal/zip_first/zip_longest
// template <typename R, typename UnaryPredicate>
// bool all_of(R &&range, UnaryPredicate P);

// template <typename R, typename UnaryPredicate>
// bool any_of(R &&range, UnaryPredicate P);

template <typename T>
bool all_equal(std::initializer_list<T> Values);

namespace detail {

using std::declval;

// We have to alias this since inlining the actual type at the usage site
// in the parameter list of iterator_facade_base<> below ICEs MSVC 2017.
template <typename... Iters>
struct ZipTupleType {
    using type = std::tuple<decltype(*declval<Iters>())...>;
};

template <typename ZipType, typename... Iters>
using zip_traits = iterator_facade_base<
    ZipType,
    std::common_type_t<std::bidirectional_iterator_tag,
                       typename std::iterator_traits<Iters>::iterator_category...>,
    // ^ TODO: Implement random access methods.
    typename ZipTupleType<Iters...>::type,
    typename std::iterator_traits<
        std::tuple_element_t<0, std::tuple<Iters...>>>::difference_type,
    // ^ FIXME: This follows boost::make_zip_iterator's assumption that all
    // inner iterators have the same difference_type. It would fail if, for
    // instance, the second field's difference_type were non-numeric while the
    // first is.
    typename ZipTupleType<Iters...>::type *, typename ZipTupleType<Iters...>::type>;

template <typename ZipType, typename... Iters>
struct zip_common : public zip_traits<ZipType, Iters...> {
    using Base = zip_traits<ZipType, Iters...>;
    using value_type = typename Base::value_type;

    std::tuple<Iters...> iterators;

   protected:
    template <size_t... Ns>
    value_type deref(std::index_sequence<Ns...>) const {
        return value_type(*std::get<Ns>(iterators)...);
    }

    template <size_t... Ns>
    decltype(iterators) tup_inc(std::index_sequence<Ns...>) const {
        return std::tuple<Iters...>(std::next(std::get<Ns>(iterators))...);
    }

    template <size_t... Ns>
    decltype(iterators) tup_dec(std::index_sequence<Ns...>) const {
        return std::tuple<Iters...>(std::prev(std::get<Ns>(iterators))...);
    }

    template <size_t... Ns>
    bool test_all_equals(const zip_common &other, std::index_sequence<Ns...>) const {
        return ((std::get<Ns>(this->iterators) == std::get<Ns>(other.iterators)) && ...);
    }

   public:
    zip_common(Iters &&...ts) : iterators(std::forward<Iters>(ts)...) {}

    value_type operator*() const { return deref(std::index_sequence_for<Iters...>{}); }

    ZipType &operator++() {
        iterators = tup_inc(std::index_sequence_for<Iters...>{});
        return *reinterpret_cast<ZipType *>(this);
    }

    ZipType &operator--() {
        static_assert(Base::IsBidirectional,
                      "All inner iterators must be at least bidirectional.");
        iterators = tup_dec(std::index_sequence_for<Iters...>{});
        return *reinterpret_cast<ZipType *>(this);
    }

    /// Return true if all the iterator are matching `other`'s iterators.
    bool all_equals(zip_common &other) {
        return test_all_equals(other, std::index_sequence_for<Iters...>{});
    }
};

template <typename... Iters>
struct zip_first : public zip_common<zip_first<Iters...>, Iters...> {
    using Base = zip_common<zip_first<Iters...>, Iters...>;

    bool operator==(const zip_first<Iters...> &other) const {
        return std::get<0>(this->iterators) == std::get<0>(other.iterators);
    }

    zip_first(Iters &&...ts) : Base(std::forward<Iters>(ts)...) {}
};

template <typename... Iters>
class zip_shortest : public zip_common<zip_shortest<Iters...>, Iters...> {
    template <size_t... Ns>
    bool test(const zip_shortest<Iters...> &other, std::index_sequence<Ns...>) const {
        return ((std::get<Ns>(this->iterators) != std::get<Ns>(other.iterators)) && ...);
    }

   public:
    using Base = zip_common<zip_shortest<Iters...>, Iters...>;

    zip_shortest(Iters &&...ts) : Base(std::forward<Iters>(ts)...) {}

    bool operator==(const zip_shortest<Iters...> &other) const {
        return !test(other, std::index_sequence_for<Iters...>{});
    }
};

template <template <typename...> class ItType, typename... Args>
class zippy {
   public:
    using iterator = ItType<decltype(std::begin(std::declval<Args>()))...>;
    using iterator_category = typename iterator::iterator_category;
    using value_type = typename iterator::value_type;
    using difference_type = typename iterator::difference_type;
    using pointer = typename iterator::pointer;
    using reference = typename iterator::reference;

   private:
    std::tuple<Args...> ts;

    template <size_t... Ns>
    iterator begin_impl(std::index_sequence<Ns...>) const {
        return iterator(std::begin(std::get<Ns>(ts))...);
    }
    template <size_t... Ns>
    iterator end_impl(std::index_sequence<Ns...>) const {
        return iterator(std::end(std::get<Ns>(ts))...);
    }

   public:
    zippy(Args &&...ts_) : ts(std::forward<Args>(ts_)...) {}

    iterator begin() const { return begin_impl(std::index_sequence_for<Args...>{}); }
    iterator end() const { return end_impl(std::index_sequence_for<Args...>{}); }
};

}  // namespace detail

/// zip iterator for two or more iteratable types. Iteration continues until the
/// end of the *shortest* iteratee is reached.
template <typename T, typename U, typename... Args>
detail::zippy<detail::zip_shortest, T, U, Args...> zip(T &&t, U &&u, Args &&...args) {
    return detail::zippy<detail::zip_shortest, T, U, Args...>(
        std::forward<T>(t), std::forward<U>(u), std::forward<Args>(args)...);
}

/// zip iterator that assumes that all iteratees have the same length.
/// In builds with assertions on, this assumption is checked before the
/// iteration starts.
template <typename T, typename U, typename... Args>
detail::zippy<detail::zip_first, T, U, Args...> zip_equal(T &&t, U &&u, Args &&...args) {
    assert(all_equal({std::distance(adl_begin(t), adl_end(t)),
                      std::distance(adl_begin(u), adl_end(u)),
                      std::distance(adl_begin(args), adl_end(args))...}) &&
           "Iteratees do not have equal length");
    return detail::zippy<detail::zip_first, T, U, Args...>(
        std::forward<T>(t), std::forward<U>(u), std::forward<Args>(args)...);
}

/// zip iterator that, for the sake of efficiency, assumes the first iteratee to
/// be the shortest. Iteration continues until the end of the first iteratee is
/// reached. In builds with assertions on, we check that the assumption about
/// the first iteratee being the shortest holds.
template <typename T, typename U, typename... Args>
detail::zippy<detail::zip_first, T, U, Args...> zip_first(T &&t, U &&u, Args &&...args) {
    assert(std::distance(adl_begin(t), adl_end(t)) <=
               std::min({std::distance(adl_begin(u), adl_end(u)),
                         std::distance(adl_begin(args), adl_end(args))...}) &&
           "First iteratee is not the shortest");

    return detail::zippy<detail::zip_first, T, U, Args...>(
        std::forward<T>(t), std::forward<U>(u), std::forward<Args>(args)...);
}

namespace detail {
template <typename Iter>
Iter next_or_end(const Iter &I, const Iter &End) {
    if (I == End) return End;
    return std::next(I);
}

template <typename Iter>
auto deref_or_none(const Iter &I, const Iter &End)
    -> std::optional<std::remove_const_t<std::remove_reference_t<decltype(*I)>>> {
    if (I == End) return std::nullopt;
    return *I;
}

template <typename Iter>
struct ZipLongestItemType {
    using type = std::optional<
        std::remove_const_t<std::remove_reference_t<decltype(*std::declval<Iter>())>>>;
};

template <typename... Iters>
struct ZipLongestTupleType {
    using type = std::tuple<typename ZipLongestItemType<Iters>::type...>;
};

template <typename... Iters>
class zip_longest_iterator
    : public iterator_facade_base<
          zip_longest_iterator<Iters...>,
          std::common_type_t<std::forward_iterator_tag,
                             typename std::iterator_traits<Iters>::iterator_category...>,
          typename ZipLongestTupleType<Iters...>::type,
          typename std::iterator_traits<
              std::tuple_element_t<0, std::tuple<Iters...>>>::difference_type,
          typename ZipLongestTupleType<Iters...>::type *,
          typename ZipLongestTupleType<Iters...>::type> {
   public:
    using value_type = typename ZipLongestTupleType<Iters...>::type;

   private:
    std::tuple<Iters...> iterators;
    std::tuple<Iters...> end_iterators;

    template <size_t... Ns>
    bool test(const zip_longest_iterator<Iters...> &other,
              std::index_sequence<Ns...>) const {
        return ((std::get<Ns>(this->iterators) != std::get<Ns>(other.iterators)) || ...);
    }

    template <size_t... Ns>
    value_type deref(std::index_sequence<Ns...>) const {
        return value_type(
            deref_or_none(std::get<Ns>(iterators), std::get<Ns>(end_iterators))...);
    }

    template <size_t... Ns>
    decltype(iterators) tup_inc(std::index_sequence<Ns...>) const {
        return std::tuple<Iters...>(
            next_or_end(std::get<Ns>(iterators), std::get<Ns>(end_iterators))...);
    }

   public:
    zip_longest_iterator(std::pair<Iters &&, Iters &&>... ts)
        : iterators(std::forward<Iters>(ts.first)...),
          end_iterators(std::forward<Iters>(ts.second)...) {}

    value_type operator*() const { return deref(std::index_sequence_for<Iters...>{}); }

    zip_longest_iterator<Iters...> &operator++() {
        iterators = tup_inc(std::index_sequence_for<Iters...>{});
        return *this;
    }

    bool operator==(const zip_longest_iterator<Iters...> &other) const {
        return !test(other, std::index_sequence_for<Iters...>{});
    }
};

template <typename... Args>
class zip_longest_range {
   public:
    using iterator = zip_longest_iterator<decltype(adl_begin(std::declval<Args>()))...>;
    using iterator_category = typename iterator::iterator_category;
    using value_type = typename iterator::value_type;
    using difference_type = typename iterator::difference_type;
    using pointer = typename iterator::pointer;
    using reference = typename iterator::reference;

   private:
    std::tuple<Args...> ts;

    template <size_t... Ns>
    iterator begin_impl(std::index_sequence<Ns...>) const {
        return iterator(
            std::make_pair(adl_begin(std::get<Ns>(ts)), adl_end(std::get<Ns>(ts)))...);
    }

    template <size_t... Ns>
    iterator end_impl(std::index_sequence<Ns...>) const {
        return iterator(
            std::make_pair(adl_end(std::get<Ns>(ts)), adl_end(std::get<Ns>(ts)))...);
    }

   public:
    zip_longest_range(Args &&...ts_) : ts(std::forward<Args>(ts_)...) {}

    iterator begin() const { return begin_impl(std::index_sequence_for<Args...>{}); }
    iterator end() const { return end_impl(std::index_sequence_for<Args...>{}); }
};
}  // namespace detail

/// Iterate over two or more iterators at the same time. Iteration continues
/// until all iterators reach the end. The std::optional only contains a value
/// if the iterator has not reached the end.
template <typename T, typename U, typename... Args>
detail::zip_longest_range<T, U, Args...> zip_longest(T &&t, U &&u, Args &&...args) {
    return detail::zip_longest_range<T, U, Args...>(
        std::forward<T>(t), std::forward<U>(u), std::forward<Args>(args)...);
}

}  // namespace llvm
