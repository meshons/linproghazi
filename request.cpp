#include "request.h"

Response Request::handle(const std::shared_ptr<ConnectionHandler> & ch) {
    const char * CRLF = "\r\n";

    try {
        Request request{ch};
        std::stringstream requestText(ch->msg);
        std::string line;

        getline(requestText, line);
        request.extractRequestLine(line);

        if (!request.simple)
            request.extractHeader(requestText);//tovabbi dolgok

        //make respone message

    } catch (const HTTP::HTTPException & exception) {
        std::cout << exception.what() << std::endl;
    }
    Response response;
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

void Request::extractRequestLine(const std::string & requestLine) {
    std::istringstream iss(requestLine);
    std::vector<std::string> words(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());

    if (words.size() < 2 || words.size() > 3)
        throw HTTP::HTTPException("400", "Request line parameter list not correct");

    simple = words.size() == 2;

    method = HTTP::methodType(words[0]);
    url = words[1];
}

void Request::extractHeader(std::stringstream &request) {
    std::string line;
    std::getline(request, line);
    while (!request.eof() && !line.empty()) {
        std::stringstream liness{line};
        std::string name;
        getline(liness, name, ':');
        if (name == line)
            continue;
        if (name == "If-Modified-Since") {
            std::string date;
            std::getline(liness, date);
            if (date.empty())
                throw HTTP::HTTPException("400", "Empty If-Modified-Since");
            std::stringstream dates[3] = {std::stringstream{date},
                                          std::stringstream{date},
                                          std::stringstream{date}};
            std::tm t = {};
            dates[0] >> std::get_time(&t, "%a, %d %b %Y %H:%M:%S");
            if (dates[0].fail()) {
                dates[1] >> std::get_time(&t, "%a, %d-%b-%y %H:%M:%S");
                if (dates[1].fail()) {
                    dates[2] >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
                    if (dates[2].fail())
                        throw HTTP::HTTPException("400", "Wrong time format in If-Modified-Since");
                }
            }

            std::time_t now = std::time(nullptr);
            std::tm * now_tm = std::gmtime(&now);

            if (std::difftime(std::mktime(now_tm), std::mktime(&t)) < 0)
                throw HTTP::HTTPException("400", "If-Modified-Since is greater than server time");

            ifModifiedSince = new std::time_t(std::mktime(&t));
        } else if (name == "Content-Encoding") {
            std::string ccname;
            getline(liness, ccname);
            contentCoding = HTTP::contentCodingDetection(ccname);
        }
    }
}
