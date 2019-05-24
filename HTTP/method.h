#ifndef LINPROGHAZI_METHOD_H
#define LINPROGHAZI_METHOD_H

#include <string>
#include <algorithm>
#include "exception.h"

namespace HTTP{
    /**
     * A HTTP metódus enumja
     */
    enum method {
        GET,
        POST,
        HEAD
    };

    /**
     * Visszatér az enum megfelelőjével egy metódusnak
     * @param methodname a metódus neve
     * @return a metódus enum megfelelője
     */
    method methodType(std::string methodname);
}

#endif //LINPROGHAZI_METHOD_H
