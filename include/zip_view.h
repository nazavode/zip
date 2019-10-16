template <typename... Sequences>
struct zip_view {
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

    constexpr zip_view(Sequences&... sqs) noexcept : m_seq{sqs...} {}

    constexpr iterator begin() { return begin_impl(std::make_index_sequence<arity>{}); }

    constexpr iterator end() { return end_impl(std::make_index_sequence<arity>{}); }

    constexpr size_type size() const noexcept {
        return size_impl(std::make_index_sequence<arity>{});
    }

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