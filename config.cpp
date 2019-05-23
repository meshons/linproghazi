#include "config.h"

Config::Config(const std::string &file) {
    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1)
        throw std::runtime_error("can't open config file");
    lseek(fd, 0, SEEK_END);
    long fsize = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);

    char *string = (char *)malloc(fsize+1);
    read(fd, string, fsize);

    std::istringstream iss(std::string{string});
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());

    if (results.size()%3)
        throw std::runtime_error("wrong config file format");

    for (int i=0; i<results.size()/3; ++i)
        loadConfig(results[i*3], results[i*3+2]);
}

void Config::loadConfig(const std::string & name, const std::string &value) {
    if (name == "maxConnections")
        maxConnections = std::stoi(value);
    else if (name == "requestMaxSize")
        requestMaxSize = std::stoi(value);
    else if (name == "PHPPath")
        PHPPath = value;
    else if (name == "webPath")
        webPath = value;
    else if (name == "errorFileName")
        errorFileName = value;
    else if (name == "mimeTypesPath")
        mimeTypesPath = value;
    else if (name == "defaultFileName")
        defaultFileName = value;
    else if (name == "port")
        port = value;
}