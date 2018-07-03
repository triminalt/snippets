//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef AAC_UTILS_HXX
#define AAC_UTILS_HXX


#include <cstdint>
#include <string>


struct aac_utils {
    aac_utils() = delete;
    ~aac_utils() = delete;

    static bool has_syncword(std::string const& packet) {
        if (packet.size() < 2) {
            return false;
        }
        return 0xff == packet[0] && 0xf0 == (0xf0 & packet[1]);
    }

    static std::uint16_t packet_len(std::string const& packet) {
        if (packet.size() < 6) {
            return 0;
        }
        return   ((packet[3] & 0x03) << 11)
               | (packet[4] << 3)
               | ((packet[5] & 0xE0) >> 5);
    }

    static bool has_syncword(char const* bytes, std::size_t size) {
        if (size < 2) {
            return false;
        }
        return    std::uint8_t{0xff} == static_cast<uint8_t>(bytes[0])
               && std::uint8_t{0xf0} == ( std::uint8_t{0xf0}
                                        & static_cast<uint8_t>(bytes[1]));
    }

    static std::uint16_t packet_len(char const* bytes, std::size_t size) {
        if (size < 3) {
            return 0;
        }
        return   ((bytes[0] & 0x03) << 11)
               | (bytes[1] << 3)
               | ((bytes[2] & 0xE0) >> 5);
    }
};


#endif // AAC_UTILS_HXX
