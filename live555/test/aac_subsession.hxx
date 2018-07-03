
#ifndef AAC_SUBSESSION_HXX
#define AAC_SUBSESSION_HXX


#include <OnDemandServerMediaSubsession.hh>
#include <PassiveServerMediaSubsession.hh>
#include <MPEG4GenericRTPSink.hh>
#include "./aac_source.hxx"

class aac_subsession final : public OnDemandServerMediaSubsession {
public:
    aac_subsession( UsageEnvironment& env
                  , Boolean reused
                  , aac_pump* pump
                  , unsigned profile
		          , unsigned sampling_freq_idx
                  , unsigned channel_cfg)
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
    unsigned profile_;
    unsigned sampling_frequency_index_;
    unsigned channel_config_;
};

#endif // AAC_SUBSESSION_XX