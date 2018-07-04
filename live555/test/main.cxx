//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#include <memory>
#include <ios>
#include <iostream>
#include <fstream>
#include "./aac_pump.hxx"
#include "./aac_producer.hxx"
#include "./aac_stream.hxx"

#include "./h264_pump.hxx"
#include "./h264_producer.hxx"
#include "./h264_stream.hxx"

#include "./stream.hxx"

#include "./on_demand_server.hxx"


static auto const aac_pmp = std::shared_ptr<aac_pump>{new aac_pump{44100, 1000}};
static auto const h264_pmp = std::shared_ptr<h264_pump>{new h264_pump{25, 1000}};

class aac_consumer {
public:
    aac_consumer() {
        initialize_thread();
    }
    void thread_routine() {
        for (;looping_;) {
            std::string packet;
            aac_pmp->consume(packet);
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
    aac_producer aac_prd(aac_pmp);
    aac_stream aac_s(aac_pmp);
    aac_s.start(1, 4, 2);
#elif 0
    h264_producer h264_prd(h264_pmp);
    h264_stream h264_s(h264_pmp);
    h264_s.start(25, h264_prd.sps(), h264_prd.pps());
#elif 1
#if 0
    aac_producer aac_prd(aac_pmp);
    h264_producer h264_prd(h264_pmp);
#endif
    stream s("mirror", 8854);
    s.start( 1
           , 4
           , 2
           , 1080
           , 720
           , 2000
           , 25);
    std::clog << "\n\nURL   "  << s.url() << std::endl;

    std::this_thread::sleep_for(std::chrono::minutes{1});
    s.end();
#else
    run_on_demand_server();
#endif
    std::this_thread::sleep_for(std::chrono::minutes{1});
    return 0;
}