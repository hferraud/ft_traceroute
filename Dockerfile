FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    valgrind \
    lldb \
    wget

RUN apt-get install inetutils-traceroute -y

WORKDIR /traceroute
