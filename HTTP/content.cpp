#include "content.h"

namespace HTTP{
    contentCoding contentCodingDetection(std::string ccname) {
        std::transform(ccname.begin(), ccname.end(), ccname.begin(), ::toupper);
        contentCoding m;
        if (ccname == "GZIP" || ccname == "X-GZIP")
            m = contentCoding::gzip;
        else if (ccname == "COMPRESS" || ccname == "X-COMPRESS")
            m = contentCoding::compress;
        else
            m = contentCoding::none;
        return m;
    }
}
