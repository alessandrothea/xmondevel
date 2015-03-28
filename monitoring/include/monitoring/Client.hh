#ifndef __MON_CLIENT_HH__
#define __MON_CLIENT_HH__

#include "xdaq/Application.h"
#include "toolbox/task/TimerListener.h"
#include "xgi/framework/UIManager.h"

namespace xdaq {
class ApplicationStub;
}

class SimpleClient : 
    public toolbox::task::TimerListener,
    public xdaq::Application,
    public xgi::framework::UIManager {
        public:
        SimpleClient( xdaq::ApplicationStub* );
        virtual ~SimpleClient();

        // This method is used to auto-expire the hardware lease after a
        // given time.
        void timeExpired(toolbox::task::TimerEvent& event);
  
        void Default(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);
};

#endif