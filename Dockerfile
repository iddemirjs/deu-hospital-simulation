FROM debian:latest AS env

RUN echo 'deb http://deb.debian.org/debian testing main' >> /etc/apt/sources.list
RUN apt update -y
RUN apt install -y gcc

COPY . /usr/src/c_thread
WORKDIR /usr/src/c_thread

CMD gcc main.c -o run -lpthread && ./run