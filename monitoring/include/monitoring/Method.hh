#include "toolbox/lang/Method.h"

namespace monitoring {
class MethodSignature: public toolbox::lang::Method {
    public:
        virtual ~MethodSignature() {
        }

        virtual void invoke ( xdata::Table::Reference& table ) throw (xcept::Exception) = 0;
        
        virtual std::string name() = 0;
};

template <class LISTENER>
class Method: public MethodSignature {
    public:

    virtual ~Method(){
    }

    std::string type(){
        return "mon";
    }
        
    void invoke ( xdata::Table::Reference& table ) throw (xcept::Exception){
        // signature is void f(xgi::Input,xgi::Output)  
        (obj_->*func_)(table);
    }
    
    std::string name(){
        return name_;
    }
    
    void name(std::string name){
        name_ = name;
    }
    
    LISTENER * obj_;
    void (LISTENER::*func_)( xdata::Table::Reference& table ) throw (xcept::Exception);
    std::string name_;
};


}