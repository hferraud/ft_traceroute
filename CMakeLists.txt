cmake_minimum_required(VERSION 3.29)
project(traceroute C)

set(CMAKE_C_STANDARD 11)

include_directories(include/)

add_executable(traceroute
        src/main.c
        src/parser.c
        src/socket.c
        src/icmp.c
        src/traceroute.c
        src/udp.c
        src/time.c
        src/print.c
)
