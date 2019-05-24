#ifndef LINPROGHAZI_PHPCALLER_H
#define LINPROGHAZI_PHPCALLER_H

#include <map>
#include <memory>
#include <string>
#include "config.h"

class phpCaller {
    std::string file;
    std::map<std::string, std::string> GET;
    std::map<std::string, std::string> POST;
    std::shared_ptr<Config> cfg;
public:
    phpCaller(const std::string & get,
            const std::string & post,
            std::shared_ptr<Config> cfg,
            std::string file);
    std::string response() const;
private:
    std::map<std::string, std::string> splitParam(const std::string & get) const;
};


#endif //LINPROGHAZI_PHPCALLER_H
