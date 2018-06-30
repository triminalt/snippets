//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-05-04
//


#ifndef ADTS_HXX
#define ADTS_HXX


#include <climits>
#include <bitset>
#include <array>
#include <cstdint>
#include "./bit.hxx"


//
// - ADTS (Audio Data Transport Stream), Please refer to:
//    http://mpeg-audio.org/docs/w14751_(mpeg_AAC_TransportFormats).pdf
//    https://wiki.multimedia.cx/index.php?title=ADTS
//
// - ADTS Frame
//    #==========+==========+----------+===========#
//    #          |          |          |           #
//    # 28 bit   | 28 bit   | 16 bit   | data len  #
//    #          |          |          |           #
//    # ADTS     | ADTS     | OPTIONAL | Raw       #
//    # Fixed    | Variable | CRC      | Data      #
//    # Header   | Header   | Check    | Block     #
//    #          |          |          |           #
//    #==========+==========+=---------+===========#
//
// - ADTS Fixed Header
//    - Structure
//       AAAA AAAA AAAA BCCD EEFF FFGH HHIJ
//
//       +------+------+--------------------------+
//       | Sign | Bits | Feild                    |
//       +------+------+--------------------------+
//       | A    | 12   | Syncword                 |
//       | B    | 1    | ID                       |
//       | C    | 2    | Layer                    |
//       | D    | 1    | Protection Absent        |
//       | E    | 2    | Profile Object Type      |
//       | F    | 4    | Sampling Frequency Index |
//       | G    | 1    | Private Bit              |
//       | H    | 3    | Channel Configuration    |
//       | I    | 1    | Original / Copy          |
//       | J    | 1    | Home                     |
//       +------+------+--------------------------+
//
//   - Description
//      - Syncword
//         0xFFF, all bits must be 1
//      - ID
//         0 for MPEG-4, 1 for MPEG-2
//      - Layer
//         Always 0
//      - Protection Absent
//         Set to 0 if there is CRC and 1 if there is no CRC
//      - Profile Object Type
//         The MPEG-4 Audio Object Type minus 1
//      - Sampling Frequency Index
//         0:  96000 Hz
//         1:  88200 Hz
//         2:  64000 Hz
//         3:  48000 Hz
//         4:  44100 Hz
//         5:  32000 Hz
//         6:  24000 Hz
//         7:  22050 Hz
//         8:  16000 Hz
//         9:  12000 Hz
//         10: 11025 Hz
//         11: 8000 Hz
//         12: 7350 Hz
//         13: Reserved
//         14: Reserved
//         15: frequency is written explictly
//      - Private Bit
//      - Channel Configuration
//      - Original / Copy
//         Set to 0 when encoding, ignore when decoding
//      - Home
//         Set to 0 when encoding, ignore when decoding
//
// - ADTS Variable Header
//    - Structure
//       KLMM MMMM MMMM MMMO OOOO OOOO OOPP
//
//       +------+------+------------------------------------+
//       | Sign | Bits | Feild                              |
//       +------+------+------------------------------------+
//       | K    | 1    | Copyright ID Bit                   |
//       | L    | 1    | Copyright ID Start                 |
//       | M    | 13   | AAC Frame Length                   |
//       | O    | 11   | ADTS Buffer Fullness               |
//       | P    | 2    | Number of Raw Data Blocks In Frame |
//       +------+------+------------------------------------+
//
//   - Description
//      - Copyright ID Bit
//      - Copyright ID Start
//      - AAC Frame Length 
//      - ADTS Buffer Fullness
//      - Number of Raw Data Blocks In Frame
//

class adts {
    class header {
    public:
        class fixed {
        public:
            fixed() = default;
            fixed( unsigned id
                 , unsigned layer
                 , unsigned protection_absent
                 , unsigned profile_object_type
                 , unsigned sampling_frequency_index
                 , unsigned private_bit
                 , unsigned channel_configuration
                 , unsigned original_copy
                 , unsigned home)
                : id_(id)
                , layer_( layer_)
                , protection_absent_(protection_absent)
                , profile_object_type_(profile_object_type)
                , sampling_frequency_index_(sampling_frequency_index)
                , private_bit_(private_bit)
                , channel_configuration_(channel_configuration)
                , original_copy_(original_copy)
                , home_(home) {
                // EMPTY
            }

            unsigned id() const {
                return id_;
            }
            fixed& id(unsigned id) {
                id_ = id;
                return *this;
            }

            unsigned layer() const {
                return layer_;
            }
            fixed& layer(unsigned layer) {
                layer_ = layer;
                return *this;
            }

            unsigned protection_absent() const {
                return protection_absent_;
            }
            fixed& protection_absent(unsigned absent) {
                protection_absent_ = absent;
                return *this;
            }

            unsigned profile_object_type() const {
                return profile_object_type_;
            }
            fixed& profile_object_type(unsigned type) {
                profile_object_type_ = type;
                return *this;
            }

            unsigned sampling_frequency_index() const {
                return sampling_frequency_index_;
            }
            fixed& sampling_frequency_index(unsigned index) {
                sampling_frequency_index_ = index;
                return *this;
            }

            unsigned private_bit() const {
                return private_bit_;
            }
            fixed& private_bit(unsigned bit) {
                private_bit_ = bit;
                return *this;
            }

