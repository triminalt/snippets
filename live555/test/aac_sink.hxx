#ifndef AAC_SINK
#define AAC_SINK


#include <cstdint>
#include <MPEG4GenericRTPSink.hh>


class aac_sink final : public MPEG4GenericRTPSink {
public:
    aac_sink( UsageEnvironment& env
            , Groupsock* rtp
            , std::uint8_t rtp_payload_fmt
            , std::uint32_t sampling_frequency
            , char const* config
            , unsigned channels)
        : MPEG4GenericRTPSink( env
                             , rtp
                             , rtp_payload_fmt
                             , sampling_frequency
                             , "audio"
                             , "AAC-hbr"
                             , config
                             , channels){
        // EMPTY
    }
    ~aac_sink() = default;
};


#endif
