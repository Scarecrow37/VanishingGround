#pragma once

namespace StdHelper
{
	template <typename T>
	constexpr bool is_std_array_v = false;

	template <typename T, std::size_t N>
	constexpr bool is_std_array_v<std::array<T, N>> = true;

	template <typename T>
	constexpr bool is_std_vector_v = false;

	template <typename T, typename Alloc>
	constexpr bool is_std_vector_v<std::vector<T, Alloc>> = true;

    template <typename T>
    constexpr bool is_string_view_v = std::is_same_v<T, std::string_view>;

    template <typename T>
    constexpr bool is_wstring_view_v = std::is_same_v<T, std::wstring_view>;

    template <typename T>
    using StringViewToString = std::conditional_t<is_string_view_v<T>, std::string, std::conditional_t<is_wstring_view_v<T>, std::wstring, T>>;

    template<typename Tuple, typename Func>
    void for_each_tuple(Tuple&& tuple, Func&& func)
    {
        std::apply(
            [&](auto&&... args)
            {
                (func(std::forward<decltype(args)>(args)), ...);
            },
            std::forward<Tuple>(tuple)
        );
    }
}

