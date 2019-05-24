#include "request.h"
#include "phpCaller.h"

Response Request::handle(const std::shared_ptr<ConnectionHandler> & ch) {
    const char * CRLF = "\r\n";
    Request request{ch};

    try {
        std::string str{ch->msg};
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
        std::stringstream requestText{str};

        std::string line;

        getline(requestText, line);
        if (line.empty()) {
            Response rsp;
            rsp.msg = "";
            return rsp;
        }
        request.extractRequestLine(line);

        if (!request.simple)
            request.extractHeader(requestText);

        request.extractPostBody(requestText);

        request.extractUrl();

        return request.createResponse();
    } catch (const HTTP::HTTPException & exception) {
        return request.errorResponse(exception);
    }
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
    std::string CRLF = "\r\n";

    std::string extension;
    std::stringstream filenameExtractor{url};
    while (!filenameExtractor.eof())
        std::getline(filenameExtractor, extension, '/');
    std::stringstream extensionExtractor{extension};
    std::string tmp;
    std::getline(extensionExtractor, tmp, '.');
    std::getline(extensionExtractor, extension);

    std::string fileContent;

    std::ifstream in(url);
    if (!in.good())
        throw HTTP::HTTPException("404", "File not exists.");

    if (extension == "php") {
        in.close();
        phpCaller php{get, post, ch->cfg, url};
        fileContent = php.response();
    } else {
        fileContent = std::string(static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str());
    }
    in.close();

    Response response;

    std::stringstream message;

    message << "HTTP/1.0 200 okesBro" << CRLF;
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::gmtime(&t);
    message << "Date: " << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S %Z") << CRLF;
    message << "Server: linproghazi" << CRLF;
    message << "Content-Type: " << ch->cfg->mimeHandler.getMime(extension) << CRLF;
    message << "Content-Length: " << fileContent.length() << CRLF;

    message << CRLF;

    if (method!=HTTP::method::HEAD) {
        message << fileContent;
    }

    response.msg = message.str();

    return response;
}

Response Request::errorResponse(const HTTP::HTTPException &e) const {
    Response response;
    std::string CRLF = "\r\n";
    std::stringstream message;
    std::string fileContent, errorFile = ch->cfg->webPath+ch->cfg->errorFileName;

    std::ifstream in(errorFile);
    if (!in.good())
        std::cout << "No error file" << std::endl;

    in.close();

    std::string errorPost;
    errorPost+="code="+e.getCode()+"&msg="+e.getMsg();

    phpCaller php{"", errorPost, ch->cfg, errorFile};
    fileContent = php.response();

    message << "HTTP/1.0 " << e.getCode() << " nemOkesBro" << CRLF;
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::gmtime(&t);
    message << "Date: " << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S %Z") << CRLF;
    message << "Server: linproghazi" << CRLF;
    message << "Content-Type: " << ch->cfg->mimeHandler.getMime("php") << CRLF;
    message << "Content-Length: " << fileContent.length() << CRLF;

    message << CRLF;

    if (method!=HTTP::method::HEAD) {
        message << fileContent;
    }

    response.msg = message.str();
    return response;
}

void Request::extractUrl() {
    if (url.find(':') != std::string::npos) {
        url = url.substr(url.find(':')+1);
    }

    if (url.find('?') != std::string::npos) {
        std::stringstream getstr{url};
        std::string tmp;
        std::getline(getstr, tmp, '?');
        std::getline(getstr, get);
    }

    bool onlyHost = false;
    std::string tmpUrl = url;
    std::stringstream urlsplitter{url};
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

    url = ch->cfg->webPath + tmpUrl;
}
