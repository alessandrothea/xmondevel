#include "monitoring/Client.hh"

#include "xdaq/ApplicationStub.h"

#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"


#include "xgi/Method.h"
#include "xgi/framework/Method.h"

XDAQ_INSTANTIATOR_IMPL(SimpleClient)

SimpleClient::SimpleClient( xdaq::ApplicationStub *stub ) :
  xdaq::Application(stub),
  xgi::framework::UIManager(this), counter_(0x0) {
  
  xgi::framework::deferredbind(this, this,  &SimpleClient::Default, "Default");
  //xgi::framework::deferredbind(this, this,  &SimpleClient::getConsoleData, "getConsoleData");
   xgi::bind(this, &SimpleClient::getConsoleData, "getConsoleData");
  
  toolbox::task::Timer * timer =
  toolbox::task::getTimerFactory()->createTimer("PeriodicTime");
  toolbox::TimeInterval interval(5,0); // period of 8 secs 
  toolbox::TimeVal start;
  start = toolbox::TimeVal::gettimeofday();
  timer->scheduleAtFixedRate( start, this, interval,  0, "" );	

}


SimpleClient::~SimpleClient() {
}

void
SimpleClient::timeExpired(toolbox::task::TimerEvent& event) {
  LOG4CPLUS_INFO (this->getApplicationLogger(), "now!" );
  ++counter_;
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
  
  std::stringstream callback;
    callback << this->getApplicationDescriptor()->getContextDescriptor()->getURL();
    callback << "/" << this->getApplicationDescriptor()->getURN() << "/getConsoleData";

    *out << "<div id=\"my-console\" class=\"xdaq-console\" data-callback=\"" << callback.str() << "\" autosize></div>" << std::endl;

}

void SimpleClient::getConsoleData(xgi::Input*, xgi::Output* out) throw (xgi::exception::Exception) {
  out->getHTTPResponseHeader().addHeader("Content-Type", "text/plain");
  *out << "Counter : " << counter_;
}
