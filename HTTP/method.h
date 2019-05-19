#ifndef LINPROGHAZI_METHOD_H
#define LINPROGHAZI_METHOD_H

#include <string>
#include <algorithm>
#include "exception.h"

namespace HTTP{
    enum method {
        GET,
        POST,
        HEAD
    };

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

#endif //LINPROGHAZI_METHOD_H
