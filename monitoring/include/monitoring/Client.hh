#ifndef __MON_CLIENT_HH__
#define __MON_CLIENT_HH__

#include "xdaq/Application.h"
#include "toolbox/task/TimerListener.h"
#include "xgi/framework/UIManager.h"
#include "hyperdaq/framework/Layout.h"





namespace xdaq {
class ApplicationStub;
}


class ALayout: public hyperdaq::framework::Layout
{
    public:
      ALayout();
      virtual ~ALayout();

      // NOTE: This method needs the throw specifier, since the parent
      // class already has it.
      virtual void getHTMLHeader(xgi::framework::UIManager* manager,
                                 xgi::Input* in,
                                 xgi::Output* out) throw (xgi::exception::Exception);
};

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
        void updateData(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);

private:
  
  ALayout layout_;
  
  int counter_;
};

#endif