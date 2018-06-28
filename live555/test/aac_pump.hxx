
#ifndef AAC_PUMP_HXX
#define AAC_PUMP_HXX

class aac_pump final {
public:
    aac_pump() {
    }
    
private:
  unsigned sample_frq_;
  unsigned channel_;
  unsigned profile_;
};

#endif // AAC_PUMP_HXX
