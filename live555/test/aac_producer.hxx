//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef AAC_PRODUCER_HXX
#define AAC_PRODUCER_HXX


#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include "./aac_utils.hxx"
#include "./aac_pump.hxx"


class aac_producer {
public:
    aac_producer(aac_pump* pump)
        : pump_(pump) {
        std::ifstream ifs{"test.aac", std::ios::binary};
        if (!ifs) {
            return;
        }
        ifs.seekg(0, std::ios::end);
        auto const size = static_cast<std::size_t>(ifs.tellg());
        ifs.seekg(0, std::ios::beg);

        std::unique_ptr<char[]> buffer{new char[size]};
        ifs.read(buffer.get(), size);

        std::size_t i = 0;
        for (;;) {
            if (i > size - 7) {
                break;
            }
            if (!aac_utils::has_syncword(&buffer[i], 2)) {
                break;
            }
            auto const len = aac_utils::packet_len(&buffer[i + 3], 3);
            packets_.emplace_back(std::string{&buffer[i], len});
            i += len;
        }

        initialize_thread();
    }
    void thread_routine() {
        if (packets_.empty()) {
            return;
        }
        auto const size = packets_.size();
        auto i = std::vector<std::string>::size_type{0};
        for (;looping_;) {
            if (size == i) {
                i = 0;
            }
            pump_->produce(packets_[i++]);
            std::this_thread::sleep_for(std::chrono::milliseconds{21});
        }
    }
    bool initialize_thread() {
        thread_ = std::thread{&aac_producer::thread_routine, this};
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
    std::atomic_bool looping_{true};
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::atomic_bool finished_{false};
    std::condition_variable finish_cv_;
    std::mutex efinish_mutex_;
private:
    aac_pump* pump_;
    std::vector<std::string> packets_;
};


#endif // AAC_PRODUCER_HXX
