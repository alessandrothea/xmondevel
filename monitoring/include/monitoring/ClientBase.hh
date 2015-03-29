#ifndef __MONITORING_CLIENTBASE_HH__
#define __MONITORING_CLIENTBASE_HH__

#include "xdaq/Application.h"
#include "toolbox/task/TimerListener.h"
#include "toolbox/BSem.h"
#include "xdata/String.h"
#include "xdata/Table.h"

#include "monitoring/Method.hh"

#include <map>
#include <vector>

namespace toolbox {
namespace task {
class Timer;
}
}

namespace monitoring {

class ClientUIBase;

class ClientBase :
public xdaq::Application,
public xdata::ActionListener,
public toolbox::task::TimerListener {
public:

  XDAQ_INSTANTIATOR();

  ClientBase(xdaq::ApplicationStub*);
  virtual ~ClientBase();

  // This method is used to auto-expire the hardware lease after a
  // given time.
  virtual void timeExpired(toolbox::task::TimerEvent& event);

  virtual void actionPerformed(xdata::Event& event); 

  template<class LISTENER>
  void addFlashListCallback(const std::string& fl, LISTENER * obj, void (LISTENER::* func)( xdata::Table::Reference& ) ) {
    monitoring::Method<LISTENER>* f = new monitoring::Method<LISTENER>();
    f->obj_ = obj;
    f->func_ = func;

    callbacks_[fl].push_back(f);
  }

protected:

  void processReport( const std::string& fl, xdata::Table::Reference& table );

  void registerGui( ClientUIBase* gui ) { gui_ = gui; }
private:

  typedef std::map< std::string, std::vector<monitoring::MethodSignature*> > MethodMap;

  ClientUIBase* gui_;

  xdata::String   updatePeriod_;
  xdata::String   slashURL_;
  xdata::String   flashlists_;

  toolbox::BSem lock_;
  toolbox::task::Timer * updateTimer_;

  MethodMap callbacks_;


};

}

#endif