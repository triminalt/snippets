#ifndef H264_SUBSESSION_HXX
#define H264_SUBSESSION_HXX


#include <string>
#include <OnDemandServerMediaSubsession.hh>
#include <H264VideoStreamFramer.hh>
#include <H264VideoRTPSink.hh>
#include "./h264_pump.hxx"
#include "./h264_source.hxx"
#include "./h264_sink.hxx"


class h264_subsession final: public OnDemandServerMediaSubsession {
public:
    h264_subsession( UsageEnvironment& env
                   , Boolean reused
                   , h264_pump* pump
                   , unsigned fps
                   , std::string sps
                   , std::string pps)
        : OnDemandServerMediaSubsession(env, reused)
        , pump_(pump)
        , fps_(fps)
        , sps_(sps)
        , pps_(pps) {
        // EMPTY
    }
    virtual ~h264_subsession() = default;
protected:
    virtual FramedSource* createNewStreamSource( unsigned
                                               , unsigned& bitrate) override {
        bitrate = 1024;
        return H264VideoStreamFramer::createNew( envir()
                                               , new h264_source( envir()
                                                                , pump_
                                                                , fps_)
                                               , true);
    }
    virtual RTPSink* createNewRTPSink( Groupsock* rtp
                                     , unsigned char rtp_payload_type_if_dynamic
                                     , FramedSource*) override {
        return new h264_sink( envir()
                            , rtp
                            , rtp_payload_type_if_dynamic
                            , sps_
                            , pps_);
    }
private:
    h264_pump* pump_;
    unsigned fps_;
    std::string sps_;
    std::string pps_;
};


#endif // H264_SUBSESSION_HXX
