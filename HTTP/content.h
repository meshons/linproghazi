#ifndef LINPROGHAZI_CONTENT_H
#define LINPROGHAZI_CONTENT_H

#include <algorithm>
#include <string>
#include "exception.h"

namespace HTTP{
    /**
     * Az állományok kódolásáért felelős enum
     */
    enum contentCoding {
        gzip,
        compress,
        none
    };

    /**
     * Adott nevű kódolásra visszaadja a contentCoding enum megfelelőjét
     * @param contentCodingName a kódolás neve
     * @return az enum-ban vett megfelelője a kódolásnak
     */
    contentCoding contentCodingDetection(std::string contentCodingName);
}

#endif //LINPROGHAZI_CONTENT_H
