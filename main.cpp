#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <array>
#include <arpa/inet.h>
#include <thread> // Libreria de hilos
#include <exception> // libreria de excepciones

#include <cerrno>
#include <cstring>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h> // Cancelacion de hilos

#include <atomic>
#include <csignal> 
#include <unistd.h>        // para getopt(), optarg, optind, …
#include <vector>
#include <set>


#include "socket.hpp"

std::string comando;
sockaddr_in local_ip;
sockaddr_in dest_ip;

// FLAGS
std::atomic<bool> quit(false);

std::set<std::pair<int, std::thread *>> active_threads;

struct CommandLineArguments {
    bool show_help = false;
    bool server_mode = false;
    bool client_mode = false;
    int conn_port = 0;
    std::string conn_ip;
    std::vector<std::string> other_arguments;

CommandLineArguments(int argc, char* argv[]);
};

void thread_send(std::exception_ptr& eptr, int port, std::string ip_add, std::string comando) {     // Función de los hilos.

    int contador = 1;
    try {
        int contador = 1;
       // while (comando != "/quit") {
            //std::cin >> comando;
            //std::cout << "comando: " << comando;
            
            local_ip = make_ip_address("127.0.0.1", 5000);
            dest_ip = make_ip_address(ip_add, port);

            int contenido = 1;
            char buffer[1024]; 
            Message message;
            Socket socket(local_ip);
            
           
             //if (comando != "/quit") {
            int fd = open(comando.c_str(), O_RDONLY); // Abrimos el fichero para lectura y escritura 
            
            if( fd == -1 ) {
                std::cerr << "No se ha podido abrir el fichero " << std::strerror(errno) << '\n';
                //comando = "/quit";
            }
            
            
            while (contenido != 0) {
                
                contenido = read(fd, buffer, 1023);
                std::string string_contenido(buffer); // Convertimos el char * en string
                string_contenido.copy(message.text.data(), message.text.size() - 1, 0); // Copiamos los atributos del string hacia nuestra estructura Message

                socket.send_to(message, dest_ip);
               // socket.send_to(me)
                
            }
           // contador++;
            close(fd);
        //}
      // }
    }
    catch (const std::exception& e) {
        eptr = std::current_exception();
 
    }
    
}

void thread_recv(std::exception_ptr& eptr, int port) {     

    try {
        while (comando != "/quit") {
            
            local_ip = make_ip_address("127.0.0.1", 5001);
            dest_ip = make_ip_address("127.0.0.1", port);
           // std::cout << "Puerto: " << port << std::endl;

            Socket socket(local_ip);
            Message message;

            while(true){
                socket.receive_from(message, dest_ip);

                char *remote_ip = inet_ntoa(dest_ip.sin_addr); // Ip desde la que se ha enviado el mensaje
                int remote_port = ntohs(dest_ip.sin_port);
        
                std::cout << "El sistema " << remote_ip << ":" << remote_port << " envió el fichero:\n" << message.text.data() << std::endl;
            }
            
        }
    }
        
    catch (const std::exception& e) {
        eptr = std::current_exception();
 
    }
    
}

void request_cancellation(std::thread& thread) {

    pthread_cancel(thread.native_handle());
}

void int_signal_handler(int signum) {
    if (signum == SIGINT) {
        write(1, "¡Señal SIGINT interceptada!\n", 31);
        quit = true;
    }
    if (signum == SIGTERM) {
        write(1, "¡Señal SIGTERM, se ha apagado el sistema!\n", 45);
        quit = true;
    }
    if (signum == SIGHUP) {
        write(1, "¡Señal SIGHUP, se ha cerrado la terminal!\n", 45);
        quit = true;
    }
}


