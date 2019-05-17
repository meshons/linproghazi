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

struct Config {
    int maxConnections;
    size_t requestMaxSize;
    std::string PHPPath;
    std::string webPath;

    explicit Config(const std::string &file);
private:
    void loadConfig(const std::string & name, const std::string &value);
};


#endif //LINPROGHAZI_CONFIG_H
