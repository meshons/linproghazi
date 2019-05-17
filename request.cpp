#include "request.h"

Response Request::handle(std::shared_ptr<ConnectionHandler> ch) {
    Response response;
    std::stringstream message;

    //interpret request

    //make respone message

    response.msg = std::string("HTTP/1.0 200 OK\n"
                               "Date: Fri, 17 May 2019 08:39:36 GMT\n"
                               "Server: Apache\n"
                               "Last-Modified: Wed, 16 Jan 2019 12:59:18 GMT\n"
                               "Vary: Accept-Encoding\n"
                               "Content-Length: 11\n"
                               "Content-Type: text/html\n\n"
                               "Hello world");

    return response;
}
