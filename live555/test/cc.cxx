
#include <type_traits>
#include <iostream>

template< std::size_t N
        , typename = std::enable_if_t<(N > 0 && N <= 64)>
        >
struct integer {
    static auto constexpr size = (N > 0 && N <= 8) ? 8
                                                   : (N > 8 && N <= 16)
                                                   ? 16
                                                   : (N > 16 && N <= 32)
                                                   ? 32
                                                   : 64;
    using type = typename integer<size>::type;
    static auto constexpr max = integer<size>::max;
};

template<>
struct integer<8> {
    using type = uint_least8_t;
    static auto constexpr max = UINT_LEAST8_MAX;
};

template<>
struct integer<16> {
    using type = std::uint_least16_t;
    static auto constexpr max = UINT_LEAST16_MAX;
};

template<>
struct integer<32> {
    using type = std::uint_least32_t;
    static auto constexpr max = UINT_LEAST32_MAX;
};

template<>
struct integer<64> {
    using type = std::uint_least64_t;
    static auto constexpr max = UINT_LEAST64_MAX;
};

template<std::size_t N>
using integer_t = typename integer<N>::type;

template<std::size_t N>
static auto constexpr integer_m = integer<N>::max;

template< std::size_t N
        , typename = std::enable_if_t<(N > 0)>
        >
struct bits_max {
    static integer_t<N> constexpr value = (1 << (N-1))| mask<N - 1>::value;
};

template<>
struct bits_max<1> {
    static integer_t<1> constexpr value = 1;
};

template< std::size_t N>
static auto constexpr bits_max_v = bits_max<N>::value;

int main() {
    std::cout << "1 : " << bits_max_v<1> << std::endl;
    std::cout << "2 : " << bits_max_v<2> << std::endl;
    std::cout << "3 : " << bits_max_v<3> << std::endl;
    std::cout << "4 : " << bits_max_v<4> << std::endl;

    return 0;
}