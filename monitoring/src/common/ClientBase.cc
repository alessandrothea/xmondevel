#include "monitoring/ClientBase.hh"

#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"

#include "log4cplus/loggingmacros.h"

namespace monitoring {

ClientBase::ClientBase( xdaq::ApplicationStub *stub ) :
  xdaq::Application(stub),
  lock_ (toolbox::BSem::FULL) {

}

void ClientBase::timeExpired(toolbox::task::TimerEvent& event) {
  LOG4CPLUS_DEBUG(getApplicationLogger(), "timeExpired, event: "+event.type());
}


void ClientBase::actionPerformed(xdata::Event& event) {
  LOG4CPLUS_DEBUG(getApplicationLogger(), "actionPerformed, event: "+event.type());

    try {
        
        if (event.type() == "urn:xdaq-event:setDefaultValues") {
            LOG4CPLUS_DEBUG(getApplicationLogger(),"Default values loaded");
            // fire item available
            updateTimer_ = toolbox::task::getTimerFactory()->createTimer("SlashUpdateTimer");
            toolbox::TimeInterval interval(2,0); // period of 8 secs 
            toolbox::TimeVal start;
            start = toolbox::TimeVal::gettimeofday();
            updateTimer_->scheduleAtFixedRate( start, this, interval,  0, "" );  
            
        }
    }
    catch (xcept::Exception &e) 
    {
//        XCEPT_DECLARE_NESTED(ecal::mon::exception::ClientException, ex, "failure in setting default values" ,e);
//        client_->notifyQualified("error",e);
        LOG4CPLUS_ERROR(this->getApplicationLogger(),"Failure in setting default values");

    }
}
}