#include "monitoring/Client.hh"

#include "xdaq/ApplicationStub.h"

#include "xgi/Method.h"
#include "xgi/framework/Method.h"

XDAQ_INSTANTIATOR_IMPL(SimpleClient)

SimpleClient::SimpleClient( xdaq::ApplicationStub *stub ) : xdaq::Application(stub), xgi::framework::UIManager(this) {
  
  xgi::framework::deferredbind(this, this,  &SimpleClient::Default, "Default");
}


SimpleClient::~SimpleClient() {
}

void
SimpleClient::timeExpired(toolbox::task::TimerEvent& event) {

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

