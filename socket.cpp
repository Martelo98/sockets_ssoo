// Autor: Alejandro Martín de León
// Curso: 2019/2020
// Asignatura: Sistemas Operativos
// Primera Práctica de Sockets
// Envío del contenido de un fichero desde una terminal send hacia una terminal receive
/////////////////////////////////////////////////////////////////////////////////////////
//
// Método de compilacion explicado en ambos mains: receive.cpp y send.cpp

#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <array>
#include <arpa/inet.h>
#include <string>
#include <cerrno>
#include <cstring>



#include "socket.hpp"

sockaddr_in make_ip_address(const std::string& ip_address, int port) {

    sockaddr_in local_address{};    
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(port);
    if (ip_address == "") {
        local_address.sin_addr.s_addr = htonl(INADDR_ANY); 
    }
    else {
        inet_aton(ip_address.data(), &local_address.sin_addr);
    }
    
    return local_address;

}

Socket::Socket(const sockaddr_in& address) {

    fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) {
        std::cerr << "No se pudo crear el socket" << std::strerror(errno) << '\n';
    }
    
    int result = bind(fd_, reinterpret_cast<const sockaddr*>(&address),sizeof(address));
    if (result < 0) {
        std::cerr << "Falló el bind" << std::strerror(errno) << '\n';
    }

}

Socket::~Socket() {
    close(fd_);
}

void Socket::send_to(const Message& message, const sockaddr_in& address) {

    int result = sendto(fd_, &message, sizeof(message), 0, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
    if(result < 0) {
        std::cerr << "Falló el send to" << std::strerror(errno) << '\n';
        //return 6;
    }
}

void Socket::receive_from(Message& message, sockaddr_in& address) {

    socklen_t src_len = sizeof(address);

    int result = recvfrom(fd_, &message, sizeof(message), 0, reinterpret_cast<sockaddr*>(&address), &src_len);
    if (result < 0){
      std::cerr << "Falló el receive to" << std::strerror(errno) << '\n';

    }
}