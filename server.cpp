/* Universidad del Valle de Guatemala
Sistemas operativos
Proyecto: Chat
Por:
Orlando Cabrera
Jose Javier Hurtarte
Juan Pablo Pineda */
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include "register.pb.h"

using namespace std;
using namespace google::protobuf;
struct userInformation
    {
        string name;
        int socket;
        string ip;
        string status;
    };
pthread_t tid;

int init () {
     // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return 0;
    }
 
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    bind(listening, (sockaddr*)&hint, sizeof(hint));
 
    // Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);
    return listening;
}

void* clientConnection (void *args){
    int clientSocket = (int) args;
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
 
    memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
 
    /* if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    } */
 
    // Close listening socket
    // close(listening);
 
    // While loop: accept and echo message back to client
    char buf[4096];
    while (true)
    {
        memset(buf, 0, 4096);
 
        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }
        // TODO cambiar esto para que solo se lo cierre del lado del cliente y hacer uno para cerrar el servidor
        if (string(buf, 0, bytesReceived) == "exit"){
            send(clientSocket, "Terminando conexion...", bytesReceived + 1, 0);
            close(clientSocket);
        }

        // TODO cambiar esto para que muestre el nombre de cada usuario
        cout << host<< "> " <<string(buf, 0, bytesReceived) << endl;     //Aqui se muestra el mensaje que mando el usuario
 
        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }
 
    // Close the socket
    close(clientSocket);
}   

int main()
{
    
    // Wait for a connection
    int listening = init();
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    string userInput;
    int number = 2;
    bool run = true;
    int rc;
    //while (run){
    //    cout << "Haciendo pruebas" << endl;
    //    getline(cin, userInput);
    //    if (userInput == "exit"){
    //        return 0;
    //    }
    //    rc = pthread_create(&tid, NULL, clientConnection,nullptr);
    //    if(rc == 0)			/* could not create thread */
    //    {
    //        printf("\n ERROR: return code from pthread_create is %d \n", rc);
    //        exit(1);
    //    }  
    //}
    while (run){
        int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        pthread_create(&tid, NULL, clientConnection,(void *)clientSocket);
        cout << "Se ha conectado un nuevo usuario" << endl;
    }
 
    return 0;
}




// Cosas que me serviran mas adelante
/* std::vector<userInformation> userList;
    userInformation user;
    chat::ServerResponse response;
    chat::ServerResponse *response2 = new chat::ServerResponse();
    chat::UserRegistration *reg= new chat::UserRegistration();;
    chat::ClientRequest request;
    user.name="Orlando";
    user.socket=123;
    user.ip="123";
    user.status="on";
    reg -> set_username("Orlando");
    reg -> set_ip("123");
    request.set_allocated_newuser(reg);   
    //user.name = "Orlando";
    //user.socket = 123;
    //user.ip = "195";
    //user.status = "on";
    //userList.push_back(user);
    //cout << userInformation.username() << endl;
    //cout << request.has_newuser() << endl; */