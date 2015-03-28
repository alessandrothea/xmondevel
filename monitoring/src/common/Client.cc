#include "monitoring/Client.hh"

#include "xdaq/ApplicationStub.h"

#include "xgi/Method.h"
#include "xgi/framework/Method.h"


SimpleClient::SimpleClient( xdaq::ApplicationStub *stub ) : xdaq::Application(stub), xgi::framework::UIManager(this) {
  
  xgi::framework::deferredbind(this, this,  &SimpleClient::Default, "Default");
}


SimpleClient::~SimpleClient() {
}

void
SimpleClient::timeExpired(toolbox::task::TimerEvent& event) {

}

void
SimpleClient::Default(xgi::Input*, xgi::Output*) {

}

