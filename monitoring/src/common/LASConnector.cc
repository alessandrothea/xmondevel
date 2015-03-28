/*
 * =====================================================================================
 *
 *       Filename:  LASConnector.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/11/2009 08:28:47 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ECALDEV (), 
 *        Company:  CERN
 *
 * =====================================================================================
 */

#include "monitoring/LASConnector.hh"
#include "log4cplus/loggingmacros.h"
#include <curl/curl.h>
#include "toolbox/string.h"
#include "xdata/String.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/Table.h"
#include "xdata/Boolean.h"
#include "xdata/TimeVal.h"
#include "xdata/Float.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/UnsignedLong.h" 
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedInteger.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/String.h"
#include "xdata/Double.h"
#include "xdata/Vector.h"
#include "xdata/exdr/FixedSizeInputStreamBuffer.h"
#include "xdata/exdr/FixedSizeOutputStreamBuffer.h"
#include "xdata/exdr/Serializer.h"
#include "xcept/tools.h"


    namespace mon {

        const std::string LASConnector::stdPrefix = "urn:xdaq-flashlist:";
        const std::string LASConnector::serviceName = "xmaslas2g";

        //______________________________________________________________________________
        LASConnector::LASConnector(const std::string& host, const std::string& userpwd, log4cplus::Logger l)
            : host_(host), userpwd_(userpwd), logger_(l) {
            }

        //______________________________________________________________________________
        LASConnector::~LASConnector() {

        }

        //______________________________________________________________________________
        xdata::Table::Reference LASConnector::collectFlashList( const std::string& flashlist ) {

            xdata::Table *t = new xdata::Table();
            xdata::Table::Reference refT(t);
            std::vector<char> buffer=collectData( getFlashListURL( flashlist ) );
            try {
                parseResponseExdr(buffer,*refT);
            } catch (xcept::Exception& e) {
                LOG4CPLUS_ERROR(logger_, e.what());
            } 

            return refT;
        }

        //______________________________________________________________________________
        xdata::Table::Reference LASConnector::collectFlashList( const std::string& flashlist, std::map<std::string, std::string> filters ) {

            xdata::Table *t = new xdata::Table();
            xdata::Table::Reference refT(t);
            std::vector<char> buffer=collectData( getFlashListURL( flashlist, filters ) );
            try {
                parseResponseExdr(buffer,*refT);
            } catch (xcept::Exception& e) {
                LOG4CPLUS_ERROR(logger_, e.what());
            } 

            return refT;
        }

        //______________________________________________________________________________
        std::vector<std::string> LASConnector::collectCatalog() {

            std::vector<std::string> flashLists;
            std::vector<char> buf = collectData( getCatalogURL() );

            std::vector<std::string> flashList;
            std::string strbuffer( buf.begin(), buf.end() );
            toolbox::StringTokenizer token(strbuffer,"\n");
            // the first entry is the column title: "Name"
            token.nextToken();
            while( token.hasMoreTokens() ) {
                flashList.push_back(token.nextToken());
            }

            for( std::vector<std::string>::iterator it = flashList.begin(); it != flashList.end(); it++ ) {
                LOG4CPLUS_DEBUG(logger_, "- " + *it);
            }

            return flashList;
        }

        //______________________________________________________________________________
        std::string LASConnector::getFlashListURL( const std::string& listName, std::map< std::string, std::string> filters ) {
            std::string filter = "&flash=";
            if ( listName.find(stdPrefix) == std::string::npos )
                filter += stdPrefix + listName;
            else
                filter += listName;



            std::map< std::string, std::string>::iterator it;
            for( it = filters.begin(); it != filters.end(); ++it ) {
                //            if ( it->second.empty() ) continue;
                filter += '&'+it->first+'='+it->second;
            }

            std::string url =std::string("http://")+host_
                + std::string("/urn:xdaq-application:service="+serviceName+"/retrieveCollection?fmt=exdr")
                + filter;
            return url;
        }

        //______________________________________________________________________________
        std::string LASConnector::getFlashListURL( const std::string& listName ) {
            std::string name = listName;
            if ( listName.find(stdPrefix) == std::string::npos )
                name = stdPrefix + listName;

            std::string url =std::string("http://")+host_
                + std::string("/urn:xdaq-application:service="+serviceName+"/retrieveCollection?fmt=exdr&flash=")
                + name;
            return url;
        }

        //______________________________________________________________________________
        std::string LASConnector::getCatalogURL() {
            std::string url =std::string("http://")+host_
                + std::string("/urn:xdaq-application:service="+serviceName+"/retrieveCatalog?fmt=plain");
            return url;
        }

        //______________________________________________________________________________
        std::vector<char> LASConnector::collectData( std::string url ) {
            std::vector<char> data;

            //        std::cout << "flasshlist url = " << url<< std::endl;
            CURL* cc=curl_easy_init();
            curl_easy_setopt(cc,CURLOPT_URL,url.c_str());
            if (userpwd_.length()>1) curl_easy_setopt(cc,CURLOPT_USERPWD,userpwd_.c_str());
            curl_easy_setopt(cc,CURLOPT_WRITEFUNCTION,LASConnector::readLASFunc);
            curl_easy_setopt(cc,CURLOPT_WRITEDATA,&data);

            curl_easy_perform(cc);

            curl_easy_cleanup(cc);    
            return data; 
        }

        //______________________________________________________________________________
        size_t LASConnector::readLASFunc(void* ptr, size_t sizei, size_t nmemb, void* str) {
            std::vector<char>* buffer=(std::vector<char>*)(str);
            if (sizei==1) {
                char* work=(char*)ptr;
                buffer->insert(buffer->end(),work,work+nmemb);
            }
            return sizei*nmemb;
        }

        //______________________________________________________________________________
        void LASConnector::parseResponseExdr(const std::vector<char>& data, xdata::Table &t) {
            xdata::exdr::Serializer serializer_;

            xdata::Table table;
            xdata::exdr::FixedSizeInputStreamBuffer  inBuffer((char*)(&(data[0])), data.size());

            try {
                serializer_.import(&table, &inBuffer);
            } catch (xdata::exception::Exception& e) {
                LOG4CPLUS_WARN(logger_, "Failed to deserialize incoming table: " <<
                        xcept::stdformat_exception_history(e) << std::endl;
                        ); 
                return;
            }

            t = table;
        }

}
