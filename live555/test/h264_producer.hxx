#ifndef H264_PRODUCER_HXX
#define H264_PRODUCER_HXX


#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include "./h264_pump.hxx"


class h264_producer {
public:
    h264_producer(h264_pump* pump)
		: pump_(pump) {
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
		std::size_t last = i;
        for (;;) {
            if (i > size - 4) {
                break;
            }
            unsigned char byte0 = static_cast<unsigned char>(buffer[i]);
            unsigned char byte1 = static_cast<unsigned char>(buffer[i + 1]);
			unsigned char byte2 = static_cast<unsigned char>(buffer[i + 2]);
			unsigned char byte3 = static_cast<unsigned char>(buffer[i + 3]);
			auto const is_0001 = 0x00 == byte0 && 0x00 == byte1 && 0x00 == byte2 && 0x01 == byte3;
            if (is_0001) {
				if (0 != i) {
					frames_.emplace_back(std::string{&buffer[last], i - last});
					last = i;
				}
				i += 4;
			} else {
				++i;
			}
        }
        initialize_thread();
    }
    void thread_routine() {
        auto const size = frames_.size();
        auto i = std::vector<std::string>::size_type{0};
        for (;looping_;) {
            if (size == i) {
                i = 0;
            }
            pump_->produce(frames_[i++]);
            std::this_thread::sleep_for(std::chrono::milliseconds{20});
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
    std::atomic_bool looping_{true};
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::atomic_bool finished_{false};
    std::condition_variable finish_cv_;
    std::mutex efinish_mutex_;   
private:
	h264_pump* pump_;
    std::vector<std::string> frames_;
};


#endif
