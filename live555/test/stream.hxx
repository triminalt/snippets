//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef STREAM_HXX
#define STREAM_HXX


#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <iostream>
#include <BasicUsageEnvironment.hh>
#include <RTSPServer.hh>
#include "./aac_pump.hxx"
#include "./aac_subsession.hxx"
#include "./h264_pump.hxx"
#include "./h264_subsession.hxx"

class stream final {
private:
    using ulock = std::unique_lock<std::mutex>;
public:
    stream(aac_pump* aac_pmp, h264_pump* h264_pmp)
        : env_(create_env())
        , aac_pump_(aac_pmp)
        , h264_pump_(h264_pmp) {
    }

    ~stream() {
        env_->reclaim();
    }
public:
    bool start( std::uint8_t aac_profile
              , std::uint8_t aac_sample_freq_idx
              , std::uint8_t aac_channel_cfg
              , unsigned h264_fps
              , std::string h264_sps
              , std::string h264_pps) {
        auto const instance = RTSPServer::createNew(*env_, 8554, nullptr);
        if (nullptr == instance) {
            return false;
        }
        auto sms = ServerMediaSession::createNew( *env_
                                                , "mirror"
                                                , "mirror"
                                                , "mirror stream");
        sms->addSubsession(new aac_subsession( *env_
                                             , true
                                             , aac_pump_
                                             , aac_profile
                                             , aac_sample_freq_idx
                                             , aac_channel_cfg));
        sms->addSubsession(new h264_subsession( *env_
                                              , true
                                              , h264_pump_
                                              , h264_fps
                                              , h264_sps
                                              , h264_pps));
        instance->addServerMediaSession(sms);
        announce(instance, sms);
        return loop();
    }
    bool end() {
        event_looping_ = 1;
        ulock lock(finish_mutex_);
        auto const wait_predicate = [this]() -> bool {
            return finished_;
        };
        finish_cv_.wait(lock, wait_predicate);
        return true;
    }
public:
    bool push_aac(std::string const& packet) {
        return h264_pump_->produce(packet);
    }
    bool push_h264(std::string const& frame) {
        return h264_pump_->produce(frame);
    }
private:
    static inline BasicUsageEnvironment* create_env() {
        auto const scheduler = BasicTaskScheduler::createNew();
        return BasicUsageEnvironment::createNew(*scheduler);
    }
private:
    void thread_routine() {
        env_->taskScheduler().doEventLoop(&event_looping_);
        finished_ = true;
        std::notify_all_at_thread_exit(finish_cv_, ulock{finish_mutex_});
    }
    bool loop() {
        thread_ = std::thread{&stream::thread_routine, this};
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
    static void announce(RTSPServer* server, ServerMediaSession* sms) {
        std::unique_ptr<char[]> url{server->rtspURL(sms)};
        std::clog << "stream url: " << url.get() << std::endl;
    }
private:
    char volatile event_looping_ = 0;
private:
    BasicUsageEnvironment* env_;
private:
    aac_pump* aac_pump_;
    h264_pump* h264_pump_;
private:
    std::atomic_bool running_{true};
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::atomic_bool finished_{false};
    std::condition_variable finish_cv_;
    std::mutex finish_mutex_;
};


#endif // STREAM_HXX
