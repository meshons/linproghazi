#ifndef LINPROGHAZI_REQUEST_H
#define LINPROGHAZI_REQUEST_H

#include <memory>
#include <iomanip>

#include "response.h"
#include "connectionhandler.h"
#include "HTTP/method.h"
#include "HTTP/exception.h"
#include "HTTP/content.h"

/**
 * A kéréseket képviselő osztály
 */
class Request {
public:
    /**
     * A kérések kezelésének segédfüggvénye, ami már a válasszal tér vissza
     * egy megadott már létrejött kapcsolat paraméterül kapásával.
     * @param ch a bejövő kapcsolat
     * @return az elkészült válasz
     */
    static Response handle(const std::shared_ptr<ConnectionHandler> & ch);
private:
    std::shared_ptr<ConnectionHandler> ch; /// a létrejött kapcsolat
    HTTP::method method; /// a kérés metódusának neve
    std::string url; /// a kérés címe, dinamikusan feldolgozódik
    bool simple; /// HTTP/0.9-es kérés-e
    std::time_t * ifModifiedSince = nullptr; /// módosult-e azóta
    HTTP::contentCoding contentCoding; /// a kérés kódolása
    std::string get; /// a get paraméterlista
    std::string post; /// a post paraméterlista
public:
    /**
     * A konstruktora beállítja a fogadott kapcsolatot.
     * @param ch a fogadott kapcsolat
     */
    explicit Request(std::shared_ptr<ConnectionHandler> ch):ch{std::move(ch)} {
        method = HTTP::method::GET;
    }
    /**
     * Felszabadít a destruktorban.
     */
    ~Request() {
        delete ifModifiedSince;
    }
private:
    /**
     * Hibaüzenetet kreáló segédfüggvény
     * @param e a kivétel ami kiváltotta
     * @return a válasz
     */
    Response errorResponse(const HTTP::HTTPException &e) const;
    /**
     * A kérést felhasználva választ készít
     * @return a kérésre adott válasz
     */
    Response createResponse() const;
    /**
     * A kérés első sorát boncoló segédfüggvény
     * @param line a kérés első sora
     */
    void extractRequestLine(const std::string &line);
    /**
     * A kérés header-jét boncoló segédfüggvény
     * @param lines a kérés sorai
     */
    void extractHeader(std::stringstream &lines);
    /**
     * A kérés body-ját boncoló segédfüggvény
     * @param lines a kérés sorai
     */
    void extractPostBody(std::stringstream &lines);

    /**
     * A kérés url-jét boncoló segédfüggvény
     */
    void extractUrl();
};

#endif //LINPROGHAZI_REQUEST_H
