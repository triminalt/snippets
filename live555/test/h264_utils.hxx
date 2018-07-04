//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef H264_UTILS_HXX
#define H264_UTILS_HXX


#include <cstdint>
#include <cstring>
#include <string>
#include <array>


struct h264_utils {
    h264_utils() = delete;
    ~h264_utils() = delete;

    static bool has_start_code(std::string const& frame) {
        if (frame.size() < 4) {
            return false;
        }
        return 0 == memcmp( std::array<char, 4>{0x00, 0x00, 0x00, 0x01}.data()
                          , frame.c_str()
                          , 4);
    }

    static bool has_start_code(char const* bytes, std::size_t size) {
        if (size < 4) {
            return false;
        }
        return 0 == memcmp( std::array<char, 4>{0x00, 0x00, 0x00, 0x01}.data()
                          , bytes
                          , 4);
    }

    static std::uint8_t get_nal_unit_type(std::uint8_t header) {
        return std::uint8_t{0x1f} & header;
    }

    static bool is_idr(std::uint8_t header) {
        return std::uint8_t{0x05} == get_nal_unit_type(header);
    }

    static bool is_sps(std::uint8_t header) {
        return std::uint8_t{0x07} == get_nal_unit_type(header);
    }

    static bool is_pps(std::uint8_t header) {
        return std::uint8_t{0x08} == get_nal_unit_type(header);
    }
};


#endif // H264_UTILS_HXX
