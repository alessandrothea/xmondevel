
#include "hyperdaq/framework/Layout.h"

namespace monitoring {

  class Layout: public hyperdaq::framework::Layout {
  public:
    Layout( xdaq::Application* app );
    virtual ~Layout();

    // NOTE: This method needs the throw specifier, since the parent
    // class already has it.
    virtual void getHTMLHeader(xgi::framework::UIManager* manager,
                               xgi::Input* in,
                               xgi::Output* out) throw (xgi::exception::Exception);
  private:
    std::string callback_;
  };

}