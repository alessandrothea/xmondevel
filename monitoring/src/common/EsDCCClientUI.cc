#include "monitoring/EsDCCClientUI.hh"
#include "monitoring/EsDCCClient.hh"

#include "xgi/Method.h"
#include "xgi/framework/Method.h"

#include "toolbox/string.h"

namespace monitoring {

EsDCCClientUI::EsDCCClientUI( EsDCCClient* client ) :
ClientUIBase( client ) {
  xgi::framework::deferredbind(client, this,  &EsDCCClientUI::Default, "Default");

  xgi::deferredbind(client, this, &EsDCCClientUI::getUpdatedValues, "getUpdatedValues");

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
        "        <th>last bx</th>\n"
        "        <th>last l1a</th>\n"
        "      </tr>\n"
        "    </thead>\n"
        "    <tbody>\n";

    for ( itFed = client->feds_.begin(); itFed != client->feds_.end(); ++itFed ) {
      const std::string baseId = toolbox::toString("fed%d_", itFed->first);
      *out <<
          "      <tr>\n"
          "        <td >" << itFed->first << "</td>\n"
          "        <td id='"<< baseId << "reportTime'>" << itFed->second.reportTime.toString() << "</td>\n"
          "        <td id='"<< baseId << "daqOk' >" << itFed->second.daqOk << "</td>\n"
          "        <td id='"<< baseId << "bx' >" << itFed->second.bx << "</td>\n"
          "        <td id='"<< baseId << "l1a' >" << itFed->second.l1a << "</td>\n"
          "      </tr>\n";
    }

    *out << 
      "</tbody>\n"
      "</table>\n"
      "<div id='result'></div>\n"
      "<div id='error'></div>\n";

    client->fedsLock_.give();

}

void
EsDCCClientUI::getUpdatedValues(xgi::Input* in, xgi::Output* out) throw (xgi::exception::Exception) {

    EsDCCClient* client = getClient<EsDCCClient>();

    client->fedsLock_.take();

    if ( !client->feds_.size() ) {
      *out << "{}";
      return;
    }

    std::map<unsigned int, EsFedStatus>::iterator itFed;
    *out << "{\n";

    std::stringstream sout;
    for ( itFed = client->feds_.begin(); itFed != client->feds_.end(); ++itFed ) {
      const std::string baseId = toolbox::toString("fed%d_", itFed->first);
      *out <<
          "\""<< baseId << "reportTime\" : \"" << itFed->second.reportTime.toString() << "\",\n"
          "\""<< baseId << "daqOk\" : " << itFed->second.daqOk << ",\n"
          "\""<< baseId << "bx\" : " << itFed->second.bx << ",\n"
          "\""<< baseId << "l1a\" : " << itFed->second.l1a << ",\n";
    }

    long pos = out->tellp();
    out->seekp(pos-2);
    // std::string sout.str());

    *out << "\n}";

    client->fedsLock_.give();
}


}