#ifndef AAC_SERVER_HXX
#define AAC_SERVER_HXX


#include <atomic>
#include <mutex>
#include <thread>
#include <iostream>
#include <BasicUsageEnvironment.hh>
#include <RTSPServer.hh>
#include "./aac_pump.hxx"
#include "./aac_subsession.hxx"


class aac_server {
public:
    aac_server(aac_pump* pump)
        : env_(create_env())
        , pump_(pump) {
    }

    ~aac_server() {
        env_->reclaim();
    }
public:
    bool start( std::uint8_t profile
              , std::uint8_t sample_freq_idx
              , std::uint8_t channel_cfg) {
        auto const instance = RTSPServer::createNew(*env_, 8554, nullptr);
        if (nullptr == instance) {
            return false;
        }
        auto sms = ServerMediaSession::createNew(*env_, "aac", "aac", "aac stream");
        sms->addSubsession(new aac_subsession(*env_, true, pump_, profile, sample_freq_idx, channel_cfg));
        instance->addServerMediaSession(sms);
        announce(instance, sms, "aac");
        return loop();
    }

    bool end() {
        event_looping_ = 1;
        std::unique_lock<std::mutex> lock(finish_mutex_);
        auto const wait_predicate = [this]() -> bool {
            return finished_;
        };
        finish_cv_.wait(lock, wait_predicate);
        return true;
    }
public:
    bool push(std::string const& packet) {
        return pump_->produce(packet);
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
        std::notify_all_at_thread_exit( finish_cv_
                                      , std::unique_lock<std::mutex>{finish_mutex_});
    }

    bool loop() {
        thread_ = std::thread{&aac_server::thread_routine, this};
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
    static void announce(RTSPServer* server, ServerMediaSession* sms, char const* stream) {
        char* url = server->rtspURL(sms);
        UsageEnvironment& env = server->envir();
        env << "\n\"" << stream << "\" stream, from the file \n";
        env << "Play this stream using the URL  " << url << "  \n";
        delete[] url;
    }
private:
    char volatile event_looping_ = 0;
private:
    BasicUsageEnvironment* env_;
private:
    aac_pump* pump_;
private:
    std::atomic_bool running_{true};
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::atomic_bool finished_{false};
    std::condition_variable finish_cv_;
    std::mutex finish_mutex_;
};


#endif
