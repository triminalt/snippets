//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef AAC_SOURCE_HXX
#define AAC_SOURCE_HXX


#include <cstddef>
#include <cstdint>
#include <string>
#include <GroupsockHelper.hh>
#include <FramedSource.hh>
#include "./aac_pump.hxx"

class aac_source final: public FramedSource {
public:
    aac_source( UsageEnvironment& env
              , aac_pump* pump
              , std::uint8_t profile
              , std::uint8_t sampling_freq_idx
              , std::uint8_t channel_cfg)
        : FramedSource(env)
        , pump_(pump)
        , profile_(profile)
        , sampling_frequency_(sampling_frequency(sampling_freq_idx))
        , channels_(channel_cfg == 0 ? 2 : channel_cfg)
        , usecs_pre_frame_((1024 * 1000000) / sampling_frequency(sampling_freq_idx)) {
        std::uint8_t specific_cfg[2] = {0};
        std::uint8_t const object_type = profile + 1;
        specific_cfg[0] = (object_type << 3) | (sampling_freq_idx >> 1);
        specific_cfg[1] = (sampling_freq_idx << 7) | (channel_cfg << 3);
        sprintf(config_, "%02X%02x", specific_cfg[0], specific_cfg[1]);
    }
    virtual ~aac_source() = default;
public:
    unsigned sampling_frequency() const {
        return sampling_frequency_;
    }
    unsigned channels() const {
        return channels_;
    }
    char const* config() const {
        return config_;
    }
private:
    static inline unsigned sampling_frequency(std::size_t i) {
        static unsigned constexpr table[16] = {
            96000 , 88200 , 64000 , 48000
          , 44100 , 32000 , 24000 , 22050
          , 16000 , 12000 , 11025 , 8000
          , 7350  , 0     , 0     , 0
        };
        return table[i];
    }
private:
    virtual void doGetNextFrame() override {
        std::string packet;
        if (!pump_->consume(packet) && packet.size() < 7) {
            schedule();
            return;
        }
        if (!check(packet)) {
            schedule();
            return;
        }
        get(packet);
        pt();
        schedule();
    }
private:
    bool check(std::string const& packet) {
        auto const size = packet.size();
        if (size < 7) {
            return false;
        }
        unsigned char byte0 = static_cast<unsigned char>(packet[0]);
        unsigned char byte1 = static_cast<unsigned char>(packet[1]);
        auto const is_ff = 0xff == byte0;
        auto const is_f = 0xf0 == (0xf0 & byte1);
        if (!(is_ff && is_f)) {
            return false;
        }
        auto const len =static_cast<std::uint16_t>(((packet[3] & 0x03) << 11)
                                      | (packet[4] << 3)
                                      | ((packet[5] & 0xE0) >> 5));
        if (size != len) {
            return false;
        }
        return true;
    }
    void get(std::string const& packet) {
        auto const packet_size = packet.size();
        auto const protection_absent = static_cast<bool>(packet[1] & 0x01);
        u_int16_t frame_length = ((packet[3] & 0x03) << 11)
                               | (packet[4] << 3)
                               | ((packet[5] & 0xE0) >> 5);
        if (frame_length != packet_size) {
            schedule();
            return;
        }
        auto const data_index = protection_absent ? 7 : 9;
        auto const data_size = protection_absent ? (packet_size - 7)
                                                 : (packet_size - 9);
        fFrameSize = (data_size > fMaxSize) ? fMaxSize : data_size;
        fNumTruncatedBytes = (data_size > fMaxSize) ? (data_size - fMaxSize)
                                                    : 0;
        memcpy(fTo, packet.c_str() + data_index, fFrameSize);
    }
    void pt() {
        if (0 == fPresentationTime.tv_sec && 0 == fPresentationTime.tv_usec) {
            gettimeofday(&fPresentationTime, nullptr);
        } else {
            unsigned uSeconds = fPresentationTime.tv_usec + usecs_pre_frame_;
            fPresentationTime.tv_sec += uSeconds / 1000000;
            fPresentationTime.tv_usec = uSeconds % 1000000;
        }
        fDurationInMicroseconds = usecs_pre_frame_;
    }
    void schedule() {
        auto& task = nextTask();
        auto const f = reinterpret_cast<TaskFunc*>(FramedSource::afterGetting);
        task = envir().taskScheduler().scheduleDelayedTask(0, f, this);
    }
private:
    aac_pump* pump_;
private:
    unsigned profile_;
    unsigned sampling_frequency_;
    unsigned channels_;
    unsigned usecs_pre_frame_;
private:
    char config_[5];
};


#endif // AAC_SOURCE_HXX
