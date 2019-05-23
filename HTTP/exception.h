#ifndef LINPROGHAZI_EXCEPTION_H
#define LINPROGHAZI_EXCEPTION_H

#include <exception>
#include <string>

namespace HTTP{
    class HTTPException : public std::exception
    {
        const std::string code;
        const std::string msg;
    public:
        HTTPException(std::string &&code, std::string &&msg): code{code}, msg{msg} {}

        const char * what() const noexcept
        {
            return (std::string("HTTP Exception ")+code+msg).c_str();
        }

        bool displayable() const {
            return true;
        }

        const std::string & getCode() const {
            return code;
        }

        const std::string & getMsg() const {
            return msg;
        }
    };
}

#endif //LINPROGHAZI_EXCEPTION_H
