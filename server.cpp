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

std::vector<userInformation> userList;
bool run = true;

// Esta funcion servira para cuando querramos buscar un usuario especifico dentro de la lista de usuarios
int findUser(string name){
    for (int i = 0; i<userList.size(); i++){
        if (userList[i].name == name){
            return i;
        }
    }
}
// user -> Usuario que envia el mensaje
// name -> Usuario que recibe el mensaje
// text -> El mensaje que se envio
// TODO probar
void privateChat(string user, string name, string text){
    chat::ServerResponse response;
    //chat::ClientRequest request;
    chat::Message *message = response.mutable_messg(); 
    char buf[4096];
    int sock_dest = userList[findUser(name)].socket;
    message -> set_sender(user);
    message -> set_text(text);
    message -> set_receiver(name);
    response.set_option(chat::ServerResponse_Option_SEND_MESSAGE);
    response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);

    std::string response_serialized;
    response.SerializeToString(&response_serialized);

    

    strcpy(buf, response_serialized.c_str());
    send(sock_dest, buf, response_serialized.size()+1, 0);
}
// user -> Usuario que envia el mensaje
// text -> El mensaje que se envio
// TODO probar
void generalChat(string user, string text){
    
    for (int i = 0; i < userList.size(); i++){
        chat::ServerResponse response;
        //chat::ClientRequest request;
        //chat::Message *message = response.mutable_messg();
        //std::string response_serialized; 
        int sock;
        response.mutable_messg() -> set_sender(user);
        response.mutable_messg() -> set_text(text);
        response.mutable_messg() -> set_receiver("all");//userList[i].name
        response.set_option(chat::ServerResponse_Option_SEND_MESSAGE);
        response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
        char buf[4096];
        //userList[i].name

        cout<<"USUarios en general"<<userList[i].name<<endl;
        sock = userList[i].socket;
        std::string response_serialized; 
        
        response.SerializeToString(&response_serialized);
        strcpy(buf, response_serialized.c_str());
        send(sock, buf, response_serialized.size()+1, 0);
    }
}
// TODO probar
// user -> Usuario a quien se le envia el mensaje (all es para todos los usurios)
// name -> Usuario quien envio el mensaje
// text -> Mensaje
void messageChat (chat::ClientRequest request){
    if (request.messg().receiver() == "all"){
        generalChat(request.messg().sender(), request.messg().text());
    } else {
        privateChat(request.messg().sender(),request.messg().receiver(), request.messg().text());
    }
}
// TODO
// void connectedUsers(int sock){
//     chat::ServerResponse response;
//     chat::ClientRequest request;
//     std::string response_serialized;
//     char buf[4096];
// if(request.user().has_user() != true){
//         response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
//         chat::ConnectedUsers *allUsers = new chat::ConnectedUsers();
//         for (auto item = userList.begin(); item!=userList.end(); item++){
//             chat::UserInformation * user = allUsers->add_users();
//             user->set_username(item->name);
//             user->set_ip(item->ip);
//             user->set_status(item->status);
//         }
//         response.Clear();
//         response.set_option(response.CONNECTED_USERS);
//         response.set_allocated_users(allUsers);
//         response.SerializeToString(&response_serialized);
//         strcpy(buf, response_serialized.c_str());
//         send(sock, buf, response_serialized.size() + 1, 0);
//     }
// }
// user -> Usuario a quien buscar
// sock -> Socket del usuario que hizo la peticion
void getUser(string user, int sock){
    //chat::ClientRequest request;
    chat::ServerResponse response;
    chat::UserInformation *userRequest = response.mutable_user();
    char buf[4096];
    userInformation information = userList[findUser(user)];
    userRequest -> set_username(information.name);
    userRequest -> set_ip(information.ip);
    userRequest -> set_status(information.status);
    response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
    response.set_option(chat::ServerResponse_Option_USER_INFORMATION);

    std::string response_serialized; 
    response.SerializeToString(&response_serialized);
    
    strcpy(buf, response_serialized.c_str());
    send(sock, buf, response_serialized.size()+1, 0);
}

