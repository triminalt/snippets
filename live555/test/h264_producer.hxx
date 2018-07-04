//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef H264_PRODUCER_HXX
#define H264_PRODUCER_HXX


#include <atomic>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <queue>
#include <fstream>
#include "h264_utils.hxx"
#include "./h264_pump.hxx"


class h264_producer {
public:
    h264_producer(std::shared_ptr<h264_pump> const& pump)
        : pump_(pump) {
        std::ifstream ifs{"test.h264", std::ios::binary};
        if (!ifs) {
            return;
        }
        ifs.seekg(0, std::ios::end);
        auto const size = static_cast<std::size_t>(ifs.tellg());
        ifs.seekg(0, std::ios::beg);

        std::unique_ptr<char[]> buffer{new char[size]};
        ifs.read(buffer.get(), size);

        std::size_t i = 0;
        std::size_t last = i;
        for (;;) {
            if (i > size - 4) {
                break;
            }
            if (h264_utils::has_start_code(&buffer[i], 4)) {
                if (0 != i) {
                    auto const frame = std::string{&buffer[last], i - last};
                    if (frame.size() > 4) {
                        if (h264_utils::is_sps(frame[4])) {
                            sps_ = frame.substr(4, frame.size() - 4);
                        }
                        else if (h264_utils::is_pps(frame[4])) {
                            pps_ = frame.substr(4, frame.size() - 4);
                        } else {
                            frames_.emplace_back(frame);
                        }
                        last = i;
                    }
                }
                i += 4;
            } else {
                ++i;
            }
        }

        initialize_thread();
    }
public:
    std::string sps() const {
        return sps_;
    }
    std::string pps() const {
        return pps_;
    }
private:
    void thread_routine() {
        if (frames_.empty()) {
            return;
        }
        auto const size = frames_.size();
        auto i = std::vector<std::string>::size_type{0};
        for (;looping_;) {
            if (size == i) {
                i = 0;
            }
            pump_->produce(frames_[i++]);
            std::this_thread::sleep_for(std::chrono::milliseconds{38});
        }
    }
    bool initialize_thread() {
        thread_ = std::thread{&h264_producer::thread_routine, this};
        try {
            thread_.detach();
        } catch (std::exception const& e) {
            std::cerr << "thread detach failed: "
                      << e.what()
                      << std::endl;
            return false;
        }
        return true;
    }
private:
    std::shared_ptr<h264_pump> pump_;
    std::vector<std::string> frames_;
    std::string sps_;
    std::string pps_;
private:
    std::atomic_bool looping_{true};
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::atomic_bool finished_{false};
    std::condition_variable finish_cv_;
    std::mutex efinish_mutex_;
};


#endif // H264_PRODUCER_HXX

