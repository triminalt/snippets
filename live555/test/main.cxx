
#include <memory>
#include <ios>
#include <fstream>
#include "./aac_pump.hxx"
#include "./aac_producer.hxx"
#include "./aac_server.hxx"

#include "./h264_pump.hxx"
#include "./h264_producer.hxx"
#include "./h264_server.hxx"

#include "./server.hxx"

#include "./on_demand_server.hxx"


static aac_pump aac_pmp{44100, 1000};
static h264_pump h264_pmp{25, 1000};

class aac_consumer {
public:
    aac_consumer() {
        initialize_thread();
    }
    void thread_routine() {
        for (;looping_;) {
            std::string packet;
            aac_pmp.consume(packet);
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
#if 0
    aac_producer aac_prd(&aac_pmp);
    aac_server aac_srv(&aac_pmp);
    aac_srv.start(1, 4, 2);
#elif 0
    h264_producer h264_prd(&h264_pmp);
    h264_server h264_srv(&h264_pmp);
    h264_srv.start(25, h264_prd.sps(), h264_prd.pps());
#elif 1
    aac_producer aac_prd(&aac_pmp);
    h264_producer h264_prd(&h264_pmp);
    server srv(&aac_pmp, &h264_pmp);
    srv.start(1, 4, 2, 25, h264_prd.sps(), h264_prd.pps());
#else
    run_on_demand_server();
#endif
    std::this_thread::sleep_for(std::chrono::hours{1});
    return 0;
}