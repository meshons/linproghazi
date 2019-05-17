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

class Server {
    std::atomic_bool &running;
    std::shared_ptr<Config> config;
    pthread_mutex_t free_connection_threads_lock;
    std::queue<pthread_t> free_connection_threads;
    pthread_mutex_t busy_connection_threads_lock;
    std::set<pthread_t> busy_connection_threads;
    pthread_t server_thread;
    pthread_attr_t connection_attr;
    sem_t * free_connection_semaphore;
    pthread_mutex_t free_connection_handlers_lock;
    std::queue<std::shared_ptr<ConnectionHandler>> free_connection_handlers;
    sigset_t sigset;
    int server_socket;
    addrinfo server_addr;
public:
    explicit Server(std::atomic_bool & running);
    void run();
    void join();

private:
    static void * connection_thread(void * param);
    static void * server(void * param);
public:
    ~Server();
};

#endif //LINPROGHAZI_SERVER_H
