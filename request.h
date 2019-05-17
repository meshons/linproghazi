#ifndef LINPROGHAZI_REQUEST_H
#define LINPROGHAZI_REQUEST_H

#include <memory>

#include "response.h"
#include "connectionhandler.h"

class Request {
public:
    static Response handle(std::shared_ptr<ConnectionHandler>);
};

#endif //LINPROGHAZI_REQUEST_H
