#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include "register.pb.h"

using namespace std;

void showMenu()
{
    printf("\n\n\n");
    printf( "Welcome to the chat room!\r\n");
    printf( "1. Obtener usuarios conectados\r\n");
    printf( "2. Mostrar información de un usuario\r\n");
    printf( "3. Cambiar estatus\r\n");
    printf( "4. Enviar un mensaje general\r\n");
    printf( "5. Enviar un mensaje directo\n");
    printf( "6. Ver mensajes del chat\n");
    printf( "7. Salir\r\n");
}


int main()
{
    //	Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return 1;
    }
    //	Create a hint structure for the server we're connecting with
    int port = 54001;
    string ipAddress = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    chat::ServerResponse response;
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    //	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        return 1;
    }

    //	While loop:
    char buf[4096];
    string userInput;
    
    string option;

    //para opcion 2
    string user_info;

    //para opcion 3
    string status_option, new_status;

    

    //para opcion 4
    string general_message;

    //para opcion 5
    string direct_message, direct_user;

    

    //TODO: Hacer que el primer argumento sea el usuario
    string name;
    cout << "Ingrese su nombre de usuario" << endl;
    cin >> name;

    //get the current ip address of the client and store it in a string
    string ip = "";
    char hostname[1024];
    gethostname(hostname, 1024);
    struct hostent *host = gethostbyname(hostname);
    if (host)
    {
        ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
    }



    chat::ClientRequest setUsername;
    setUsername.set_option(chat::ClientRequest_Option_USER_LOGIN);

    chat::UserRegistration* user = setUsername.mutable_newuser();
    user->set_username(name);
    user->set_ip(ip);
    // setUsername.mutable_newuser() ->set_username(name);
    // setUsername.mutable_newuser() ->set_ip("127.1.1.1");

    std::string login_serialized; 
    setUsername.SerializeToString(&login_serialized);

    //setUsername.SerializeToString(&login_serialized);

    strcpy(buf, login_serialized.c_str());
    send(sock, buf, login_serialized.size()+1, 0);

    //    Receive the response
    recv(sock, buf, 4096, 0);

    chat::ServerResponse loginresponse;
    loginresponse.ParseFromString(buf);

    if(loginresponse.code() == chat::ServerResponse_Code_FAILED_OPERATION)
    {
        printf("login Failed\n");
        return 1;
    }
    else
    {
        printf("login Successful\n");
    }

    do {
        //		Enter lines of text
        cin.ignore();
        showMenu();
        cout << "> ";
        getline(cin, userInput);

        //		Send to server
        // int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        // if (sendRes == -1)
        // {
        //     cout << "Could not send to server! Whoops!\r\n";
        //     continue;
        // }

        // //		Wait for response
        // memset(buf, 0, 4096);
        // int bytesReceived = recv(sock, buf, 4096, 0);
        // if (bytesReceived == -1)
        // {
        //     cout << "There was an error getting response from server\r\n";
        // }
        // else
        // {
        //     //		Display response
        //     cout << "SERVER> " << string(buf, bytesReceived) << "\r\n";
        // }
        /*
        switch userInput:
            case 1:
                chat::ConnectedUsers& actives;
                
        */

        if(userInput == "1"){
            //  Send the request to server
            chat::ClientRequest allUsers;
            allUsers.set_option(chat::ClientRequest_Option_CONNECTED_USERS);
            std::string request_serial;
            allUsers.SerializeToString(&request_serial);
            strcpy(buf, request_serial.c_str());
            send(sock, request_serial.c_str(), request_serial.size(), 0);
            
            //  Recieve response
            recv(sock, buf, 4096, 0);
            chat::ServerResponse respuesta;
            respuesta.ParseFromString(buf);
            //cout << respuesta.code() << endl;
            cout << respuesta.DebugString() << '\n';
            if(respuesta.code() != chat::ServerResponse_Code_FAILED_OPERATION){
                //  Print the users
                printf("Usuarios conectados en el servidor: \n");
                // for (int i = 0; i<respuesta.ConnectedUsers().size(); i++){
                //     printf("%s ", respuesta.users().users());
                // }
                // for (chat:: thing : respuesta.users().users()){
                //     cout << thing.name << endl;
                // }
                //for (int i = 0; i < respuesta.users)
                std::cout << respuesta.DebugString() << '\n';
            }
            else{
                printf("Error en el servidor. \n");
            }

        }        
        else if(userInput == "2"){
            user_info="";
            printf("Ingrese el nombre del usuario para saber su informacion: ");
            cin >> user_info;
            chat::ClientRequest *server = new chat::ClientRequest();
            server->set_option(chat::ClientRequest_Option_USER_INFORMATION);

            std::string serialized;
            server->SerializeToString(&serialized);
            strcpy(buf, serialized.c_str());
            send(sock, buf, serialized.size()+1, 0);

            
            
        }              
        else if(userInput == "3"){

            do {
            printf("Ingrese su nuevo estado\n");
            printf("1. Online\n");
            printf("2. Offline\n");
            cin >> status_option;
            
            } while (status_option != "1" && status_option != "2");

            if(status_option == "1"){
                new_status = "Online";
            }
            else {
                new_status = "Offline";
            }

             chat::ClientRequest *server = new chat::ClientRequest;
                server->set_option(chat::ClientRequest_Option_STATUS_CHANGE);
                server->mutable_status() ->set_status(new_status);

                std::string serialized;
                server->SerializeToString(&serialized);

                strcpy(buf, serialized.c_str());
                send(sock, buf, serialized.size()+1, 0);

                recv(sock, buf, 4096, 0);
                chat::ServerResponse response;
                response.ParseFromString(buf);
                if(response.code() != chat::ServerResponse_Code_FAILED_OPERATION)
                {
                    printf("Status change Failed\n");
                    return 1;
                }
                else
                {
                    printf("Status change was Successful\n");
                }
                
                


        }//Para mensajes generales,name es el username
        else if(userInput == "4"){

            general_message = "";
            printf("Ingrese el mensaje general que desea mandar: ");
            cin >> general_message;

            chat::ClientRequest *server = new chat::ClientRequest;
            server->set_option(chat::ClientRequest_Option_SEND_MESSAGE); 

            chat::Message* new_message = server->mutable_messg();
            new_message->set_receiver("all");
            new_message->set_sender(name);
            new_message->set_text(general_message);

            std::string serialized;
            server->SerializeToString(&serialized);
            
            strcpy(buf, serialized.c_str());
            send(sock, buf, serialized.size()+1, 0);

            
            
        }  
        else if(userInput == "5"){
            direct_message = "";


            printf("\nIngrese el nombre del usuario al que desea mandar mensaje: ");
            cin >> direct_user;
            printf("\nIngrese el mensaje a mandar: ");
            cin >> direct_message;

            chat::ClientRequest *server = new chat::ClientRequest;
            server->set_option(chat::ClientRequest_Option_SEND_MESSAGE); 

            chat::Message* new_message = server->mutable_messg();
            new_message->set_receiver(direct_user);
            new_message->set_sender(name);
            new_message->set_text(direct_message);

            std::string serialized;
            server->SerializeToString(&serialized);

            strcpy(buf, serialized.c_str());
            send(sock, buf, serialized.size()+1, 0);

        }
        
    } while(userInput != "7");

    //	Close the socket
    close(sock);

    return 0;
}

