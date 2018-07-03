//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-03
//


#ifndef AAC_SUBSESSION_HXX
#define AAC_SUBSESSION_HXX


#include <cstdint>
#include <OnDemandServerMediaSubsession.hh>
#include "./aac_pump.hxx"
#include "./aac_source.hxx"
#include "./aac_sink.hxx"

class aac_subsession final : public OnDemandServerMediaSubsession {
public:
    aac_subsession( UsageEnvironment& env
                  , Boolean reused
                  , aac_pump* pump
                  , std::uint8_t profile
                  , std::uint8_t sampling_freq_idx
                  , std::uint8_t channel_cfg)
        : OnDemandServerMediaSubsession(env, reused)
        , pump_(pump)
        , profile_(profile)
        , sampling_frequency_index_(sampling_freq_idx)
        , channel_config_(channel_cfg)  {
        // EMPTY
    }
protected:
    virtual FramedSource* createNewStreamSource( unsigned // clientSessionId
                                               , unsigned& estBitrate) override {
      estBitrate = 128; // kbps, estimate
      return new aac_source( envir()
                           , pump_
                           , profile_
                           , sampling_frequency_index_
                           , channel_config_);
    }
    virtual RTPSink* createNewRTPSink( Groupsock* rtp
                                     , unsigned char rtp_payload_type_if_dynamic
                                     , FramedSource* src) override {
        aac_source* aac_src = reinterpret_cast<aac_source*>(src);
        return new aac_sink( envir()
                           , rtp
                           , rtp_payload_type_if_dynamic
                           , aac_src->sampling_frequency()
                           , aac_src->config()
                           , aac_src->channels());
    }
private:
    aac_pump* pump_;
    std::uint8_t profile_;
    std::uint8_t sampling_frequency_index_;
    std::uint8_t channel_config_;
};


#endif // AAC_SUBSESSION_HXX
