
#ifndef H264_SOURCE_HXX
#define H264_SOURCE_HXX

#include <cstddef>
#include <cstdint>
#include <string>
#include <GroupsockHelper.hh>
#include <FramedSource.hh>
#include "./h264_utils.hxx"
#include "./h264_pump.hxx"

class h264_source final: public FramedSource {
public:
    h264_source( UsageEnvironment& env
               , h264_pump* pump
               , unsigned fps)
        : FramedSource(env)
        , pump_(pump)
        , fps_(fps) {
    }
	virtual ~h264_source() = default;
public:
    unsigned fps() const {
        return fps_;
    }
private:
    virtual void doGetNextFrame() override {
        std::string frame;
        if (!pump_->consume(frame) && frame.size() < 7) {
            schedule();
            return;
        }
        if (!check(frame)) {
            schedule();
            return;
        }
        get(frame);
        pt();
        schedule();
    }
private:
    bool check(std::string const& frame) {
        return h264_utils::has_start_code(frame);
    }
    void get(std::string const& frame) {
        auto const data_size = frame.size();
        fFrameSize = (data_size > fMaxSize) ? fMaxSize : data_size;
        fNumTruncatedBytes = (data_size > fMaxSize) ? (data_size - fMaxSize)
                                                    : 0;
        memcpy(fTo, frame.c_str(), fFrameSize);
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
};


#endif
