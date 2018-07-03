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

    static std::uint8_t get_nal_unit_type(std::uint8_t header) {
        return 0x1f & header;
    }

    static bool is_idr(std::uint8_t header) {
        return 0x05 == get_nal_unit_type(header);
    }

    static bool is_sps(std::uint8_t header) {
        return 0x07 == get_nal_unit_type(header);
    }

    static bool is_pps(std::uint8_t header) {
        return 0x08 == get_nal_unit_type(header);
    }
};


#endif
