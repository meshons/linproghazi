#ifndef LINPROGHAZI_CONTENT_H
#define LINPROGHAZI_CONTENT_H

#include <algorithm>
#include <string>
#include "exception.h"

namespace HTTP{
    enum contentCoding {
        gzip,
        compress,
        none
    };

    contentCoding contentCodingDetection(std::string methodname);
}

#endif //LINPROGHAZI_CONTENT_H
