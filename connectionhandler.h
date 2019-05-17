#ifndef LINPROGHAZI_CONNECTIONHANDLER_H
#define LINPROGHAZI_CONNECTIONHANDLER_H

#include <sys/socket.h>
#include "config.h"

struct ConnectionHandler{
    int socket;
    sockaddr src_addr;
    socklen_t addrlen;
    char * msg;
    size_t msg_size;
    std::shared_ptr<Config> cfg;

    explicit ConnectionHandler(std::shared_ptr<Config> cfg): msg_size{cfg->requestMaxSize}, cfg{cfg} {
        msg = new char[cfg->requestMaxSize];
    }

    ~ConnectionHandler() {
        delete[] msg;
    }
};

#endif //LINPROGHAZI_CONNECTIONHANDLER_H
