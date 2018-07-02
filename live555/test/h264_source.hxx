
#ifndef H264_SOURCE_HXX
#define H264_SOURCE_HXX

#include <cstddef>
#include <cstdint>
#include <string>
#include <GroupsockHelper.hh>
#include <FramedSource.hh>
#include "./h264_pump.hxx"

class h264_source final: public FramedSource {

public:
    static h264_source* createNew( UsageEnvironment& env
                                 , h264_pump* pump
                                 , unsigned fps) {
        return new h264_source(env, pump, fps);
    }
public:
    unsigned fps() const {
        return fps_;
    }
    h264_source( UsageEnvironment& env
               , h264_pump* pump
               , unsigned fps)
        : FramedSource(env)
        , pump_(pump)
        , fps_(fps) {
    }
	virtual ~h264_source() = default;
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
    h264_pump* pump_;
private:
    unsigned fps_;
	unsigned usecs_pre_frame_;
private:
    char config_str_[5];
};


#endif
