#include "monitoring/EsDCCClientUI.hh"
#include "monitoring/EsDCCClient.hh"

#include "xgi/Method.h"
#include "xgi/framework/Method.h"

namespace monitoring {

EsDCCClientUI::EsDCCClientUI( EsDCCClient* client ) :
ClientUIBase( client ) {
  xgi::framework::deferredbind(client, this,  &EsDCCClientUI::Default, "Default");

}


EsDCCClientUI::~EsDCCClientUI() {

}

void
EsDCCClientUI::Default(xgi::Input* in, xgi::Output* out) throw (xgi::exception::Exception) {
  
    EsDCCClient* client = getClient<EsDCCClient>();
    std::map<unsigned int, EsFedStatus>::iterator itFed;
  
    client->fedsLock_.take();
  
    *out << 
        "<table class='xdaq-table'>\n"
        "  <caption>Collected Fed IDs</caption>\n"
        "    <thead>\n"
        "      <tr>\n"
        "        <th>Fed</th>\n"
        "        <th>time stamp</th>\n"
        "        <th>daq OK</th>\n"
        "      </tr>\n"
        "    </thead>\n"
        "    <tbody>\n";

    for ( itFed = client->feds_.begin(); itFed != client->feds_.end(); ++itFed ) {
    *out <<
        "      <tr>\n"
        "        <td >" << itFed->first << "</td>\n"
        "        <td >" << itFed->second.reportTime.toString() << "</td>\n"
        "        <td >" << itFed->second.daqOk << "</td>\n"
        "      </tr>\n";
    }

    *out << 
    "</tbody>\n"
    "</table>\n"
    "<div id='result'></div>\n"
    "<div id='error'></div>\n";

    client->fedsLock_.give();

}

}