cmake_minimum_required(VERSION 3.10)
project(linproghazi)

set(CMAKE_CXX_STANDARD 17)

add_executable(linproghazi main.cpp server.cpp server.h request.cpp request.h response.h config.cpp config.h connectionhandler.h HTTP/method.h HTTP/exception.h)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(linproghazi Threads::Threads)
