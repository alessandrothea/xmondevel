#include "monitoring/Client.hh"

#include "xdaq/ApplicationStub.h"

#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"


#include "xgi/Method.h"
#include "xgi/framework/Method.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

ALayout::ALayout( xdaq::Application* app ) {
  // 
  std::stringstream callback;
  callback << app->getApplicationDescriptor()->getContextDescriptor()->getURL();
  callback << "/" << app->getApplicationDescriptor()->getURN() << "/updateData";

  callback_ = callback.str();

}

ALayout::~ALayout() {

}

void
ALayout::getHTMLHeader(xgi::framework::UIManager* manager, xgi::Input* in, xgi::Output* out) throw (xgi::exception::Exception) {

  xgi::Output tmpOut;
  tmpOut.str("");
  tmpOut.setHTTPResponseHeader(out->getHTTPResponseHeader());

  // Get the 'default' HyperDAQ layout.
  hyperdaq::framework::Layout::getHTMLHeader(manager, in, &tmpOut);

  std::string headerStr = tmpOut.str();


  std::string scriptStr = 
  "<script type='text/javascript'>\n"
  "\n"
  "  dummy = {}\n"
  "  dummy.updateUrl = '"+callback_+"'\n"
  "\n"
  "  $(function worker(){\n"
  "      // don't cache ajax or content won't be fresh\n"
  "      $.ajaxSetup ({\n"
  "          cache: false,\n"
  "          complete: function() {\n"
  "            // Schedule the next request when the current one's complete\n"
  "            setTimeout(worker, 2000);\n"
  "          }\n"
  "      });\n"
  "\n"
  "      updateData();\n"
  "  // end  \n"
  "  });\n"
  "\n"
  "  function updateData()\n"
  "  {\n"
  "     var data;\n"
  "      $.ajax({\n"
  "          url : dummy.updateUrl,\n"
  "          dataType : 'json',\n"
  "          data : data,\n"
  "          timeout : 2000,\n"
  "          success : processAJAXSuccess,\n"
  "          error : processAJAXError\n"
  "      });\n"
  "  };\n"
  "\n"
  "  function processAJAXSuccess(data, textStatus, jqXHR) {\n"
  "\n"
  "      if (data == '') {\n"
  "          $('#result').html('Failed to retrieve ajax data');\n"
  "          return;\n"
  "      }\n"
  "\n"
  "      $('#result').html('The counter is now: ' + data['counterA'])\n"
  "\n"      
  "      for ( var key in data ) {\n"
  "         $('#'+key).html(data[key]);\n"
  "      }\n"
  "\n"
  "  }\n"
  "\n"
  "  function processAJAXError(data, textStatus, jqXHR) {\n"
  "      $('error').html('something went wrong...')\n"
  "  }\n"
  "\n"
  "</script>";



  size_t pos = headerStr.find("</head>");
  if (pos == std::string::npos) {
      pos = headerStr.size();
  }
  headerStr.insert(pos, scriptStr);

  out->setHTTPResponseHeader(tmpOut.getHTTPResponseHeader());
  *out << headerStr;

  // See tcds/utils/Layout.cc
}



XDAQ_INSTANTIATOR_IMPL(SimpleClient)

SimpleClient::SimpleClient( xdaq::ApplicationStub *stub ) :
  xdaq::Application(stub),
  xgi::framework::UIManager(this),
  lock_ (toolbox::BSem::FULL),
  layout_(this),
  counter_(0x0) {
  
  xgi::framework::deferredbind(this, this,  &SimpleClient::Default, "Default");
  //xgi::framework::deferredbind(this, this,  &SimpleClient::getConsoleData, "getConsoleData");
  
   xgi::bind(this, &SimpleClient::getConsoleData, "getConsoleData");
   xgi::bind(this, &SimpleClient::updateData, "updateData");

  setLayout(&layout_);
  
  toolbox::task::Timer * timer =
  toolbox::task::getTimerFactory()->createTimer("PeriodicTime");
  toolbox::TimeInterval interval(2,0); // period of 8 secs 
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
      "<table class='xdaq-table'>\n"
      "  <caption>My XDAQ Table</caption>\n"
      "    <thead>\n"
      "      <tr>\n"
      "        <th>Counter A</th>\n"
      "        <th>Counter B</th>\n"
      "      </tr>\n"
      "    </thead>\n"
      "    <tbody>\n"
      "      <tr>\n"
      "        <td id='counterA'>Nirvana</td>\n"
      "        <td id='counterB'>Nevermind</td>\n"
      "      </tr>\n"
      "</tbody>\n"
      "</table>\n"
      "<div id='result'></div>\n"
      "<div id='error'></div>\n";
  

  // std::stringstream callback;
    // callback << this->getApplicationDescriptor()->getContextDescriptor()->getURL();
    // callback << "/" << this->getApplicationDescriptor()->getURN() << "/getConsoleData";

    // *out << "<div id=\"my-console\" class=\"xdaq-console\" data-callback=\"" << callback.str() << "\" autosize></div>" << std::endl;

}

void SimpleClient::getConsoleData(xgi::Input*, xgi::Output* out) throw (xgi::exception::Exception) {
  out->getHTTPResponseHeader().addHeader("Content-Type", "text/plain");
  *out << "Counter : " << counter_;
}

void SimpleClient::updateData(xgi::Input*, xgi::Output* out) throw (xgi::exception::Exception) {
  *out << "{ \"counterA\": " << counter_ << ", \"counterB\": " << counter_%3 << "}";
}
