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


    namespace monitoring {

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

        // //______________________________________________________________________________
        // std::vector<char> LASConnector::collectData2g( std::string url ) {
        //     std::vector<char> data;

        //     //        std::cout << "flasshlist url = " << url<< std::endl;
        //     CURL* cc=curl_easy_init();
        //     curl_easy_setopt(cc,CURLOPT_URL,url.c_str());

        //     if (userpwd_.length()>1) curl_easy_setopt(cc,CURLOPT_USERPWD,userpwd_.c_str());
            
        //     curl_easy_setopt(cc,CURLOPT_WRITEFUNCTION,LASConnector::fileWriter);
            
        //     curl_easy_setopt(cc,CURLOPT_WRITEDATA,&data);

        //     curl_easy_perform(cc);

        //     curl_easy_cleanup(cc);    
        //     return data; 
        // }

        //______________________________________________________________________________
        std::vector<char> LASConnector::collectData( std::string url ) {
            std::vector<char> data;

            long timeout_(4);

           CURL* curl_handle ( 0 );
           CURLcode code;
           // struct MemoryStruct chunk;
           // chunk.memory=0;
           // chunk.size = 0;
           //init the curl session
           curl_handle = curl_easy_init();
           //Error Buffer
           char errorBuffer[CURL_ERROR_SIZE];
           code = curl_easy_setopt ( curl_handle, CURLOPT_ERRORBUFFER, errorBuffer );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed to set error buffer (code " << code << ")." << errorBuffer;
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           //specify URL to get
           // url_ = string2cstr ( url );
           code = curl_easy_setopt ( curl_handle, CURLOPT_URL, url.c_str() );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed to specify URL (code " << code << ")." << errorBuffer;
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           //send all data to this function
           code = curl_easy_setopt ( curl_handle, CURLOPT_WRITEFUNCTION, fileWriter );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed to specify Handler (code " << code << ")." << errorBuffer;
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           //we pass our 'chunk' struct to the callback function
           code = curl_easy_setopt ( curl_handle, CURLOPT_WRITEDATA, ( void* ) &data );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed to specify buffer (code " << code << ")." << errorBuffer;
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           //some servers don't like requests that are made without a user-agent field, so we provide one
           code = curl_easy_setopt ( curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0" );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed to user-agent libcurl-agent/1.0 (code " << code << ")." << errorBuffer;
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           code = curl_easy_setopt ( curl_handle, CURLOPT_TIMEOUT, timeout_ );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed to set write data " << errorBuffer;
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           unsigned long nosignal ( 0 );
           code = curl_easy_setopt ( curl_handle, CURLOPT_NOSIGNAL, nosignal );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed to set write data " << errorBuffer;
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           //get it!
           code = curl_easy_perform ( curl_handle );
         
           if ( code != CURLE_OK )
           {
             std::ostringstream msg;
             msg << "Failed HTTP request to URL '" << url << "' error found (errno =" << code << "): " << errorBuffer << ".";
             XCEPT_RAISE ( xcept::Exception,msg.str() );
           }
         
           //cleanup curl stuff
           curl_easy_cleanup ( curl_handle );

           return data;
        }

        //______________________________________________________________________________
        // size_t LASConnector::readLASFunc(void* ptr, size_t sizei, size_t nmemb, void* str) {
        //     std::vector<char>* buffer=(std::vector<char>*)(str);
        //     if (sizei==1) {
        //         char* work=(char*)ptr;
        //         buffer->insert(buffer->end(),work,work+nmemb);
        //     }
        //     return sizei*nmemb;
        // }

        //______________________________________________________________________________
        size_t LASConnector::fileWriter(void* ptr, size_t size, size_t nmemb, void* data) {
            size_t realsize = size * nmemb;

            std::vector<char>* buffer=(std::vector<char>*)(data);
            char* work=(char*)ptr;
            buffer->insert(buffer->end(),work,work+realsize);
            return realsize;
        }

        //______________________________________________________________________________
        void LASConnector::parseResponseExdr(const std::vector<char>& data, xdata::Table &table) {

            table.clear();
            xdata::exdr::Serializer serializer_;

            // xdata::Table table;
            xdata::exdr::FixedSizeInputStreamBuffer  inBuffer((char*)(&(data[0])), data.size());

            try {
                serializer_.import(&table, &inBuffer);
            } catch (xdata::exception::Exception& e) {
                LOG4CPLUS_WARN(logger_, "Failed to deserialize incoming table: " <<
                        xcept::stdformat_exception_history(e) << std::endl;
                        ); 
                return;
            }

            std::stringstream ss;
            ss << "Table successufully imported from exdr format (rows " << table.getRowCount() << ")";
            LOG4CPLUS_INFO(logger_, ss.str());
            // t = table;
        }

}
