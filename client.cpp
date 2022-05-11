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
    printf( "Welcome to the chat room!\r\n");
    printf( "1. Obtener usuarios conectados\r\n");
    printf( "2. Mostrar información de un usuario\r\n");
    printf( "3. Cambiar estatus\r\n");
    printf( "4. Enviar un mensaje general\r\n");
    printf( "5. Enviar un mensaje directo\n");
    printf( "6. Salir\r\n");
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
    int port = 54000;
    string ipAddress = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
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
    

    //TODO: Hacer que el primer argumento sea el usuario
    string name = "elpepexd";

    //get the current ip address of the client and store it in a string
    string ip = "";
    char hostname[1024];
    gethostname(hostname, 1024);
    struct hostent *host = gethostbyname(hostname);
    if (host)
    {
        ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
    }



    // chat::ClientRequest setUsername;
    // setUsername.set_option(chat::ClientRequest_Option_USER_LOGIN);

    // chat::UserRegistration* user = setUsername.mutable_newuser();
    // user->set_username("elpepon xd");
    // // user->set_ip("127.1.1.1");
    // setUsername.mutable_newuser() ->set_username("elpepon xd");
    // setUsername.mutable_newuser() ->set_ip("127.1.1.1");

    // std::string login_serialized; 
    // setUsername.SerializeToString(&login_serialized);

    //setUsername.SerializeToString(&login_serialized);

    //strcpy(buf, login_serialized.c_str());
    // send(sock, login_serialized.c_str(), login_serialized.size(), 0);

    //    Receive the response
    // recv(sock, buf, 4096, 0);

    // chat::ServerResponse loginresponse;
    // loginresponse.ParseFromString(buf);

    // if(loginresponse.code() == chat::ServerResponse_Code_FAILED_OPERATION)
    // {
    //     printf("login Failed\n");
    //     return 1;
    // }
    // else
    // {
    //     printf("login Successful\n");
    // }


    do {
        //		Enter lines of text
        showMenu();
        cout << "> ";
        getline(cin, userInput);

        //		Send to server
        int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        if (sendRes == -1)
        {
            cout << "Could not send to server! Whoops!\r\n";
            continue;
        }

        //		Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cout << "There was an error getting response from server\r\n";
        }
        else
        {
            //		Display response
            cout << "SERVER> " << string(buf, bytesReceived) << "\r\n";
        }
    } while(true);

    //	Close the socket
    close(sock);

    return 0;
}

