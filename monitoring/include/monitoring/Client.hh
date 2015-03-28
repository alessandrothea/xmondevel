#ifndef __MON_CLIENT_HH__
#define __MON_CLIENT_HH__

#include "xdaq/Application.h"
#include "toolbox/task/TimerListener.h"
#include "xgi/framework/UIManager.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"


namespace xdaq {
class ApplicationStub;
}

class SimpleClient : 
    public xdaq::Application,
    public xgi::framework::UIManager,
    public toolbox::task::TimerListener {
        public:
          
        XDAQ_INSTANTIATOR();

        SimpleClient( xdaq::ApplicationStub* );
        virtual ~SimpleClient();

        // This method is used to auto-expire the hardware lease after a
        // given time.
        void timeExpired(toolbox::task::TimerEvent& event);
  
        void Default(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);
        void getConsoleData(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);
private:
  int counter_;
};

#endif