#ifndef LINPROGHAZI_EXCEPTION_H
#define LINPROGHAZI_EXCEPTION_H

#include <exception>
#include <string>

namespace HTTP{
    /**
     * Egy egyedi kivétel, ami a státuszkódot és az ehhez tartozó üzenetet cipeli
     */
    class HTTPException : public std::exception
    {
        /**
         * A státusz kód
         */
        const std::string code;
        /**
         * Az üzenet
         */
        const std::string msg;
    public:
        /**
         * Egyszerű konstruktor
         * @param code a státusz kód
         * @param msg az üzenet
         */
        HTTPException(std::string &&code, std::string &&msg): code{code}, msg{msg} {}

        /**
         * Visszatér a kivétel leírásával
         * @return a kivétel leírása
         */
        const char * what() const noexcept
        {
            return (std::string("HTTP Exception ")+code+msg).c_str();
        }

        /**
         * Getter a státusz kódhoz
         * @return a státusz kód
         */
        const std::string & getCode() const {
            return code;
        }

        /**
         * Getter az üzenethez
         * @return az üzenet
         */
        const std::string & getMsg() const {
            return msg;
        }
    };
}

#endif //LINPROGHAZI_EXCEPTION_H
