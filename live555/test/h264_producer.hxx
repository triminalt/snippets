#ifndef H264_PRODUCER_HXX
#define H264_PRODUCER_HXX


#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include "./h264_pump.hxx"


class aac_producer {
public:
    aac_producer(h264_pump* pump) {
        std::ifstream ifs{"test.264", std::ios::binary};
        if (!ifs) {
            return;
        }
        ifs.seekg(0, std::ios::end);
        auto const size = static_cast<std::size_t>(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        
        std::unique_ptr<char[]> buffer{new char[size]};
        ifs.read(buffer.get(), size);

        std::size_t i = 0;
        std::size_t x = 0;
        for (;;) {
            if (i > size - 7) {
                break;
            }
            unsigned char byte0 = static_cast<unsigned char>(buffer[i]);
            unsigned char byte1 = static_cast<unsigned char>(buffer[i + 1]);
            auto const is_ff = 0xff == byte0;
			auto const is_f = 0xf0 == (0xf0 & byte1);
            if (!(is_ff && is_f)) {
                break;
            }
            std::uint16_t const len = ((buffer[i + 3] & 0x03) << 11)
                                    | (buffer[i + 4] << 3)
                                    | ((buffer[i + 5] & 0xE0) >> 5);
            if (++x > 2500) {
                break;
            }
            packets_.emplace_back(std::string{&buffer[i], len});
            i += len;
        }

        initialize_thread();
    }
    void thread_routine() {
        auto const size = packets_.size();
        auto i = std::vector<std::string>::size_type{0};
        for (;looping_;) {
            if (size == i) {
                i = 0;
            }
            pump_->produce(packets_[i++]);
            std::this_thread::sleep_for(std::chrono::milliseconds{20});
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
	h264_pump* pump_;
    std::vector<std::string> packets_;
};

#endif
