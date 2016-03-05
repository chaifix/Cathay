#include "cathay.h"
#include <stdio.h>

#define BUFFER_SIZE 2048

ct_Server* ct_newserver(uint16 portnumber)
{
    tk_init(); 

    ct_Server* server; 
    server = malloc(sizeof(ct_Server));
    memset(server, 0, sizeof(server));
    tk_IPaddress ip = {
        0,
        portnumber
    };
    server->sk = tk_tcp_open(ip);
    if (server->sk == 0)
    {
        printf("unable to create new server instance"); 
        return 0;
    }
    return server; 
}

int send_file(tk_TCPsocket* sk, FILE *resource)
{
    char send_buf[BUFFER_SIZE];
    while (1)
    {
        memset(send_buf, 0, sizeof(send_buf));       //ª∫¥Ê«Â0
        fgets(send_buf, sizeof(send_buf), resource);
        //  printf("send_buf: %s\n",send_buf);
        if(!tk_tcp_send(sk, send_buf, sizeof(send_buf), 0))
        {
            printf("send() Failed:%d\n", WSAGetLastError());
            return 0;
        }
        if (feof(resource))
            return 0;
    }
}

static int ct_parse(tk_TCPsocket* sk, char* buffer, int bsize)
{
    char method[BUFFER_SIZE]; 
    memset(method, 0, sizeof(method)); 
    
    // get method, might be HEAD, GET, POST(not support)
    int i = 0, j = 0;
    while (!(' ' == buffer[j]) && (i < sizeof(method) - 1))
    {
        method[i] = buffer[j];
        i++; j++;
    }
    method[i] = '\0';   

    if (strcmp(method, "GET") && strcmp(method, "HEAD"))
    {
        tk_tcp_close(sk); 
        printf("invalid request method, espect GET or HEAD but get %s", method);
        return 0; 
    }

    char url[BUFFER_SIZE]; 
    memset(url, 0, sizeof(url)); 
    i = 0;
    while ((' ' == buffer[j]) && (j < sizeof(buffer)))
        j++;
    while (!(' ' == buffer[j]) && (i < bsize - 1) && (j < bsize))
    {
        if (buffer[j] == '/')
            url[i] = '\\';
        else if (buffer[j] == ' ')
            break;
        else
            url[i] = buffer[j];
        i++; j++;
    }
    url[i] = '\0';

    char path[256];

    _getcwd(path, _MAX_PATH);    
    strcat(path, url);

    FILE *resource = fopen(path, "rb");

    if (resource == 0)
    {// 404
        char response[BUFFER_SIZE] = 
            "HTTP/1.1 404 NOT FOUND\r\n"
            "Connection: keep-alive\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<HTML>"
            "<BODY>"
            "cathay error: </b>404 not found."
            "</BODY>"
            "</HTML>"
            ; 
        tk_tcp_send(sk, response, sizeof(response), 0);
        tk_tcp_close(sk);
        return 0;
    }

    fseek(resource, 0, SEEK_SET);
    fseek(resource, 0, SEEK_END);
    long flen = ftell(resource);
    printf("file length: %ld\n", flen);
    fseek(resource, 0, SEEK_SET);

    // 200 ok 
    char response[BUFFER_SIZE] =
        "HTTP/1.1 200 OK\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: %ld\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        ;
    tk_tcp_send(sk, response, sizeof(response), 0);

    if (0 == _stricmp(method, "GET"))
    {
        if (0 == send_file(sk, resource))
            printf("file send ok.\n");
        else
            printf("file send fail.\n");
    }
    fclose(resource);

    tk_tcp_close(sk);

    return 0;

}

int ct_startserver(ct_Server* server)
{
    char buffer[BUFFER_SIZE];
    int len = 0;
    while (1)
    {
        tk_TCPsocket* csk = tk_tcp_accept(server->sk);
        if (csk)
        {
            memset(buffer, 0, sizeof(buffer));
            tk_tcp_recv(csk, buffer, sizeof(buffer), &len);
            if (buffer)
            {
                printf("recv data from client:%s\n", buffer);
                ct_parse(csk, buffer, len);
            }
        }
    }
}

int ct_stopserver(ct_Server* server)
{

}

int ct_closeserver(ct_Server* server)
{
    tk_tcp_close(server->sk);
}