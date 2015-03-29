#ifndef __MONITORING_CLIENTBASEUI_HH__
#define __MONITORING_CLIENTBASEUI_HH__ 

#include "monitoring/ClientBase.hh"
#include "monitoring/Layout.hh"

#include "xgi/framework/UIManager.h"

namespace monitoring {
class ClientUIBase : public xgi::framework::UIManager
{
public:
    ClientUIBase( ClientBase* );
    virtual ~ClientUIBase();


    void Default(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);

    void getUpdatedValues(xgi::Input*, xgi::Output*) throw (xgi::exception::Exception);

    void displayTableToHTML( std::ostream& out, xdata::Serializable * s );

    template<class C>
    C* getClient() { return dynamic_cast<C*>(client_); }
private:
    ClientBase* client_;

};

#endif

}