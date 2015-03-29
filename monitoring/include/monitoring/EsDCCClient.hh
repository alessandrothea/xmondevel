#ifndef __MONITORING_ESDCCCLIENT_HH__
#define __MONITORING_ESDCCCLIENT_HH__

#include "monitoring/ClientBase.hh"

// XDAQ Headers
#include "xdata/TimeVal.h"

namespace monitoring {

struct EsFedStatus {
    //global status of the DCCs
    unsigned short dccStatus;
    std::set<std::string> dccErrorsPresent;
    
    //timestamp for reports received
    xdata::TimeVal reportTime;
    
    // cDAQ backpressure data
    bool backPressure;
    
    std::string errorTT;
    
    std::string lastReport;
    std::string lastInfo;
    
    long int bx;
    long int l1a;
    
    bool daqOk;
};

class EsDCCClient : public ClientBase {
public:

  XDAQ_INSTANTIATOR();

  EsDCCClient(xdaq::ApplicationStub*);

  ~EsDCCClient();

  void updateGlobalStatus( xdata::Table::Reference& table ) throw (xcept::Exception);
  void checkDiskInfo( xdata::Table::Reference& table ) throw (xcept::Exception);

private:
  std::map<unsigned int, EsFedStatus> feds_;
  toolbox::BSem fedsLock_;
  friend class EsDCCClientUI;


};

}

#endif
