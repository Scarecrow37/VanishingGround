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

