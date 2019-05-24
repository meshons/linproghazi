#ifndef LINPROGHAZI_SERVER_H
#define LINPROGHAZI_SERVER_H

#include <iostream>
#include <memory>
#include <atomic>
#include <queue>
#include <set>
#include <cstring>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <semaphore.h>

#include "config.h"
#include "request.h"
#include "response.h"
#include "connectionhandler.h"
#include "mime.h"

/**
 * A webszerver osztálya
 * Tárolja a kapcsolatokat, az azokat lekezelő szálakat
 * és minden beállítást
 */
class Server {
    std::atomic_bool &running; /// a szerver futását szabályozó bool
    std::shared_ptr<Config> config; /// a szerver beállítása
    pthread_mutex_t free_connection_threads_lock; /// a szabad kapcsolat szálak mutex-e
    std::queue<pthread_t> free_connection_threads; /// a szabad kapcsolat szálak sora
    pthread_mutex_t busy_connection_threads_lock; /// a foglalt kapcsolat szálak mutex-e
    std::set<pthread_t> busy_connection_threads; /// a foglalt kapcsolat szálak
    pthread_t server_thread; /// a webszerver futási szála
    pthread_attr_t connection_attr; /// a szálak paraméterei
    sem_t * free_connection_semaphore; /// a még felhasználható kapcsolatok szemafor-a
    pthread_mutex_t free_connection_handlers_lock; /// a szabad kapcsolatkezelők mutex-e
    std::queue<std::shared_ptr<ConnectionHandler>> free_connection_handlers; /// a szabad kapcsolatkezelők
    sigset_t sigset; /// a signal-oknak a set-je
    int server_socket; /// a szervernek a fogadó socket-je
    addrinfo server_addr; /// a szerver címének információi
    mime mimeTypeHandler; /// a MIME típuskezelő
public:
    /**
     * Létrehozza szervert, betölti a beállításokat.
     * Config fájl betöltése, majd MIME kezelő betöltése
     * szálak előkészítése
     * fogadó socket előkészítése
     * szálak elindítása
     * mutex-ek előkészítése
     * a signal-ok előkészítése
     * @param running a szerver futását jelző változó
     */
    explicit Server(std::atomic_bool & running);
    /**
     * A webszerver futtatása, ekkor kezdődik a kapcsolatfogadás
     */
    void run();
    /**
     * A webszerver összes szálának befejezésének megvárása
     * mutexek felszabadítása.
     * Ez után a webszerver nem indítható újra.
     */
    void join();

private:
    /**
     * A kapcsolatok fogadásának szála.
     * Signal-ra vár, majd ha kapott, akkor fogadja a kapcsolatot,
     * választ ad rá és zárja a kapcsolatot
     * @param param a webszerver pointere
     * @return
     */
    static void * connection_thread(void * param);
    /**
     * A webszerver szála.
     * Itt fogadja a kapcsolatok és adja át a kapcsolatkezelőknek,
     * majd jelez egy szabad szálnak átadva a jelzésben a kapcsolatkezelőt.
     * @param param a webszerver pointere
     * @return
     */
    static void * server(void * param);
public:
    /**
     * A destruktor felszabadít és zárja a kapcsolatokat
     */
    ~Server();
};

#endif //LINPROGHAZI_SERVER_H
