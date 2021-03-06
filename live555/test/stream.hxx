//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef STREAM_HXX
#define STREAM_HXX


#include <atomic>
#include <mutex>
#include <memory>
#include <string>
#include <thread>
#include <iostream>
#include <BasicUsageEnvironment.hh>
#include <RTSPServer.hh>
#include "./aac_pump.hxx"
#include "./aac_subsession.hxx"
#include "./h264_pump.hxx"
#include "./h264_subsession.hxx"

#define STREAM_TEST 1
#if !defined(_DEBUG) && !defined(DEBUG)
#   undef STREAM_TEST
#   define STREAM_TEST 0
#endif
#if STREAM_TEST
#   include "./aac_producer.hxx"
#   include "./h264_producer.hxx"
#endif


class stream final {
private:
    using ulock = std::unique_lock<std::mutex>;
public:
    stream(std::string const& path, std::uint16_t port)
        : env_(create_env()) {
        server_ = RTSPServer::createNew(*env_, port);
        if (nullptr == server_) {
            return;
        }
        session_ = ServerMediaSession::createNew(*env_, path.c_str());
        url_ = url(server_, session_);
        available_ = true;
    }
    ~stream() {
        end();
        Medium::close(server_);
        env_->reclaim();
    }
public:
    bool available() const {
        return available_;
    }
    std::string url() const {
        return url_;
    }
public:
    bool start( std::uint8_t aac_profile
              , std::uint8_t aac_sampling_frequency_index
              , std::uint8_t aac_channel_config
              , unsigned h264_width
              , unsigned h264_height
              , unsigned h264_bitrate
#if STREAM_TEST
              , unsigned h264_fps) {
#else
              , unsigned h264_fps
              , std::string const& h264_sps
              , std::string const& h264_pps) {
#endif
        if (working_) {
            end();
        }
        working_ = true;
        aac_pump_.reset(new aac_pump{ aac_utils::sampling_frequency
                                    ( aac_sampling_frequency_index)
                                    , buffer_ms});
        h264_pump_.reset(new h264_pump{h264_fps, buffer_ms});
#if STREAM_TEST
        aac_producer_.reset(new aac_producer(aac_pump_));
        h264_producer_.reset(new h264_producer(h264_pump_));
        auto const h264_sps = h264_producer_->sps();
        auto const h264_pps = h264_producer_->pps();
#endif
        auto const aac_ss = new aac_subsession( *env_
                                              , true
                                              , aac_pump_
                                              , aac_profile
                                              , aac_sampling_frequency_index
                                              , aac_channel_config);
        if (!session_->addSubsession(aac_ss)) {
            return false;
        }
        auto const h264_ss = new h264_subsession( *env_
                                                , true
                                                , h264_pump_
                                                , h264_fps
                                                , h264_sps
                                                , h264_pps);
        if (!session_->addSubsession(h264_ss)) {
            return false;
        }
        server_->addServerMediaSession(session_);
        return loop();
    }
    bool end() {
        if (!working_) {
            return false;
        }
        working_ = false;
        event_looping_ = 1;
        ulock lock(finish_mutex_);
        auto const wait_predicate = [this]() -> bool {
            return finished_;
        };
        finish_cv_.wait(lock, wait_predicate);
        server_->deleteServerMediaSession(session_);
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
    static std::string url(RTSPServer* server, ServerMediaSession* session) {
        std::unique_ptr<char[]> url{server->rtspURL(session)};
        return std::string{url.get()};
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
            std::cerr << "thread detach failed: "
                      << e.what()
                      << std::endl;
            return false;
        }
        return true;
    }
private:
    static auto const buffer_ms = 500u;
private:
    bool available_;
private:
    std::atomic_bool working_{false};
private:
    BasicUsageEnvironment* env_;
    ServerMediaSession* session_;
    RTSPServer* server_;
    std::string url_;
private:
    std::shared_ptr<aac_pump> aac_pump_;
    std::shared_ptr<h264_pump> h264_pump_;
#if STREAM_TEST
    std::shared_ptr<aac_producer> aac_producer_;
    std::shared_ptr<h264_producer> h264_producer_;
#endif
private:
    char volatile event_looping_ = 0;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::atomic_bool finished_{false};
    std::condition_variable finish_cv_;
    std::mutex finish_mutex_;
};


#endif // STREAM_HXX
