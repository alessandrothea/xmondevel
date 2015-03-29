#ifndef __MONITORING_CLIENTBASE_HH__
#define __MONITORING_CLIENTBASE_HH__

#include "xdaq/Application.h"
#include "toolbox/task/TimerListener.h"
#include "toolbox/BSem.h"

namespace toolbox {
namespace task {
class Timer;
}
}

namespace monitoring {

class ClientBase :
public xdaq::Application,
public xdata::ActionListener,
public toolbox::task::TimerListener {
public:
  ClientBase(xdaq::ApplicationStub*);
  ~ClientBase();

  // This method is used to auto-expire the hardware lease after a
  // given time.
  virtual void timeExpired(toolbox::task::TimerEvent& event);

  virtual void actionPerformed(xdata::Event& event);  
  
private:


  toolbox::BSem lock_;
  toolbox::task::Timer * updateTimer_;
};

}

#endif