#include "request.h"

Response Request::handle(const std::shared_ptr<ConnectionHandler> & ch) {
    const char * CRLF = "\r\n";
    Request request{ch};

    try {
        std::string str{ch->msg};
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
        std::stringstream requestText{str};

        std::string line;

        getline(requestText, line);
        request.extractRequestLine(line);

        if (!request.simple)
            request.extractHeader(requestText);

        request.extractPostBody(requestText);
    } catch (const HTTP::HTTPException & exception) {
        std::cout << exception.getMsg() << exception.getCode() << std::endl;
        //return request.errorResponse(exception);
    }

    if (request.url.find(':') != std::string::npos) {
        request.url = request.url.substr(request.url.find(':')+1);
    }

    if (request.url.find('?') != std::string::npos) {
        std::stringstream getstr{request.url};
        std::string tmp;
        std::getline(getstr, tmp, '?');
        std::getline(getstr, request.get);
    }

    bool onlyHost = false;
    std::string tmpUrl = request.url;
    std::stringstream urlsplitter{request.url};
    if (tmpUrl.find('/') != std::string::npos) {
        std::string tmp;
        std::getline(urlsplitter, tmp, '/');
    } else
        onlyHost = true;

    if (tmpUrl.find(';') != std::string::npos)
        std::getline(urlsplitter, tmpUrl, ';');
    else if (tmpUrl.find('?') != std::string::npos)
        std::getline(urlsplitter, tmpUrl, '?');
    else
        std::getline(urlsplitter, tmpUrl);

    if (onlyHost)
        tmpUrl = ch->cfg->defaultFileName;

    if (tmpUrl.back() == '/')
        tmpUrl += ch->cfg->defaultFileName;

    if (tmpUrl.empty())
        tmpUrl += '/' + ch->cfg->defaultFileName;

    if (tmpUrl.front() != '/')
        tmpUrl = '/' + tmpUrl;

    request.url = ch->cfg->webPath + tmpUrl;

    std::cout << request.url << std::endl;

    // TODO test url is right

    request.createResponse();

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
        std::getline(request, line);
    }
}

void Request::extractPostBody(std::stringstream & request) {
    std::string line;
    std::getline(request, line);
    while (!request.eof() && line.empty())
        std::getline(request, line);
    if (!line.empty())
        post = line;
}

Response Request::createResponse() const {

    return Response();
}

Response Request::errorResponse(const HTTP::HTTPException &) const {
    return Response();
}
