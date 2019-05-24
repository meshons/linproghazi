#ifndef LINPROGHAZI_PHPCALLER_H
#define LINPROGHAZI_PHPCALLER_H

#include <map>
#include <memory>
#include <string>
#include "config.h"

/**
 * A PHP hívásokat segítő osztály
 */
class phpCaller {
    std::string file; /// a futtatandó php fájl neve
    std::map<std::string, std::string> GET; /// a GET paraméterek mapje
    std::map<std::string, std::string> POST; /// a POST paraméterek mapje
    std::shared_ptr<Config> cfg; /// a beállítások
public:
    /**
     * A konstruktor a get és post stringeket feldarabolja és eltárolja
     * a GET-ben és POST-ban
     * @param get a GET paraméterek listája
     * @param post a POST paraméterek listája
     * @param cfg a beállítások
     * @param file a futtatandó php fájl neve
     */
    phpCaller(const std::string & get,
            const std::string & post,
            std::shared_ptr<Config> cfg,
            std::string file);
    /**
     * A futtatott php fájl által generált kimenetet adja vissza
     * @return a php fájl által generált kimenet
     */
    std::string response() const;
private:
    /**
     * Segédfüggvény a GET és POST kinyeréséhez.
     * Az adott stringet feldarabolja és visszaad egy MAP-et, amiben tárolja
     * @param get a string, amit fel kell darabolnia
     * @return a már elkészített map a stringből
     */
    std::map<std::string, std::string> splitParam(const std::string & get) const;
};

#endif //LINPROGHAZI_PHPCALLER_H
