// Autor: Alejandro Martín de León
// Curso: 2019/2020
// Asignatura: Sistemas Operativos
// Primera Práctica de Sockets
// Envío del contenido de un fichero desde una terminal send hacia una terminal receive
/////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

struct Message {        // Estructura del mensaje

    std::array<char, 1024> text;
};

sockaddr_in make_ip_address(const std::string& ip_address, int port);

class Socket {
    public: 
        Socket(const sockaddr_in& address);
        ~Socket();

        void send_to(const Message& message, const sockaddr_in& address);
        void receive_from(Message& message, sockaddr_in& address);

    private:

        int fd_;
};