#include "Server.h"

#define PORT 8888
#define ROOT "/media/yonatan01/1602CB9502CB77EF/University/3er/SOperativo/WebServer/wwwroot"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Usage:\n./Main <root> <port>where \"root\" is the root where the server will be runing, and \"port\" is the port in use\n");
        Server *server = (Server *)malloc(sizeof(Server));
        RunServer(server, PORT, MAX_CLIENTS, ROOT);
        return 0;
    }
    else if(argc > 3)
    {
        printf("Too many parameters have given\n");
        printf("Usage:\n./Main <root> <port>where \"root\" is the root where the server will be runing, and \"port\" is the port in use\n");
        return -1;
    }
    else if(argc == 2)
    {
        DIR *dir = opendir(argv[1]);
        if(dir == NULL)
        {
            printf("Invalid root %s\n",argv[1]);
            return -1;
        }
        closedir(dir);
        Server *server = (Server *)malloc(sizeof(Server));
        RunServer(server,PORT,MAX_CLIENTS,argv[1]);
        return 0;
    }
    else
    {
        DIR *dir = opendir(argv[1]);
        if (dir == NULL)
        {
            printf("Invalid root %s\n", argv[1]);
            return -1;
        }
        closedir(dir);
        int port = atoi(argv[2]);
        Server *server = (Server *)malloc(sizeof(Server));
        RunServer(server, port, MAX_CLIENTS, argv[1]);
        return 0;
    }
}