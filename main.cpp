#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <array>
#include <arpa/inet.h>


#include "socket.hpp"


int main() {

    
    
    sockaddr_in local_ip;
    sockaddr_in dest_ip;

    Socket socket(local_ip);

    

    local_ip = make_ip_address("127.0.0.0", 2000);
    dest_ip = make_ip_address("127.0.0.0", 2001);
    std::string message_text("Hey loco como estas todo bien o que? SI me leistes hasta aqui todo chachi");

    Message msg;
    message_text.copy(msg.text.data(), msg.text.size() - 1, 0);
    socket.send_to(msg, dest_ip);


    sockaddr_in remote_address{};    // Porque se recomienda inicializar a 0
    socklen_t src_len = sizeof(remote_address);

    Message message;
    
    socket.receive_from(message, local_ip);
    
    char *remote_ip = inet_ntoa(remote_address.sin_addr);
    int remote_port = ntohs(remote_address.sin_port);
    message.text[1023] = '\0';
    std::cout << "El sistema " << remote_ip << ":" << remote_port << "envió el mensjae " << message.text.data() << "\n";
    
}