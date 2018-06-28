//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-06-27
//


#ifndef BITS_HXX
#define BITS_HXX


#include <climits>
#include <type_traits>
#include <array>
#include <cstdint>

template<std::size_t N, typename
                      = std::enable_if_t<0 == N % sizeof(std::uint8_t)>
        >
struct bits {
    bits() = delete;
    ~bits() = delete;

    template<std::size_t N

    template< std::size_t I
            , std::size_t S
            , typename T = std::uint32_t
            , typename = std::enable_if_t< std::is_integral<T>::value
                                         && (S > 0)
                                         && (I + S < sizeof(std::uint8_t) * N)
                                         >
            >
    static inline T parse(std::array<uint8_t, N> const& bytes) {
        return T{}
    }
private:
    std::uint8_t* data_;
};

#endif // BITS_HXX
