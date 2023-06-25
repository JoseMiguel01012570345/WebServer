#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "HTMLConstructor.h"
#include "HTTPResponseConstructor.h"

#define MAX_SIZE_INFO 10192
#define MAX_CLIENTS 10
#define SERVER_ROOT "."
#define CSS_STYLES_ROOT "/media/yonatan01/1602CB9502CB77EF/University/3er/SOperativo/WebServer/wwwroot/src/css/styles.css"

// struct with the info of one server
typedef struct
{
    int socket_descriptor;
    int Domain_Family;
    int Protocol;
    int Addr;
    int Port;
    struct sockaddr_in server;
} Server;

// method to bind a server
int BindServer(Server *server)
{
    if (bind(server->socket_descriptor, (struct sockaddr *)&server->server, sizeof(server->server)) < 0)
    {
        system("fuser -k 8888/tcp");
    }
    printf("Bind Done\n");
    return 0;
}
// method that create a server
int CreateServer(Server *server, int port)
{
    // set the domain of the socket
    server->Domain_Family = AF_INET;
    // set the protocol
    server->Protocol = SOCK_STREAM;
    // create the socket
    server->socket_descriptor = socket(server->Domain_Family, server->Protocol, 0);
    if (server->socket_descriptor == -1)
    {
        printf("Can't create the socket");
        return 1;
    }
    printf("Socket Created\n");
    // we set the parameters of the server
    server->server.sin_family = server->Domain_Family;
    server->server.sin_addr.s_addr = INADDR_ANY;
    server->Port = port;
    server->server.sin_port = htons(port);
    return 0;
}

// method that remove the caracter %20 from the url
char *ParseWhiteSpace(char *url)
{
    char *result = (char *)malloc(strlen(url));
    result = strcpy(result, "");
    for (int i = 0; i < strlen(url); i++)
    {
        if (url[i] == '%')
        {
            result = strncat(result, url, i);
            result = strcat(result, " ");
            url += i + 3;
            i = 0;
        }
    }
    result = strcat(result, url);
    url = strcpy(url, result);
    free(result);
    return url;
}

char *BuildURL(char *url, char *Server_Root)
{
    if (strcmp(url, "/") == 0)
        return Server_Root;
    return url;
}

// method that get the response of open a directory
char *Get_Open_Directory_Response(char *url, char *response, DIR *dir_url)
{
    response = strcpy(response, MakeListContent(url, response, dir_url));
    response = strcpy(response, ListDirResponse(response));
    return response;
}

int SendFile(int Client_Sock, char *url, char *Server_Root)
{
    int file = open(url, O_RDONLY);
    off_t sent = 0;
    struct stat stat_buffer;
    fstat(file, &stat_buffer);
    char *response = (char *)malloc(sizeof(char) * 1024);
    response = strcpy(response, DownloadFileResponse(url, response, stat_buffer.st_size, Server_Root));
    // enviar response
    send(Client_Sock, response, strlen(response), 0);
    sendfile(Client_Sock, file, &sent, stat_buffer.st_size);
    close(file);
    free(response);
}

// method that generate a response to the client
int ResponseServer(int Client_Sock, char *request, char *Server_Root, char *response_buffer)
{
    char *url = malloc(sizeof(char) * 256);
    sscanf(request, "GET %s ", url);
    url = strcpy(url, ParseWhiteSpace(url));
    url = strcpy(url, BuildURL(url, Server_Root));
    if (IsDir(url) == 0)
    {
        DIR *dir_url;
        dir_url = opendir(url);
        response_buffer = strcpy(response_buffer, Get_Open_Directory_Response(url, response_buffer, dir_url));
        send(Client_Sock, response_buffer, strlen(response_buffer), 0);
        closedir(dir_url);
        // free(dir_url);
    }
    else if (IsFile(url) == 0)
    {
        if (IsCssStyle(url) == 0)
        {
            url = strcpy(url,CSS_STYLES_ROOT);
        }
        SendFile(Client_Sock, url, Server_Root);
    }
    free(url);
}

// method that runs the server
void RunServer(Server *server, int port, int Max_Clients, char *Server_Root)
{
    char response[MAX_SIZE_RESPONSE];
    // create the server
    int succes = CreateServer(server, port);
    // bind the server
    if (succes == 0)
        succes = BindServer(server);
    if (succes == 0)
    {
        // run the server
        int SocketLen_T, Read_Size, Client;
        struct sockaddr_in client;
        char Client_Message[MAX_SIZE_INFO];
        // we set the max number of clients that can connect to the server
        listen(server->socket_descriptor, Max_Clients);
        printf("Waiting for incoming connections ...\n");
        SocketLen_T = sizeof(struct sockaddr_in);
        // start the array of process
        int process_len = MAX_CLIENTS * 10;
        pid_t process[process_len];
        //-1 indicates that is free
        for (int i = 0; i < process_len; i++)
            process[i] = -1;
        // main loop of the server
        while (1)
        {
            // accept a connection
            Client = accept(server->socket_descriptor, (struct sockaddr *)&client, (socklen_t *)&SocketLen_T);

            if (Client < 0)
                perror("Accept Failed");
            Read_Size = recv(Client, Client_Message, MAX_SIZE_INFO, 0);
            if (Read_Size < 0)
                perror("Recv failed");
            else
            {
                pid_t p;
                p = fork();
                int success;
                if (p == 0)
                {
                    success = ResponseServer(Client, Client_Message, Server_Root, response);
                    exit(0);
                }
                // ponemos el processo en el array de procesos en ejecucion
                int ready = 0;
                for (int i = 0; i < process_len; i++)
                {
                    if (process[i] == -1 && ready == 0)
                    {
                        process[i] = p;
                        ready = 1;
                    }
                    if (waitpid(process[i], NULL, WNOHANG) == process[i])
                    {

                        process[i] = -1;
                    }
                }
            }
            close(Client);
        }
    }
}