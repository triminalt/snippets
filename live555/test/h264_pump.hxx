
#ifndef H264_PUMP_HXX
#define H264_PUMP_HXX


#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>


class h264_pump final {
public:
    h264_pump (unsigned fps, unsigned buffer_ms)
        : buffer_size_(buffer_size(fps, buffer_ms)) {
    }
    ~h264_pump() = default;
public:
    bool produce(std::string const& packet) {
        if (!mutex_.try_lock()) {
            return false;
        }
        if (frames_.size() > buffer_size_) {
            auto empty = std::queue<std::string>{};
            frames_.swap(empty);
        }
        frames_.push(packet);
        mutex_.unlock();
        std::this_thread::yield();
        return true;
    }

    bool consume(std::string& packet) {
       if (!mutex_.try_lock()) {
            return false;
        }
        if (frames_.empty()) {
            mutex_.unlock();
            std::this_thread::yield();
            return false;
        }
        packet = frames_.front();
        frames_.pop();
        mutex_.unlock();
        return true;
    }
private:
    static inline unsigned buffer_size(unsigned fps, unsigned buffer_ms) {
        auto const ms_pre_frame = double{1000} / fps;
        return static_cast<unsigned>(buffer_ms / ms_pre_frame);
    }
public:
    unsigned buffer_size_;
private:
    std::queue<std::string> frames_;
private:
    std::mutex mutex_;
private:
};

#endif // AAC_PUMP_HXX
