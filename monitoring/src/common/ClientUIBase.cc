#include "monitoring/ClientUIBase.hh"


#include "xgi/Method.h"
#include "xgi/framework/Method.h"

namespace monitoring {
ClientUIBase::ClientUIBase( ClientBase* client ) : 
 xgi::framework::UIManager(client),
 client_(client),
 layout_(client) {

  xgi::framework::deferredbind(client_, this,  &ClientUIBase::Default, "Default");

  xgi::deferredbind(client_, this, &ClientUIBase::getUpdatedValues, "getUpdatedValues");

  setLayout(&layout_);
}

ClientUIBase::~ClientUIBase() {

}

void
ClientUIBase::Default(xgi::Input* in, xgi::Output* out) throw (xgi::exception::Exception) {
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

}

void
ClientUIBase::getUpdatedValues(xgi::Input* in, xgi::Output* out) throw (xgi::exception::Exception) {
    *out << "{ \"a\": 5 }";
}


void
ClientUIBase::displayTableToHTML( std::ostream& out, xdata::Serializable * s ) {
    xdata::Table * tref = dynamic_cast<xdata::Table*>(s);

    if (tref->getRowCount() == 0)
    {
        // mutex_.give();
        return;
    }
    out << "<table class=\"xdaq-table\">";
    out << "<thead>";
    out << "<tr>";
    // By default it denormalize table
    
    
    std::vector<std::string> columns = tref->getColumns();
    for (std::vector<std::string>::size_type i = 0; i < columns.size(); i++ )
    {
        std::string localName = columns[i].substr(columns[i].rfind(":")+1);
        //*out << cgicc::td(localName).set("title",columns[i]).set("style","vertical-align: top; font-weight: bold;");
        out << "<th title=\"" << columns[i] << "\" class=\"xdaq-sortable\">" << localName << "</th>";
    }
    out << "<tr>" << std::endl;
    out << "</thead>";
    
    out << "<tbody>";

    for ( size_t j = 0; j <  tref->getRowCount(); j++ )
    {
        out << "<tr>" << std::endl;
        for (std::vector<std::string>::size_type k = 0; k < columns.size(); k++ )
        {
            xdata::Serializable * s = tref->getValueAt(j, columns[k]);
            
            if (s->type() == "mime")
            {
                
                //*out << cgicc::td("MIME").set("style","vertical-align: top;");
                out << "<td>MIME</td>" ;
                //xdata::Mime* m = dynamic_cast<xdata::Mime*>(s);
                //std::cout << *(m->getEntity()) << std::endl;
            }
            else if (s->type() == "table")
            {
                    out << "<td>";
                    this->displayTableToHTML(  out, s); 
                    out << "</td>" ;

            }
            else
            {                       
                //*out << cgicc::td(s->toString()).set("style","vertical-align: top;");
                out << "<td style=\"vertical-align: top;\">" << s->toString() << "</td>" ;
            }
        }
        out << "</tr>";
    }
    out << "</tbody>";  
    out << "</table>";
}

}