            unsigned channel_configuration() const {
                return channel_configuration_;
            }
            fixed& channel_configuration(unsigned cfg) {
                channel_configuration_ = cfg;
                return *this;
            }

            unsigned original_copy() const {
                return original_copy_;
            }
            fixed& original_copy(unsigned copy) {
                original_copy_ = copy;
                return *this;
            }

            unsigned home() const {
                return home_;
            }
            fixed& home(unsigned home) {
                home_ = home;
                return *this;
            }
        private:
            static fixed parse(std::array<std::uint8_t, 7> const& bytes) {
                if (!parse_sw(bytes)) {
                    return fixed{};
                }
                //return fixed{parse_id(bytes), parse}
                //auto const id = parse_id(bytes);
            }
        private:
            static inline bool parse_sw(std::array<uint8_t, 7> const& bytes) {
                return is_ff(bytes[0]) && is_f(bytes[1]);
            }
            static unsigned parse_id(std::array<std::uint8_t, 7> const& bytes) {
                return bytes[1] >> 3 && 0x01;
            }
            static unsigned parse_l(std::array<std::uint8_t, 7> const& bytes) {
                return bytes[1] >> 1 && 0x03;
            }
            static unsigned parse_pa(std::array<std::uint8_t, 7> const& bytes) {
                return bytes[1] >> 0 && 0x01;
            }
            static unsigned parse_pot(std::array<std::uint8_t, 7> const& bytes) {
                return bytes[2] >> 6 && 0x02;
            }
            static unsigned parse_sfi(std::array<std::uint8_t, 7> const& bytes) {
                return bytes[2] >> 6 && 0x02;
            }
        private:
            static inline bool is_ff(uint8_t b) {
                return 0x0ff == util::bit::extract<8, 0, 8>(b);
            }
            static inline bool is_f(uint8_t b) {
                return 0x0f == util::bit::extract<8, 0, 4>(b);
            }
            static inline unsigned extract(uint8_t b, std::size_t index, std::size_t size) {
                return b & 0xff;
            }
        private:
            unsigned id_;
            unsigned layer_;
            unsigned protection_absent_;
            unsigned profile_object_type_;
            unsigned sampling_frequency_index_;
            unsigned private_bit_;
            unsigned channel_configuration_;
            unsigned original_copy_;
            unsigned home_;
        private:
            unsigned id_;
            unsigned layer_;
            unsigned protection_absent_;
            unsigned profile_object_type_;
            unsigned sampling_frequency_index_;
            unsigned private_bit_;
            unsigned channel_configuration_;
            unsigned original_copy_;
            unsigned home_;
        };

        class variable {
        public:
            variable( unsigned copyright_id_bit
                    , unsigned copyright_id_start
                    , unsigned aac_frame_length
                    , unsigned adts_buffer_fullness
                    , unsigned number_of_raw_data_blocks_in_frame)
            : copyright_id_bit_(copyright_id_bit)
            , copyright_id_start_(copyright_id_start)
            , aac_frame_length_(aac_frame_length)
            , adts_buffer_fullness_(adts_buffer_fullness)
            , number_of_raw_data_blocks_in_frame_
            ( number_of_raw_data_blocks_in_frame) {
            // EMPTY
        }
        public:
            unsigned copyright_id_bit() const {
                return copyright_id_bit_;
            }
            variable& copyright_id_bit(unsigned bit) {
                copyright_id_bit_ = bit;
                return *this;
            }

            unsigned copyright_id_start() const {
                return copyright_id_start_;
            }
            variable& copyright_id_start(unsigned start) {
                copyright_id_start_ = start;
            }

            unsigned aac_frame_length() const {
                return aac_frame_length_;
            }
            variable& aac_frame_length(unsigned length) {
                aac_frame_length_ = length;
            }

            unsigned adts_buffer_fullness() const {
                return adts_buffer_fullness_;
            }
            variable& adts_buffer_fullness(unsigned fullness) {
                adts_buffer_fullness_ = fullness;
            }

            unsigned number_of_raw_data_blocks_in_frame() const {
                return number_of_raw_data_blocks_in_frame_;
            }
            variable& number_of_raw_data_blocks_in_frame(unsigned number) {
                number_of_raw_data_blocks_in_frame_ = number;
            }
        private:
            unsigned copyright_id_bit_;
            unsigned copyright_id_start_;
            unsigned aac_frame_length_;
            unsigned adts_buffer_fullness_;
            unsigned number_of_raw_data_blocks_in_frame_;
        };
    public:
        static header parse(std::array<std::uint8_t, 7> const& bytes) {
        
        }
    private:
        template< std::size_t N
                , std::size_t Index
                , std::size_t Bits
                , std::enable_if_t<  (0 != N)
                                  && (Bits > 0)
                                  && ((Index + Bits) < sizeof(std::uint8_t) * N)
                                  && (Bits < sizeof(unsigned))
                                  >
                >
        static inline unsigned parse(std::array<std::uint8_t, N> const& bytes) {
            auto const byte_index = Index / sizeof(std::uint8_t);
            auto const bits_shift = Index % sizeof(std::uint8_t) + bits - 1;
            auto const byte_count = Bits / sizeof(std::uint8_t);
            
            for ()
        }
    private:
        fixed fixed_;
        variable variable_;
    private:
    };
public:
private:
};

#endif // ADTS_HXX