void getConnectedUser(int sock){
    chat::ServerResponse response;
    chat::ConnectedUsers *connectedUsers = response.mutable_users();
    char buf[4096];
    for (int i = 0; i < userList.size(); i++){
        chat::UserInformation *userRequest = connectedUsers -> add_users();
        userRequest->set_username(userList[i].name);
        userRequest->set_ip(userList[i].ip);
        userRequest->set_status(userList[i].status);
    }

    response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
    response.set_option(chat::ServerResponse_Option_CONNECTED_USERS);

    std::string response_serialized; 
    response.SerializeToString(&response_serialized);
    
    strcpy(buf, response_serialized.c_str());
    send(sock, buf, response_serialized.size()+1, 0);
}
void changeStatus(string name, string status){
    //chat::ClientRequest request;
    chat::ServerResponse response;
    //chat::ChangeStatus *estado = response.mutable_status();
    char buf[4096];
    int sock = userList[findUser(name)].socket;
    response.mutable_status() -> set_status(status.c_str());
    response.mutable_status() -> set_username(name.c_str());
    response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
    response.set_option(chat::ServerResponse_Option_STATUS_CHANGE);
    
    
    
    userList[findUser(name)].status = status.c_str();

    printf("ELESTADO%s\n", status.c_str());

    std::string response_serialized; 
    response.SerializeToString(&response_serialized);
    //cout << userList[0].name << endl;
    strcpy(buf, response_serialized.c_str());
    send(sock, buf, response_serialized.size()+1, 0);
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
    hint.sin_port = htons(54005);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    bind(listening, (sockaddr*)&hint, sizeof(hint));
 
    // Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);
    return listening;
}


void* clientConnection (void *args){
    userInformation *user = (userInformation*) args;
    userInformation userUpdated;
    chat::ServerResponse response;
    int clientSocket = user -> socket;
    sockaddr_in client = user -> client;
    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on
    char bufClient[8192];
    char bufClient2[8192];
 
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
    if (recv(clientSocket, bufClient, 8192, 0) > 0){
        response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
    } else {
        response.set_code(chat::ServerResponse_Code_FAILED_OPERATION);
    }
    chat::ClientRequest initrequest;
    initrequest.ParseFromString(bufClient);
    userUpdated.name = initrequest.newuser().username();
    userUpdated.ip = initrequest.newuser().ip();
    userUpdated.socket = clientSocket;//(user -> socket)+0;
    userUpdated.client = user -> client;
    userUpdated.status = "Online";
    userList.push_back(userUpdated);
    std::string response_serialized; 
    response.SerializeToString(&response_serialized);
    strcpy(bufClient, response_serialized.c_str());
    send(clientSocket, bufClient, response_serialized.size(), 0);
    while (true){
        if (recv(clientSocket, bufClient2, 8192, 0) > 0){
            response.set_code(chat::ServerResponse_Code_SUCCESSFUL_OPERATION);
        } else {
            response.set_code(chat::ServerResponse_Code_FAILED_OPERATION);
            break;
        }
        std::string response_serialized; 
        response.SerializeToString(&response_serialized);
        strcpy(bufClient, response_serialized.c_str());

        send(clientSocket, bufClient2, response_serialized.size(), 0);
        chat::ClientRequest initrequest;
    
        initrequest.ParseFromString(bufClient2);

        cout << initrequest.option() << endl;
        if (initrequest.option() == chat::ClientRequest_Option_CONNECTED_USERS){
            cout << "Se escogio la opcion CONNECTED_USERS" << endl;
        } else if (initrequest.option() == chat::ClientRequest_Option_USER_INFORMATION){
            cout << "Se escogio la opcion USER_INFORMATION" << endl; //half done
            getUser(initrequest.user().user().c_str(), clientSocket); //userupdated.socket si funciona


        } else if (initrequest.option() == chat::ClientRequest_Option_STATUS_CHANGE){
            cout << "Se escogio la opcion STATUS_CHANGE" << endl;
            changeStatus(initrequest.status().username(), initrequest.status().status());
        } else if (initrequest.option() == chat::ClientRequest_Option_SEND_MESSAGE){

            cout << "Se escogio la opcion SEND_MESSAGE" << endl;
            messageChat(initrequest);
        }
    }
    close(userUpdated.socket);
    pthread_exit(nullptr);
}   

int main()
{
    
    // Wait for a connection
    int listening = init();
    int number = 2;
    int rc;
    pthread_t tid;
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    string userInput;
    auto* user = (userInformation *) malloc(sizeof(userInformation));
    while (run){
        
        int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        user-> socket = clientSocket;
        user-> client = client;


        pthread_create(&tid, &attrs, clientConnection,(void *)user);
        cout << "Se ha conectado un nuevo usuario " << clientSocket << endl;
    }
 
    return 0;
}