CommandLineArguments::CommandLineArguments(int argc, char *argv[]) {
 
    int c;
    while ((c = getopt(argc, argv, "hsc:p:01")) != -1) { // AÑADIR EL DEL CLIENTE
    
        switch (c) {
        
            case '0':
            
            case '1':
                std::cerr << "Opción: " << c << std::endl;
                break;

            case 'h':
                std::cerr << "--- HELP ---\nForma de ejecutar este programa: ./Netcp [-h ] [-p port] [-s ]\n-h: Muestra la ayuda\n-p: Seguido del argumento 'port' establece dicho número del puerto.\n-s: Inicia en modo servidor.\n";
                show_help = true;
                break;
            
            case 's':
                std::cerr << "opción s\n";
                server_mode = true;
                break;

            case 'c':
                std::cerr << "Opción c con ip: " << optarg << std::endl; 
                client_mode = true;
                conn_ip = std::string(optarg);

                break;
            
            case 'p':
                std::cerr << "Opción p con valor " << optarg << std::endl; 
                conn_port = std::atoi(optarg);
                
                break;
            
            case '?':
                throw std::invalid_argument("Argumento de línea de comandos desconocido.");
            
            default:
                throw std::runtime_error("Error procesando la línea de comandos");

           
        }
    }
       
    if (optind < argc) {
        std::cerr << "-- empezamos con los argumentos no opción --\n";
        for (; optind < argc; ++optind) {
            std::cerr << "argv[" << optind << "]: " << argv[optind] << '\n';
            other_arguments.push_back(argv[optind]);
        }
    }   
}

int protected_main(int argc, char* argv[]) {

   /*
    std::signal(SIGINT, &int_signal_handler);
    std::signal(SIGTERM, &int_signal_handler);
    std::signal(SIGHUP, &int_signal_handler);

    std::exception_ptr eptr1 {};
    std::exception_ptr eptr2 {};
    std::thread hilo_envio(&thread_send, std::ref(eptr1));
    std::thread hilo_recibir(&thread_recv, std::ref(eptr2));
    
    while(!quit);
    
    request_cancellation(hilo_envio);
    request_cancellation(hilo_recibir);

    hilo_envio.join();
    hilo_recibir.join();

    if (eptr1) {
         std::rethrow_exception(eptr1);
    }
    if (eptr2) {
        std::rethrow_exception(eptr1);
    } */
   // CommandLineArguments command;




    CommandLineArguments command(argc, argv);
    if(command.server_mode == true){
        
        //std::cout << "Puerto: " << command.conn_port << " Server: " << command.server_mode << std::endl;
        
        std::signal(SIGINT, &int_signal_handler);
        std::signal(SIGTERM, &int_signal_handler);
        std::signal(SIGHUP, &int_signal_handler);
        std::exception_ptr eptr1 {};

        
            std::thread hilo_recibir(&thread_recv, std::ref(eptr1), std::ref(command.conn_port));
            while(!quit);
            request_cancellation(hilo_recibir);
            hilo_recibir.join();

              
          
           
        


//        while(!quit);

       
        if (eptr1) {
         std::rethrow_exception(eptr1);
        }

    }

    if(command.client_mode == true){

        std::signal(SIGINT, &int_signal_handler);
        std::signal(SIGTERM, &int_signal_handler);
        std::signal(SIGHUP, &int_signal_handler);
        std::exception_ptr eptr2 {};
       // std::thread hilo_envio(&thread_send, std::ref(eptr2), std::ref(command.conn_port), std::ref(command.conn_ip));

        while(!quit) {
            std::string filename;
            std::cin >> filename;
            std::string aux = filename;
            
            if(aux != "/quit") {
                std::thread* puntero_hilo = new std::thread(&thread_send, std::ref(eptr2), std::ref(command.conn_port), std::ref(command.conn_ip), std::ref(aux));
            }else{
                quit = true;
            }
        }
/*
        request_cancellation(hilo_envio);
        hilo_envio.join();
*/
        if (eptr2) {
         std::rethrow_exception(eptr2);
        }

    }

}   


int main(int argc, char* argv[]) {

    try {
        
        //CommandLineArguments arguments(argc, argv);
        return protected_main(argc, argv);
    }

    catch(std::bad_alloc& e) {
        std::cerr << "mytalk" << ": memoria insuficiente\n";
        return 1;
    }

    catch(std::system_error& e) {
        std::cerr << "mytalk" << ": " << e.what() << '\n';
        return 2;
    }
    /*
    catch (...) {
        std::cout << "Error desconocido\n";
        return 99;
    }*/
    return 0;
}