#include "method.h"

namespace HTTP{
    method methodType(std::string methodname) {
        std::transform(methodname.begin(), methodname.end(), methodname.begin(), ::toupper);
        method m;
        if (methodname == "GET")
            m = method::GET;
        else if (methodname == "POST")
            m = method::POST;
        else if (methodname == "HEAD")
            m = method::HEAD;
        else
            throw HTTPException("400", "No proper method specified");
        return m;
    }
}
