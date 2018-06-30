
#ifndef AAC_SUBSESSION_HXX
#define AAC_SUBSESSION_HXX


#include <OnDemandServerMediaSubsession.hh>
#include <MPEG4GenericRTPSink.hh>
#include "./aac_source.hxx"

class aac_subsession final : public OnDemandServerMediaSubsession {
public:
    static aac_subsession* createNew( UsageEnvironment& env
                                    , Boolean reused
                                    , aac_pump* pump
                                    , std::uint8_t profile
		                            , std::uint8_t sampling_freq_idx
                                    , std::uint8_t channel_cfg) {
        return new aac_subsession{ env
                                 , pump
                                 , reused
                                 , profile
                                 , sampling_freq_idx
                                 , channel_cfg};
    }
private:
    aac_subsession( UsageEnvironment& env
                  , aac_pump* pump
                  , Boolean reused
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
    virtual FramedSource* createNewStreamSource( unsigned clientSessionId
					                           , unsigned& estBitrate) override {
      estBitrate = 96; // kbps, estimate
      return aac_source::createNew( envir()
                                  , pump_
                                  , profile_
                                  , sampling_frequency_index_
                                  , channel_config_);
    }
    virtual RTPSink* createNewRTPSink( Groupsock* rtp
				                     , unsigned char rtpPayloadTypeIfDynamic
				                     , FramedSource* src) override {
        aac_source* aac_src = reinterpret_cast<aac_source*>(src);
        return MPEG4GenericRTPSink::createNew( envir()
                                             , rtp
					                         , rtpPayloadTypeIfDynamic
					                         , aac_src->sampling_frequency()
					                         , "audio"
                                             , "AAC-hbr"
                                             , aac_src->config_str()
					                         , aac_src->channels());
    }
private:
    aac_pump* pump_;
    std::uint8_t profile_;
    std::uint8_t sampling_frequency_index_;
    std::uint8_t channel_config_;
};

#endif // AAC_SUBSESSION_XX