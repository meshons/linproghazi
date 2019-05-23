#include "server.h"

Server::Server(std::atomic_bool &running) : running{running} {
    config = std::make_shared<Config>(std::string{"config.txt"});

    mimeTypeHandler = mime(config->mimeTypesPath);

    if (pthread_attr_init(&connection_attr))
        throw std::runtime_error("can't init pthread attr");
    if (sigaddset(&sigset, 1) || sigaddset(&sigset,2) || sigprocmask(SIG_UNBLOCK, &sigset, NULL))
        throw std::runtime_error("can't set signal set");
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
        throw std::runtime_error("can't create server socket");

    memset(&server_addr, 0, sizeof(struct addrinfo));
    server_addr.ai_family = AF_UNSPEC;
    server_addr.ai_socktype = SOCK_STREAM;
    server_addr.ai_flags = AI_PASSIVE;
    server_addr.ai_protocol = 0;
    server_addr.ai_canonname = NULL;
    server_addr.ai_addr = NULL;
    server_addr.ai_next = NULL;

    addrinfo *result, *rp;
    int s = getaddrinfo(NULL, config->port.c_str(), &server_addr, &result);
    if (s != 0)
        throw std::runtime_error(std::string("getaddrinfo: ")+gai_strerror(s));

    for (rp = result;rp != NULL;rp = rp->ai_next) {
        server_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (server_socket == -1)
            continue;
        if (bind(server_socket, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(server_socket);
    }

    if (rp == nullptr){
        std::cout << strerror(errno) << std::endl;
        throw std::runtime_error("Could not bind");
    }

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("can't set flags on socket");

    if (listen(server_socket, config->maxConnections) == -1)
        throw std::runtime_error("can't listen on socket");

    free_connection_semaphore = sem_open("free_connections", O_CREAT, S_IRWXU, config->maxConnections);

    for (int i=0; i<config->maxConnections; ++i) {
        free_connection_handlers.push(std::make_shared<ConnectionHandler>(config));
        pthread_t con_thread;
        if(pthread_create(&con_thread, &connection_attr, connection_thread, (void *) this))
            throw std::runtime_error("can't create connection thread");
        free_connection_threads.push(con_thread);
    }

    busy_connection_threads = std::set<pthread_t>();

    if (pthread_mutex_init(&free_connection_threads_lock, nullptr))
        throw std::runtime_error("can't create mutex");
    if (pthread_mutex_init(&busy_connection_threads_lock, nullptr))
        throw std::runtime_error("can't create mutex");
    if (pthread_mutex_init(&free_connection_handlers_lock, nullptr))
        throw std::runtime_error("can't create mutex");
}

void Server::run() {
    if(pthread_create(&server_thread, &connection_attr, server, (void *) this))
        throw std::runtime_error("can't create server thread");
}

void Server::join() {
    void *return_value;
    sigval sv;
    sv.sival_int = 2;

    if (pthread_mutex_lock(&free_connection_threads_lock))
        throw std::runtime_error("can't lock mutex");
    while (!free_connection_threads.empty()) {
        if (pthread_sigqueue(free_connection_threads.front(), 2, sv))
            throw std::runtime_error("error on sending signal to connection thread");
        free_connection_threads.pop();
    }
    if (pthread_mutex_unlock(&free_connection_threads_lock))
        throw std::runtime_error("can't unlock mutex");

    if (pthread_mutex_lock(&busy_connection_threads_lock))
        throw std::runtime_error("can't lock mutex");
    for (auto & thread : busy_connection_threads)
        if (pthread_sigqueue(thread, 2, sv))
            throw std::runtime_error("error on sending signal to connection thread");
    if (pthread_mutex_unlock(&busy_connection_threads_lock))
        throw std::runtime_error("can't unlock mutex");

    pthread_join(server_thread, &return_value);
    if (pthread_mutex_destroy(&free_connection_threads_lock))
        throw std::runtime_error("can't destroy mutex");
    if (pthread_mutex_destroy(&busy_connection_threads_lock))
        throw std::runtime_error("can't destroy mutex");
    if (pthread_mutex_destroy(&free_connection_handlers_lock))
        throw std::runtime_error("can't destroy mutex");
}

void * Server::connection_thread(void * param) {
    auto srv = (Server *) param;
    while (srv->running) {
        siginfo_t signal;
        if (sigwaitinfo(&srv->sigset, &signal) == -1)
            continue;
        if (signal.si_signo == 2) return nullptr;

        auto connectionHandler = *(std::shared_ptr<ConnectionHandler> *)signal.si_value.sival_ptr;

        int size = recvfrom(
                connectionHandler->socket,
                connectionHandler->msg,
                srv->config->requestMaxSize-1,
                0,
                &connectionHandler->src_addr,
                &connectionHandler->addrlen
                );
        if (size == -1)
            std::cout << strerror(errno) << std::endl;
        connectionHandler->msg[size] = 0;
        connectionHandler->msg_size = size;

        Response response = Request::handle(connectionHandler);

        int sent_size = sendto(
                connectionHandler->socket,
                response.msg.c_str(),
                response.msg.length(),
                0,
                &connectionHandler->src_addr,
                connectionHandler->addrlen
                );
        if (sent_size == -1)
            std::cout << strerror(errno) << std::endl;

        if (pthread_mutex_lock(&srv->free_connection_handlers_lock))
            throw std::runtime_error("can't lock mutex");
        srv->free_connection_handlers.push(connectionHandler);
        if (pthread_mutex_unlock(&srv->free_connection_handlers_lock))
            throw std::runtime_error("can't unlock mutex");

        if (pthread_mutex_lock(&srv->busy_connection_threads_lock))
            throw std::runtime_error("can't lock mutex");
        srv->busy_connection_threads.erase(pthread_self());
        if (pthread_mutex_unlock(&srv->busy_connection_threads_lock))
            throw std::runtime_error("can't unlock mutex");

        if (pthread_mutex_lock(&srv->free_connection_threads_lock))
            throw std::runtime_error("can't lock mutex");
        srv->free_connection_threads.push(pthread_self());
        if (pthread_mutex_unlock(&srv->free_connection_threads_lock))
            throw std::runtime_error("can't unlock mutex");

        sem_post(srv->free_connection_semaphore);
    }
    return nullptr;
}

void * Server::server(void *param) {
    auto srv = (Server *) param;
    std::cout << "running" << std::endl;
    while (srv->running) {
        sockaddr socket_addr;
        socklen_t size = sizeof(socket_addr);
        int socket;
        if (sem_trywait(srv->free_connection_semaphore)==0)
            continue;

        if ((socket = accept(srv->server_socket, &socket_addr, &size)) == -1){
            sem_post(srv->free_connection_semaphore);
            continue;
        }

        if (pthread_mutex_lock(&srv->free_connection_threads_lock))
            throw std::runtime_error("can't lock mutex");
        pthread_t thread = srv->free_connection_threads.front();
        srv->free_connection_threads.pop();
        if (pthread_mutex_unlock(&srv->free_connection_threads_lock))
            throw std::runtime_error("can't unlock mutex");

        if (pthread_mutex_lock(&srv->busy_connection_threads_lock))
            throw std::runtime_error("can't lock mutex");
        srv->busy_connection_threads.insert(thread);
        if (pthread_mutex_unlock(&srv->busy_connection_threads_lock))
            throw std::runtime_error("can't unlock mutex");

        if (pthread_mutex_lock(&srv->free_connection_handlers_lock))
            throw std::runtime_error("can't lock mutex");
        std::shared_ptr<ConnectionHandler> ch = srv->free_connection_handlers.front();
        srv->free_connection_handlers.pop();
        if (pthread_mutex_unlock(&srv->free_connection_handlers_lock))
            throw std::runtime_error("can't unlock mutex");

        ch->addrlen = size;
        ch->src_addr = socket_addr;
        ch->socket = socket;

        sigval sv;
        sv.sival_ptr = &ch;

        if (pthread_sigqueue(thread, 1, sv))
            throw std::runtime_error("error on sending signal to connection thread");
    }
    return nullptr;
}

Server::~Server() {
    pthread_attr_destroy(&connection_attr);
    shutdown(server_socket, SHUT_RDWR);
    close(server_socket);
    sem_close(free_connection_semaphore);
    sem_unlink("free_connections");
}