//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef H264_SINK
#define H264_SINK


#include <string>
#include <H264VideoRTPSink.hh>


class h264_sink final : public H264VideoRTPSink {
public:
    h264_sink( UsageEnvironment& env
             , Groupsock* rtp
             , unsigned char rtp_payload_fmt
             , std::string sps
             , std::string pps)
        : H264VideoRTPSink( env
                          , rtp
                          , rtp_payload_fmt
                          , reinterpret_cast<u_int8_t const*>(sps.c_str())
                          , sps.size()
                          , reinterpret_cast<u_int8_t const*>(pps.c_str())
                          , pps.size()){
        // EMPTY
    }
    ~h264_sink() = default;
};


#endif // H264_SINK
