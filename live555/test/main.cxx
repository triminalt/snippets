
#include <memory>
#include <ios>
#include <fstream>
#include "./aac_pump.hxx"
#include "./aac_server.hxx"


static aac_pump pump{44100, 1000};




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