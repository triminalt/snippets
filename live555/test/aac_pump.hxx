
#ifndef AAC_PUMP_HXX
#define AAC_PUMP_HXX


#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>


class aac_pump final {
public:
    aac_pump( unsigned sampling_frequency
            , unsigned buffer_ms)
        : buffer_size_(buffer_size(sampling_frequency, buffer_ms)) {
    }
	~aac_pump() = default;
public:
    bool produce(std::string const& packet) {
        if (!mutex_.try_lock()) {
            return false;
        }
        if (packets_.size() > buffer_size_) {
            auto empty = std::queue<std::string>{};
            packets_.swap(empty);
        }
        packets_.push(packet);
        mutex_.unlock();
        std::this_thread::yield();
        return true;
    }

    bool consume(std::string& packet) {
       if (!mutex_.try_lock()) {
            return false;
        }
        if (packets_.empty()) {
            mutex_.unlock();
            std::this_thread::yield();
            return false;
        }
        packet = packets_.front();
        packets_.pop();
        mutex_.unlock();
        return true;
    }
private:
    static inline unsigned buffer_size( unsigned sampling_frequency
                                      , unsigned buffer_ms) {
        auto const ms_pre_frame = double{1024 * 1000} / sampling_frequency;
        return static_cast<unsigned>(buffer_ms / ms_pre_frame);
    }
public:
    unsigned buffer_size_;
private:
    std::queue<std::string> packets_;
private:
    std::mutex mutex_;
private:
};

#endif // AAC_PUMP_HXX
