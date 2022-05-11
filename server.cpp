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
#include <stdio.h>


using namespace std;
using namespace google::protobuf;
struct userInformation
    {
        string name;
        int socket;
        string ip;
        string status;
        sockaddr_in client;
    };
pthread_t tid;
std::vector<userInformation> userList;
bool run = true;

// Esta funcion servira para cuando querramos buscar un usuario especifico dentro de la lista de usuarios
int findUser(string name){
    for (int i = 0; i<userList.size(); i++){
        if (userList[i].name == name){
            //cout << "El usuario se encuentra en la posicion " << i << endl;
            return i;
        }
    }
}
// name -> El usuario que manda el mensaje
// receiver -> El usuario que recibe el mensaje
// message -> Mensaje que se envia
void connectedUsers(string name,string receiver, string message){
    chat::ServerResponse response;
    chat::Message mesg;
    mesg.set_receiver(receiver);
    mesg.set_sender(name);
    mesg.set_text(message);
    response.set_option(chat::ServerResponse_Option_SEND_MESSAGE);
    response.set_allocated_messg(mesg);
    int userIndex = findUser(receiver);
    int clientSocket = userList[userIndex].socket;
    //send(clientSocket, response, sizeof(response), 0);
}

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
    struct userInformation *user = (struct userInformation*) args;
    struct userInformation userUpdated;
    int clientSocket = user -> socket;
    sockaddr_in client = user -> client;
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
    char bufClient[8192];
 
    memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }
    recv(clientSocket, bufClient, 8192, 0);
    chat::ClientRequest initrequest;
    //initrequest = (ClientRequest)bufClient;
    //chat::ClientRequest initrequest = (chat::ClientRequest) bufClient;
    initrequest.ParseFromString(bufClient);
    // printf("%s\n", initrequest.newuser().username());
    std::cout << "Servidor: se recibió informacion de: "
                      << initrequest.newuser().username()
                      << std::endl;

    //std::find(userList.begin(), userList.end(), "Prueba")
    userUpdated.ip = host;
    userUpdated.socket = user -> socket;
    userUpdated.client = user -> client;
    // TODO colocarle el nombre del usuario
    chat::ClientRequest testRequest;    //Esta variable la voy a usar solo para hacer pruebas
    userUpdated.name = userUpdated.socket;
    userList.push_back(userUpdated);
    cout << userUpdated.name << endl;
    testRequest.set_option(chat::ClientRequest_Option_USER_INFORMATION);

    if (testRequest.option() == chat::ClientRequest_Option_CONNECTED_USERS){
        cout << "Se escogio la opcion CONNECTED_USERS" << endl;
    } else if (testRequest.option() == chat::ClientRequest_Option_USER_INFORMATION){
        cout << "Se escogio la opcion USER_INFORMATION" << endl;
    } else if (testRequest.option() == chat::ClientRequest_Option_STATUS_CHANGE){
        cout << "Se escogio la opcion STATUS_CHANGE" << endl;
    } else if (testRequest.option() == chat::ClientRequest_Option_SEND_MESSAGE){
        cout << "Se escogio la opcion SEND_MESSAGE" << endl;
    } else if (testRequest.option() == chat::ClientRequest_Option_USER_LOGIN){
        cout << "Se escogio la opcion USER_LOGIN" << endl;
    }
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
            run = false;
            break;
        }
        // TODO cambiar esto para que solo se lo cierre del lado del cliente y hacer uno para cerrar el servidor
        if (string(buf, 0, bytesReceived) == "exit"){
            send(clientSocket, "Terminando conexion...", bytesReceived + 1, 0);
            close(clientSocket);
        }

        // TODO cambiar esto para que muestre el nombre de cada usuario
        cout << userUpdated.name<< "> " <<string(buf, 0, bytesReceived) << endl;     //Aqui se muestra el mensaje que mando el usuario
 
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
    userInformation user;
    int number = 2;
    int rc;
    while (run){
        int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        user.socket = clientSocket;
        user.client = client;
        pthread_create(&tid, NULL, clientConnection,(void *)&user);
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