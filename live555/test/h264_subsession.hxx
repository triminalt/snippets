#ifndef H264_SUBSESSION_HXX
#define H264_SUBSESSION_HXX


#include <string>
#include <OnDemandServerMediaSubsession.hh>
#include <H264VideoRTPSink.hh>
#include "./h264_pump.hxx"
#include "./h264_source.hxx"

class h264_subsession final: public OnDemandServerMediaSubsession {
public:
	static h264_subsession* createNew( UsageEnvironment& env
									 , Boolean reused
									 , h264_pump* pump
									 , unsigned fps) {
		return new h264_subsession(env, reused, pump, fps);
	}
private:
	h264_subsession(UsageEnvironment& env
				   , Boolean reused
				   , h264_pump* pump
				   , unsigned fps)
		: OnDemandServerMediaSubsession(env, reused)
        , pump_(pump)
	    , fps_(fps) {
		// EMPTY
	}
	virtual ~h264_subsession() = default;
protected:
	virtual FramedSource* createNewStreamSource( unsigned
											   , unsigned& bitrate) override {
		bitrate = 1024;
		return h264_source::createNew(envir(), pump_, fps_);
	}
	virtual RTPSink* createNewRTPSink( Groupsock* rtp
									 , unsigned char rtp_payload_type_if_dynamic
									 , FramedSource*) override {
		auto const sps = reinterpret_cast<u_int8_t const*>(sps_.c_str());
		auto const pps = reinterpret_cast<u_int8_t const*>(pps_.c_str());
		return H264VideoRTPSink::createNew( envir()
										  , rtp
										  , rtp_payload_type_if_dynamic
										  , sps
										  , sps_.size()
										  , pps
										  , pps_.size());
	}

private:
    h264_pump* pump_;
	unsigned fps_;
	std::string sps_;
	std::string pps_;
};

#endif // H264_SUBSESSION_HXX
