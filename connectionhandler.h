#ifndef LINPROGHAZI_CONNECTIONHANDLER_H
#define LINPROGHAZI_CONNECTIONHANDLER_H

#include <sys/socket.h>
#include "config.h"

/**
 * A kapcsolatot kezelő struct
 * itt tárolódik a socket, a cím és az üzenet
 * továbbá hordozza a beállítást.
 */
struct ConnectionHandler{
    int socket; /// a fogadó socket
    sockaddr src_addr; /// a küldő socket címe
    socklen_t addrlen; /// a küldő socket címének hossza
    char * msg; /// a fogadott üzenet
    size_t msg_size; /// a fogadott üzenet hossza
    std::shared_ptr<Config> cfg; /// a beállítás

    /**
     * A konstruktora, ami lefoglalja az üzenetnek a maximális méretét
     * @param cfg a beállítások
     */
    explicit ConnectionHandler(std::shared_ptr<Config> cfg): msg_size{cfg->requestMaxSize}, cfg{cfg} {
        msg = new char[cfg->requestMaxSize];
    }

    /**
     * destruktor, felszabadítja az üzenetet
     */
    ~ConnectionHandler() {
        delete[] msg;
    }
};

#endif //LINPROGHAZI_CONNECTIONHANDLER_H
