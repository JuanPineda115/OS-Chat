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
/* void sendMessageUsers(string name,string receiver, string message){
    chat::ServerResponse response;
    chat::Message mesg;
    mesg.set_receiver(receiver);
    mesg.set_sender(name);
    mesg.set_text(message);
    response.set_option(chat::ServerResponse_Option_SEND_MESSAGE);
    //response.set_allocated_messg(mesg);
    int userIndex = findUser(receiver);
    int clientSocket = userList[userIndex].socket;
    //send(clientSocket, response, sizeof(response), 0);
} */
// name -> El nombre del usuario que se desea buscar
/* void informationUser(string name){
    chat::ServerResponse response;
    chat::UserInformation information;
    userInformation user = userList[findUser(name)];
    information.set_ip(user.ip);
    information.set_username(user.name);
    information.set_status(user.status);

    response.set_option(chat::ServerResponse_Option_USER_INFORMATION);
    response.set_allocated_users(information);
    std::string information_serialized; 
    response.SerializeToString(&information_serialized);
    send(user.socket, information_serialized.c_str(), information_serialized.size(), 0);
} */
// name -> usuario que hace la solicitud
void connectedUsers(string name){
    chat::ServerResponse response;
    chat::ConnectedUsers connectedUsers;
    chat::UserInformation information;
    userInformation users;
    //int sock = userList[findUser(name)].socket;
    for (int i = 0; i<userList.size(); i++){
        information.set_username(userList[i].name);
        information.set_ip(userList[i].ip);
        information.set_status(userList[i].status);
        //connectedUsers.add_users(information);
    }
    //response.set_allocated_users(connectedUsers);
    // response.set_allocated_users("Orlando");
    response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
    std::string response_serialized; 
    response.SerializeToString(&response_serialized);
    //strcpy(buf, response_serialized.c_str());
    //send(sock, response_serialized.c_str(), response_serialized.size(), 0);
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
    hint.sin_port = htons(54001);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    bind(listening, (sockaddr*)&hint, sizeof(hint));
 
    // Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);
    return listening;
}
// name -> Usuario a quien se le cambiara el estado


void* clientConnection (void *args){
    struct userInformation *user = (struct userInformation*) args;
    struct userInformation userUpdated;
    chat::ServerResponse response;
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
    while (true){
        if (recv(clientSocket, bufClient, 8192, 0) > 0){
            response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
        } else {
            response.set_code(chat::ServerResponse_Code_FAILED_OPERATION);
        }
        std::string response_serialized; 
        response.SerializeToString(&response_serialized);
        send(clientSocket, response_serialized.c_str(), response_serialized.size(), 0);
        chat::ClientRequest initrequest;
        //initrequest = (ClientRequest)bufClient;
        //chat::ClientRequest initrequest = (chat::ClientRequest) bufClient;
        initrequest.ParseFromString(bufClient);
        // printf("%s\n", initrequest.newuser().username());
        std::cout << "Servidor: se recibió informacion de: "
                        << initrequest.newuser().username()
                        << std::endl;

        //std::find(userList.begin(), userList.end(), "Prueba")
        userUpdated.ip = initrequest.newuser().ip();
        userUpdated.socket = user -> socket;
        userUpdated.client = user -> client;
        // TODO colocarle el nombre del usuario
        chat::ClientRequest testRequest;    //Esta variable la voy a usar solo para hacer pruebas
        //userUpdated.name = initrequest.newuser().username();
        //userList.push_back(userUpdated);
        //cout <<initrequest.newuser().username()<<"@"<<userUpdated.ip << endl;
        cout << initrequest.option() << endl;
        if (initrequest.option() == chat::ClientRequest_Option_CONNECTED_USERS){
            cout << "Se escogio la opcion CONNECTED_USERS" << endl;
            //connectedUsers(initrequest.user().user());
        } else if (initrequest.option() == chat::ClientRequest_Option_USER_INFORMATION){
            cout << "Se escogio la opcion USER_INFORMATION" << endl;
            //informationUser(initrequest.user().user());
        } else if (initrequest.option() == chat::ClientRequest_Option_STATUS_CHANGE){
            cout << "Se escogio la opcion STATUS_CHANGE" << endl;
            recv(clientSocket, bufClient, 8192, 0);
        } else if (initrequest.option() == chat::ClientRequest_Option_SEND_MESSAGE){
            cout << "Se escogio la opcion SEND_MESSAGE" << endl;
        } else if (initrequest.option() == chat::ClientRequest_Option_USER_LOGIN){
            cout << "Se escogio la opcion USER_LOGIN" << endl;
        }
    }
    // Close listening socket
    // close(listening);

    // While loop: accept and echo message back to client
    //char buf[4096];
    /* while (true)
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

        cout << initrequest.newuser().username()<<"@"<<userUpdated.ip <<"> " <<string(buf, 0, bytesReceived) << endl;     //Aqui se muestra el mensaje que mando el usuario
 
        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }
 
    // Close the socket
    close(clientSocket); */
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