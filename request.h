#ifndef LINPROGHAZI_REQUEST_H
#define LINPROGHAZI_REQUEST_H

#include <memory>
#include <iomanip>

#include "response.h"
#include "connectionhandler.h"
#include "HTTP/method.h"
#include "HTTP/exception.h"
#include "HTTP/content.h"

class Request {
public:
    static Response handle(const std::shared_ptr<ConnectionHandler> &);
private:
    std::shared_ptr<ConnectionHandler> ch;
    Response response;
    HTTP::method method;
    std::string url;
    bool simple;
    std::time_t * ifModifiedSince = nullptr;
    HTTP::contentCoding contentCoding;
public:
    explicit Request(std::shared_ptr<ConnectionHandler> ch):ch{std::move(ch)} {
        method = HTTP::method::GET;
    }
    ~Request() {
        delete ifModifiedSince;

    }
private:
    Response errorResponse(const HTTP::HTTPException &);
    void extractRequestLine(const std::string &);
    void extractHeader(std::stringstream &);
};

#endif //LINPROGHAZI_REQUEST_H
