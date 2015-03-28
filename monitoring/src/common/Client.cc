#include "monitoring/Client.hh"

#include "xdaq/ApplicationStub.h"

#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"


#include "xgi/Method.h"
#include "xgi/framework/Method.h"

XDAQ_INSTANTIATOR_IMPL(SimpleClient)

SimpleClient::SimpleClient( xdaq::ApplicationStub *stub ) : xdaq::Application(stub), xgi::framework::UIManager(this) {
  
  xgi::framework::deferredbind(this, this,  &SimpleClient::Default, "Default");
  
  toolbox::task::Timer * timer =
  toolbox::task::getTimerFactory()->createTimer("PeriodicDiagnostic");
  toolbox::TimeInterval interval(8,0); // period of 8 secs 
  toolbox::TimeVal start;
  start = toolbox::TimeVal::gettimeofday();
  timer->scheduleAtFixedRate( start, this, interval,  0, "" );	

}


SimpleClient::~SimpleClient() {
}

void
SimpleClient::timeExpired(toolbox::task::TimerEvent& event) {
  LOG4CPLUS_INFO (this->getApplicationLogger(), "now!" );
}

void
SimpleClient::Default(xgi::Input* in, xgi::Output* out) throw (xgi::exception::Exception) {
  *out << 
      "<table class=\"xdaq-table\">"
      "  <caption>My XDAQ Table</caption>"
      "    <thead>"
      "      <tr>"
      "        <th>Artist</th>"
      "        <th>Album</th>"
      "      </tr>"
      "    </thead>"
      "    <tbody>"
      "      <tr>"
      "        <td>Nirvana</td>"
      "        <td>Nevermind</td>"
      "      </tr>"
      "      <tr>"
      "        <td>Nirvana</td>"
      "        <td>Nevermind</td>"
      "      </tr>"
      "</tbody>"
      "</table>";
}

