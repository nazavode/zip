template <typename... Iterators>
struct zip_unsafe_random_access_iterator {
    static_assert(
        (std::is_same_v<typename std::iterator_traits<Iterators>::iterator_category,
                        std::random_access_iterator_tag> &&
         ...),
        "zip_unsafe_random_access_iterator supports only random access iterators");

    using iterators_tuple_type = std::tuple<std::remove_reference_t<Iterators>...>;

    using value_type =
        std::tuple<std::remove_reference_t<decltype(*std::declval<Iterators>())>&...>;
    using difference_type =
        std::common_type_t<typename std::iterator_traits<Iterators>::difference_type...>;
    using reference = value_type;
    using pointer = value_type;
    using iterator_category = std::random_access_iterator_tag;

    explicit constexpr zip_unsafe_random_access_iterator(Iterators... iterators) noexcept
        : m_it{iterators...} {}

    constexpr zip_unsafe_random_access_iterator operator+(difference_type rhs) const
        noexcept {
        zip_unsafe_random_access_iterator ret{*this};
        ret.m_offset += rhs;
        return ret;
    }

    constexpr zip_unsafe_random_access_iterator operator-(difference_type rhs) const
        noexcept {
        zip_unsafe_random_access_iterator ret{*this};
        ret.m_offset -= rhs;
        return ret;
    }

    constexpr difference_type operator-(
        const zip_unsafe_random_access_iterator& rhs) const noexcept {
        return (std::get<0>(m_it) + m_offset) - (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr zip_unsafe_random_access_iterator& operator++() noexcept {
        ++m_offset;
        return *this;
    }

    constexpr zip_unsafe_random_access_iterator operator++(int) noexcept {
        zip_unsafe_random_access_iterator ret{*this};
        ++m_offset;
        return ret;
    }

    constexpr zip_unsafe_random_access_iterator& operator--() noexcept {
        --m_offset;
        return *this;
    }

    constexpr zip_unsafe_random_access_iterator operator--(int) noexcept {
        zip_unsafe_random_access_iterator ret{*this};
        --m_offset;
        return ret;
    }

    constexpr zip_unsafe_random_access_iterator& operator+=(
        difference_type rhs) noexcept {
        m_offset += rhs;
        return *this;
    }

    constexpr zip_unsafe_random_access_iterator& operator-=(
        difference_type rhs) noexcept {
        m_offset -= rhs;
        return *this;
    }

    constexpr bool operator<(const zip_unsafe_random_access_iterator& rhs) const
        noexcept {
        return (std::get<0>(m_it) + m_offset) < (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator<=(const zip_unsafe_random_access_iterator& rhs) const
        noexcept {
        return (std::get<0>(m_it) + m_offset) <= (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator>(const zip_unsafe_random_access_iterator& rhs) const
        noexcept {
        return (std::get<0>(m_it) + m_offset) > (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator>=(const zip_unsafe_random_access_iterator& rhs) const
        noexcept {
        return (std::get<0>(m_it) + m_offset) >= (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator==(const zip_unsafe_random_access_iterator& rhs) const
        noexcept {
        return (std::get<0>(m_it) + m_offset) == (std::get<0>(rhs.m_it) + rhs.m_offset);
    }

    constexpr bool operator!=(const zip_unsafe_random_access_iterator& rhs) const
        noexcept {
        return (std::get<0>(m_it) + m_offset) != (std::get<0>(rhs.m_it) + rhs.m_offset);
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
        return ttl::transform<value_type>(
            m_it, [rhs](auto&& it) -> decltype(auto) { return it[rhs]; });
    }

   private:
    difference_type m_offset{0};
    iterators_tuple_type m_it;
};
