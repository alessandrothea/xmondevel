/*
 * =====================================================================================
 * 
 *       Filename:  LASConnector.hh
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  07/11/2009 08:18:00 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  ECALDEV (), 
 *        Company:  CERN
 * 
 * =====================================================================================
 */


#ifndef _ecal_LASConnector_hh_
#define _ecal_LASConnector_hh_

#include "xdata/Table.h"
#include <string>
#include "log4cplus/logger.h"


namespace mon {

class LASConnector {
    public:
        LASConnector(const std::string& host, const std::string& userpwd,log4cplus::Logger l);
        xdata::Table::Reference collectFlashList( const std::string& flashlist );
        xdata::Table::Reference collectFlashList( const std::string& flashlist, std::map<std::string, std::string> filters );
        std::vector<std::string> collectCatalog();
        virtual ~LASConnector();


    protected:
        void parseResponseExdr(const std::vector<char>& data, xdata::Table& t);
        std::vector<char> collectData( std::string url );
        std::string getFlashListURL( const std::string& listName );
        std::string getFlashListURL( const std::string& listName, std::map< std::string, std::string> filters );
        std::string getCatalogURL();
        std::string host_;
        std::string userpwd_;
        std::string tableUrl_;
        log4cplus::Logger logger_;

        static size_t readLASFunc(void* ptr, size_t sizei, size_t nmemb, void* str);

        static const std::string stdPrefix;
        static const std::string serviceName;
};
}

#endif /* _ecal_LASConnector_hh_ */
