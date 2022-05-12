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
#include <stdio.h>
#include <vector>
#include "register.pb.h"

using namespace std;

int sock;
std::vector<std::string> chat_history;

/**
if(loginresponse.code() == chat::ServerResponse_Code_FAILED_OPERATION)
    {
        printf("No se pudo iniciar sesion\n");
        return 1;
    } 
 */

void *listenToServer(void *args)
{
    while(1){

            char response_buffer[4096];

            chat :: ServerResponse response;
            int *sock_con = (int *)args;
            int bytesReceived = recv(*sock_con, response_buffer, 4096, 0);

            response.ParseFromString(response_buffer);

            // switch (response.option()) {
            
            // case chat::ServerResponse_Option_STATUS_CHANGE:
            //     std::cout << "El usuario " << std::endl;
            //     break;
            // }
            //TODO: Connected users
            if(response.option() == chat::ServerResponse_Option_USER_INFORMATION)
            {
                if(response.code() == chat::ServerResponse_Code_FAILED_OPERATION)
                {
                    printf("No se pudo recuperar los datos del usuario\n");
                }
                else
                {
                    printf("\nUsuario: %s\n", response.user().username().c_str());
                    printf("IP: %s\n", response.user().ip().c_str());
                    printf("Estado: %s\n\n", response.user().status().c_str());
                }
            } 
            else if(response.option() == chat::ServerResponse_Option_STATUS_CHANGE)
            {
                
                if(response.code() == chat::ServerResponse_Code_FAILED_OPERATION)
                {
                    printf("No se pudo cambiar el estado del usuario\n");
                }
                else
                {
                     std::cout << response.DebugString() << std::endl;
                    printf("\nEstado cambiado correctamente\n");
                   
                }
                
                
            }

            else if(response.option() == chat::ServerResponse_Option_SEND_MESSAGE)
            {
                
                if(response.code() == chat::ServerResponse_Code_FAILED_OPERATION)
                {
                    printf("No se pudo recibir mensaje\n");
                }
                else
                {
                    string mensaje_recibido = response.messg().text().c_str();
                    string sender_mensaje = response.messg().sender().c_str();

                    cout<<response.DebugString()<<endl;
                    if(response.messg().receiver() == "all"){
                        printf("\n%s@general: %s\n", response.messg().sender().c_str(), response.messg().text().c_str());
                        //chat_history.push_back(str(response.messg().sender().c_str())+"@general: "+str(response.messg().text().c_str()));

                    }else{
                        printf("\n%s@private: %s\n", response.messg().sender().c_str(), response.messg().text().c_str());
                            //chat_history.push_back(str(response.messg().sender().c_str())+"@private: "+str(response.messg().text().c_str()));

                    
                    }
                   
                }
                
                
            } else if(response.option() == chat::ServerResponse_Option_CONNECTED_USERS)
            {
                int size = response.users().users_size();
                
                for(int i = 0; i<size; i++){
                    chat::UserInformation user = response.users().users(i);
                    cout << "Username: " << user.username().c_str() << endl;
                    cout << "IP: " << user.ip().c_str() << endl;
                    cout << "Estado: : " << user.status().c_str() << endl;
                }
            }
    }
}
void help(){
    printf("\n\n\n");
    printf( "1. Obtener usuarios conectados -> Mostrar una lista con todos los usuarios conectados\r\n");
    printf( "2. Mostrar información de un usuario -> Mostrar el estado, ip y el nombre de un usuario colocando su nombre\r\n");
    printf( "3. Cambiar estatus -> Cambia el estado de tu cuenta: Online u Offline\r\n");
    printf( "4. Enviar un mensaje general -> Envia un mensaje a todos los usuarios conectados\r\n");
    printf( "5. Enviar un mensaje directo -> Envia un mensaje a un usuario en especifico colocando su nombre\n");
    printf( "7. Salir -> Desconectarse del servidor\r\n");
}

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
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return 1;
    }
    //	Create a hint structure for the server we're connecting with
    int port = 54006;
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
        printf("No se pudo iniciar sesion\n");
        return 1;
    }
    else
    {
        printf("Sesion iniciada.\n");
    }
    pthread_t tid;
	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_create(&tid, &attrs, listenToServer, (void *)&sock);

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

            
            chat::ClientRequest *server = new chat::ClientRequest();
            server->set_option(chat::ClientRequest_Option_CONNECTED_USERS);
            

            //  Send the request to server
            std::string request_serial;
            server->SerializeToString(&request_serial);
            strcpy(buf, request_serial.c_str());
            send(sock, buf, request_serial.size()+1, 0);
            
            // //  Recieve response
            // recv(sock, buf, 4096, 0);
            // chat::ServerResponse respuesta;
            // respuesta.ParseFromString(buf);
            // cout << respuesta.DebugString() << '\n';
            // if (respuesta.code() != chat::ServerResponse_Code_FAILED_OPERATION)
            // {
            //     //  Print the users
            //     printf("Usuarios conectados en el servidor: \n");
            //     int size = response.users().users().size();
            //     for(int i = 0; i<size; i++){
            //         auto user = response.users().users(i)
            //         cout << "Username: " << user.username().c_str() << endl;
            //         cout << "IP: " << user.ip().c_str() << endl;
            //         cout << "Estado: : " << user.status().c_str() << endl;
            //     }
            // }
            // else
            // {
            //     printf("Error en el servidor. \n");
            // }

        }        
        else if(userInput == "2"){
            user_info="";
            printf("Ingrese el nombre del usuario para saber su informacion: ");
            cin >> user_info;
            chat::ClientRequest *server = new chat::ClientRequest();
            server->set_option(chat::ClientRequest_Option_USER_INFORMATION);
            server->mutable_user() ->set_user(user_info);

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
                server->mutable_status() ->set_username(name);

                std::string serialized;
                server->SerializeToString(&serialized);

                strcpy(buf, serialized.c_str());
                send(sock, buf, serialized.size()+1, 0);

                // recv(sock, buf, 4096, 0);
                // chat::ServerResponse response;
                // response.ParseFromString(buf);
                // if(response.code() != chat::ServerResponse_Code_FAILED_OPERATION)
                // {
                //     printf("No se pudo cambiar el estado! \n");
                //     return 1;
                // }
                // else
                // {
                //     printf("Estado actualizado correctamente!\n");
                // }
                
                


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

        else if(userInput == "help"){
            help();
        }
        
    } while(userInput != "7");

    //	Close the socket
    close(sock);

    return 0;
}

