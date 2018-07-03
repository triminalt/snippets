//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-06-27
//


#ifndef UTIL_BIT_HXX
#define UTIL_BIT_HXX


#include <cstdint>
#include <type_traits>
#include <array>

namespace util {
namespace bit {
    // integer
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

    // integer_t
    template<std::size_t N>
    using integer_t = typename integer<N>::type;

    // integer_m
    template<std::size_t N>
    static auto constexpr integer_m = integer<N>::max;

    // mask_helper
    template< std::size_t N
            , typename = std::enable_if_t<(N > 0)>
            >
    struct mask_helper {
        static integer_t<N> constexpr value = (1 << (N-1))
                                            | mask_helper<N - 1>::value;
    };

    template<>
    struct mask_helper<1> {
        static integer_t<1> constexpr value = 1;
    };

    // mask_helper_v
    template< std::size_t N>
    static auto constexpr mask_helper_v = mask_helper<N>::value;

    // mask
    template< std::size_t N
            , std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<( N > 0
                                         && Index >= 0
                                         && Index < N
                                         && Size > 0
                                         && Size <= N
                                         && Index + Size <= N
                                         )>
            >
    struct mask {
        static auto constexpr value = mask_helper_v<Size> << (N - Index + Size);
    };

    // mask_v
    template<std::size_t N, std::size_t Index, std::size_t Size>
    static auto constexpr mask_v = mask<N, Index, Size>::value;

    template< std::size_t N
            , std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<( N > 0
                                         && Index >= 0
                                         && Index < N
                                         && Size > 0
                                         && Size <= N
                                         && Index + Size <= N
                                         )>
            >
    integer_t<Size> extract(integer_t<N> i) {
        return i | mask_v<N, Index, Size>;
    }

#if 0
    // cover
    template< std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<(Index >= 0 && Size > 0)>
            >
    struct cover {
        static auto constexpr value = (Index + Size) / 8 - Index / 8 + 1;
    };

    // cover_v
    template<std::size_t Index, std::size_t Size>
    static auto constexpr cover_v = cover<Index, Size>::value;

    // head
    template< std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<(Index >= 0 && Size > 0)>
            >
    struct head {
        static auto constexpr byte = Index / 8;
        static auto constexpr index = Index % 8;
        static auto constexpr size = 8 - index;
    };

    // head_b
    template<std::size_t Index, std::size_t Size>
    static auto constexpr head_b = head<Index, Size>::byte;

    // head_i
    template<std::size_t Index, std::size_t Size>
    static auto constexpr head_i = head<Index, Size>::index;

    // head_s
    template<std::size_t Index, std::size_t Size>
    static auto constexpr head_s = head<Index, Size>::size;

    // has_tail
    template< std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<(Index >= 0 && Size > 0)>
            >
    struct has_tail {
        static auto constexpr value = (cover_v<Index, Size>) > 1;
    };

    // has_tail_v
    template<std::size_t Index, std::size_t Size>
    static auto constexpr has_tail_v = has_tail<Index, Size>::value;

    // tail
    template< std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<has_tail_v<Index, Size>>
            >
    struct tail {
        static auto constexpr byte = (Index + Size) / 8;
        static auto constexpr index = 0;
        static auto constexpr size = (Index + Size) % 8;
    };

    // tail_b
    template<std::size_t Index, std::size_t Size>
    static auto constexpr tail_b = head<Index, Size>::byte;

    // tail_i
    template<std::size_t Index, std::size_t Size>
    static auto constexpr tail_i = head<Index, Size>::index;

    // head_s
    template<std::size_t Index, std::size_t Size>
    static auto constexpr tail_s = head<Index, Size>::size;

    // has_waist
    template< std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<(Index >= 0 && Size > 0)>
            >
    struct has_waist {
        static auto constexpr value = (cover_v<Index, Size>) > 2;
    };

    // has_waist_v
    template<std::size_t Index, std::size_t Size>
    static auto constexpr has_waist_v = has_waist<Index, Size>::value;

    // partition
    template< std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<(Index >= 0 && Size > 0)>
            >

    template< std::size_t N
            , std::size_t Index
            , std::size_t Size
            , std::size_t CoverIndex
            , std::size_t CoverSize>
    struct partition {
    };

    template< std::size_t N
            , std::size_t Index
            , std::size_t Size
            , std::size_t CoverIndex
            , std::size_t CoverSize>
    struct partition<1> {

    };

    // spilt
    template< std::size_t N
            , std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<( (N > 0)
                                         && (Size > 0)
                                         && (Index + Size < N)
                                         )>
            >
    integer_t<Size> constexpr parse(std::array<std::uint8_t, N> const& bytes) {
        bytes[]
        auto constexpr cover_bytes = cover_v<N, Index, Size>;

    }

    // parse
    template< std::size_t N
            , std::size_t Index
            , std::size_t Size
            , typename = std::enable_if_t<( (N > 0)
                                         && (Size > 0)
                                         && (Index + Size < N)
                                         )>
            >
    integer_t<Size> constexpr parse(std::array<std::uint8_t, N> const& bytes) {
        auto constexpr cover_bytes = cover_v<N, Index, Size>;
        if (1 == cover_bytes) {

        }
    }
    //
#endif
} // bit
} // util


#endif // UTIL_BIT_HXX
