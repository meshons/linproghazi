#ifndef LINPROGHAZI_CONFIG_H
#define LINPROGHAZI_CONFIG_H

#include <cstddef>
#include <string>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include "mime.h"

/**
 * A beállításokat tároló struct a config.txt-ből
 */
struct Config {
    int maxConnections; /// maximális kapcsolatok száma
    size_t requestMaxSize; /// a kérések maximális mérete bájtban
    std::string PHPPath; /// a php fájlnak az elérési útvonala
    std::string webPath; /// a webszerver könyvtárának elérési útvonala
    std::string defaultFileName; /// az alapértelmezett fájl neve a könyvtárban
    std::string errorFileName; /// az error fájl neve
    std::string mimeTypesPath; /// a MIME tipusokat tároló fájl elérési útvonala
    std::string port = "80"; /// a használandó port
    mime mimeHandler; /// a mimeHandler-e a webszervernek

    /**
     * A config konstruktora, átveszi a config fájl nevét
     * @param file a config file neve
     */
    explicit Config(const std::string &file);
private:
    /**
     * Betölti az adott nevű beállítást az adott értékkel
     * @param name a beállítás neve
     * @param value a beállítás értéke
     */
    void loadConfig(const std::string & name, const std::string &value);
};


#endif //LINPROGHAZI_CONFIG_H
