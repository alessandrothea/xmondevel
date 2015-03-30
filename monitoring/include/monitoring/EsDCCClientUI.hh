#ifndef __MONITORING_ESDCCCLIENTU_HH__
#define __MONITORING_ESDCCCLIENTU_HH__

#include "monitoring/ClientUIBase.hh"

namespace monitoring {

class EsDCCClient;

class EsDCCClientUI : public ClientUIBase
{
public:
    EsDCCClientUI( EsDCCClient* client );
    ~EsDCCClientUI();
    
    void Default(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);
    
    void getUpdatedValues(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);

};
    
}


#endif