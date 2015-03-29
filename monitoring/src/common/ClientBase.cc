#include "monitoring/ClientBase.hh"

#include "monitoring/ClientUIBase.hh"
#include "monitoring/LASConnector.hh"

#include "toolbox/string.h"
#include "toolbox/task/Timer.h"
#include "toolbox/task/TimerFactory.h"
#include "toolbox/TimeInterval.h"

#include "log4cplus/loggingmacros.h"


XDAQ_INSTANTIATOR_IMPL(monitoring::ClientBase)

namespace monitoring {

ClientBase::ClientBase( xdaq::ApplicationStub *stub ) :
  xdaq::Application(stub),
  gui_(0x0),
  lock_ (toolbox::BSem::FULL) {
    LOG4CPLUS_INFO(getApplicationLogger(), "Client build");

    updatePeriod_ = "PT10S";
    getApplicationInfoSpace()->fireItemAvailable("updatePeriod",&updatePeriod_);

    // address of the las service
    getApplicationInfoSpace()->fireItemAvailable("slashURL",&slashURL_);

    // address of the las service
    getApplicationInfoSpace()->fireItemAvailable("flashlists",&flashlists_);

    // Register this as listener to setDefaultValues
    getApplicationInfoSpace()->addListener(this, "urn:xdaq-event:setDefaultValues");


}


ClientBase::~ClientBase() {
  updateTimer_->stop();

  if ( gui_ ) delete gui_;
}


void ClientBase::timeExpired(toolbox::task::TimerEvent& event) {
  LOG4CPLUS_INFO(getApplicationLogger(), "timeExpired, event: "+event.type());

  std::string url = slashURL_.toString();
  if ( url.empty() ) {
    XCEPT_RAISE(xcept::Exception, "slashURL empty. Impossible to collect the data.");
  }

  try {
    LASConnector collector(slashURL_.toString(),"",getApplicationLogger());
    std::vector<std::string> catalog = collector.collectCatalog();

    // Tokenise the list of flashlists
    toolbox::StringTokenizer strtok(flashlists_.toString(),",");

    // And prepare a shopping list
    std::vector<std::string> flToFetch;

    std::vector<std::string>::iterator itCat, itFl;

    // Fill the shopping list
    while( strtok.hasMoreTokens() ) {
      std::string fl = strtok.nextToken();
      for( itCat = catalog.begin(); itCat != catalog.end(); itCat++ ) {
        // No, match, continue
        if ( (*itCat).find( fl ) == std::string::npos ) continue;
        flToFetch.push_back( *itCat );
        LOG4CPLUS_INFO(getApplicationLogger(), "Flashlist " + *itCat + " marked for retrieval.");
      }
    }

    // 
    std::map<std::string, std::string> filters;
    filters["context"] = ".*";

    // Fetch all requested flashlists
    for( itFl = flToFetch.begin(); itFl != flToFetch.end(); ++itFl ) {
      // xdata::Table::Reference t = collector.collectFlashList( *itFl, filters );
      xdata::Table::Reference t = collector.collectFlashList( *itFl );
      std::vector<std::string> cols = t->getColumns();
      if ( cols.size() == 0 ) continue;

      std::stringstream ss;
      ss << "Collected table " + *itFl + " [ ";
      for (std::vector<std::string>::iterator i = cols.begin(); i != cols.end(); ++i) {
          ss << *i << " ";
      }

      ss << "]";

      LOG4CPLUS_INFO(getApplicationLogger(), ss.str());

      processReport( *itFl, t );
    }

  } catch (xcept::Exception &e) {
    LOG4CPLUS_ERROR(getApplicationLogger(), "Cazzarola"+std::string(e.what()));
  }

}


void ClientBase::actionPerformed(xdata::Event& event) {
  LOG4CPLUS_INFO(getApplicationLogger(), "actionPerformed, event: "+event.type());

    try {
        
        if (event.type() == "urn:xdaq-event:setDefaultValues") {
            LOG4CPLUS_INFO(getApplicationLogger(),"Default values loaded");

            if ( !toolbox::task::getTimerFactory()->hasTimer("urn:xmas:las2g-timer") ) {
                updateTimer_ = toolbox::task::getTimerFactory()->createTimer("urn:xmas:las2g-timer");
            } else {
                updateTimer_ = toolbox::task::getTimerFactory()->getTimer("urn:xmas:las2g-timer");
            }
            // fire timer!
            
            toolbox::TimeInterval interval; 
            interval.fromString(updatePeriod_);
            toolbox::TimeVal start;
            start = toolbox::TimeVal::gettimeofday();
            updateTimer_->scheduleAtFixedRate( start, this, interval,  0, "" );  
            
        }
    }
    catch (xcept::Exception &e) 
    {
//        XCEPT_DECLARE_NESTED(ecal::mon::exception::ClientException, ex, "failure in setting default values" ,e);
//        client_->notifyQualified("error",e);
        LOG4CPLUS_ERROR(getApplicationLogger(),"Failure in setting default values");

    }
}


void ClientBase::processReport( const std::string& fl, xdata::Table::Reference& table ) {

  std::vector<std::string> cols = table->getColumns();

  // Nothing to do
  if(cols.size()==0) {
      return;
  }

  MethodMap::iterator it;
  if ( (it = callbacks_.find(fl)) == callbacks_.end() ) {
    LOG4CPLUS_INFO(getApplicationLogger(),"No callback for flashlist " + fl);
  } 

  std::vector<monitoring::MethodSignature*>::iterator itM;
  for ( itM = it->second.begin(); itM != it->second.end(); ++itM) {
    (*itM)->invoke(table);    
  }
}


}
