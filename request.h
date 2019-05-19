#ifndef LINPROGHAZI_REQUEST_H
#define LINPROGHAZI_REQUEST_H

#include <memory>
#include <iomanip>

#include "response.h"
#include "connectionhandler.h"
#include "HTTP/method.h"
#include "HTTP/exception.h"

class Request {
public:
    static Response handle(std::shared_ptr<ConnectionHandler>);
private:
    std::shared_ptr<ConnectionHandler> ch;
    Response response;
    HTTP::method method;
    std::string url;
public:
    explicit Request(std::shared_ptr<ConnectionHandler> ch):ch{ch} {}
private:
    Response errorResponse();
    void extractRequestLine(std::string);
};

#endif //LINPROGHAZI_REQUEST_H
