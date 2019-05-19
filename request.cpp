#include "request.h"

Response Request::handle(std::shared_ptr<ConnectionHandler> ch) {
    Response response;
    const char * CRLF = "\r\n";
    std::stringstream request(ch->msg);
    std::string method;

    request >> method;
    //interpret request

    //make respone message

    std::stringstream message;
    message << "HTTP/1.0 ";
    // response code and message
    message << CRLF;
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::gmtime(&t);
    message << "Date: " << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S %Z") << CRLF;
    message << "Server: linproghazi" << CRLF;
    message << CRLF;
    //message content

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

void Request::extractRequestLine(std::string requestLine) {
    std::istringstream iss(requestLine);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());


}