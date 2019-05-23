#ifndef LINPROGHAZI_PHPCALLER_H
#define LINPROGHAZI_PHPCALLER_H

#include <map>
#include <string>

class phpCaller {
    std::map<std::string, std::string> GET;
    std::map<std::string, std::string> POST;
public:
    phpCaller(std::string && get, std::string && post);
    std::string response() const;
};


#endif //LINPROGHAZI_PHPCALLER_H
