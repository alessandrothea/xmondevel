#include "monitoring/EsDCCClient.hh"

#include "monitoring/EsDCCClientUI.hh"

#include "log4cplus/loggingmacros.h"

#include <xdata/TableIterator.h>
#include <xdata/TimeVal.h>

#include <sstream>

XDAQ_INSTANTIATOR_IMPL(monitoring::EsDCCClient)

namespace monitoring {

EsDCCClient::EsDCCClient ( xdaq::ApplicationStub* stub ) :
ClientBase(stub),
fedsLock_ (toolbox::BSem::FULL)  {

    addFlashListCallback("urn:xdaq-flashlist:esDCCStatus", this, &EsDCCClient::updateGlobalStatus);
    addFlashListCallback("urn:xdaq-flashlist:hostInfo", this, &EsDCCClient::checkDiskInfo);

    // fire up the UI
    registerGui( new EsDCCClientUI(this) );
}
    
EsDCCClient::~EsDCCClient ( ) {

}

void EsDCCClient::updateGlobalStatus( xdata::Table::Reference& table ) throw (xcept::Exception) {
    std::stringstream ss;
    ss << "Processing esDCC status flashlist (rows " << table->getRowCount() << ")";
    LOG4CPLUS_INFO(getApplicationLogger(), ss.str());


    xdata::TableIterator rowIt, boardIt;
    for ( rowIt = table->begin(); rowIt != table->end(); ++rowIt) {
        xdata::TimeVal *ts = dynamic_cast<xdata::TimeVal*>( rowIt->getField("timestamp") );
        xdata::Table *statusTable = (xdata::Table*)rowIt->getField("dccStatusTable");
        
        for ( boardIt = statusTable->begin(); boardIt != statusTable->end(); ++boardIt) {
            //store the table in the buffer
            xdata::UnsignedInteger *id = dynamic_cast<xdata::UnsignedInteger*>( boardIt->getField("Board ID") );
            LOG4CPLUS_INFO(getApplicationLogger(),"Found Board ID "+id->toString());



            // 
            EsFedStatus status;

            status.reportTime = *ts;

            std::stringstream report;
            
            std::string info("");
            try {
                info = statusTable->getValueAt(0,"Info")->toString();
                report << info;
            } catch (xcept::Exception &e) {
            }

            if (
                (info.find("S-LINK FIFO FULL : YES", 0) != std::string::npos) ||
                (info.find("S-LINK DOWN : YES", 0) != std::string::npos)) {
                status.daqOk = false;
            } else {
                status.daqOk = true;
            }

            fedsLock_.take();
            feds_[*id] = status;
            fedsLock_.give();
        }
    }
}

void EsDCCClient::checkDiskInfo( xdata::Table::Reference& table ) throw (xcept::Exception) {
    std::stringstream ss;
    ss << "Processing diskInfo status flashlist (rows " << table->getRowCount() << ")";
    LOG4CPLUS_INFO(getApplicationLogger(), ss.str());


    std::cout << table->toString() << std::endl;
}

}
