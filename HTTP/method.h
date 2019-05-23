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

    method methodType(std::string methodname);
}

#endif //LINPROGHAZI_METHOD_H
