#include "monitoring/Layout.hh"

#include "xdaq/Application.h"
namespace monitoring {


Layout::Layout( xdaq::Application* app ) {
  // 
  std::stringstream callback;
  callback << app->getApplicationDescriptor()->getContextDescriptor()->getURL();
  callback << "/" << app->getApplicationDescriptor()->getURN() << "/getUpdatedValues";

  callback_ = callback.str();

}

Layout::~Layout() {

}

void
Layout::getHTMLHeader(xgi::framework::UIManager* manager, xgi::Input* in, xgi::Output* out) throw (xgi::exception::Exception) {

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
  // "      $('#result').html('The counter is now: ' + data['counterA'])\n"
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

   
}