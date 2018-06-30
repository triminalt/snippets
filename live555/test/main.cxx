
#include <memory>
#include <ios>
#include <fstream>
#include "./aac_pump.hxx"
#include "./aac_server.hxx"


static aac_pump pump{44100, 1000};


class aac_producer {
public:
    aac_producer() {
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
        std::size_t x = 0;
        for (;;) {
            if (i > size - 7) {
                break;
            }
            unsigned char byte0 = static_cast<unsigned>(buffer[i]);
            unsigned char byte1 = static_cast<unsigned>(buffer[i + 1]);
            auto const is_ff = unsigned{0xff} == byte0;
            auto const is_f = unsigned{0xf0} == (unsigned char{0xf0} & byte1);
            if (!(is_ff && is_f)) {
                break;
            }
            u_int16_t len = ((buffer[i + 3] & 0x03) << 11)
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
            pump.produce(packets_[i++]);
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
    std::vector<std::string> packets_;
};

class aac_consumer {
public:
    aac_consumer() {
        initialize_thread();
    }
    void thread_routine() {
        for (;looping_;) {
            std::string packet;
            pump.consume(packet);
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
    }
    bool initialize_thread() {
        thread_ = std::thread{&aac_consumer::thread_routine, this};
        try {
            thread_.detach();
        } catch (std::exception const& e) {
            std::cerr << "event thread detach failed: "
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
};

int main() {
    aac_producer prd;
#if 0
    aac_consumer csm;
#else
    aac_server srv(&pump);
    srv.start(1, 4, 2);
#endif
    std::this_thread::sleep_for(std::chrono::hours{1});
    return 0;
}