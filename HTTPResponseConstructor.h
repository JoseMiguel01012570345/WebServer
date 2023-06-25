#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTML_DOCUMENT_START "<!DOCTYPE html><html lang = \"en\"><head><meta charset = \"UTF-8\"><meta http - equiv = \"X-UA-Compatible\" content = \"IE=edge\"><meta name = \"viewport\" content = \"width=device-width, initial-scale=1.0\"><link rel=\"stylesheet\" type=\"text/css\" href=\"src/css/styles.css\"><title>WebServer</title></head><body><div><h1 id=\"titulo\">My WebServer</h1></div>"
#define HTML_DOCUMENT_END "</body></html>"
#define CSS_STYLE_BODY "*{background-color: #188accdd;text-decoration: none;font-family: 'Franklin Gothic Medium', 'Arial Narrow', Arial, sans-serif;padding: 1px;color: white;}"
#define CSS_STYLE_TITLE "#titulo {text-align: center;padding: 1px;width: 100%; font-size:50px;}"
#define CSS_STYLE_NAME "#name {font - weight : 50px;padding : 10px;background - color : rgb(21, 51, 114);}"
#define CSS_STYLE_SIZE "#size {font - weight : 50px;padding : 10px;background - color : rgba(26, 91, 232, 0.612);}"
#define CSS_STYLE_ITEMS "tr{text - align : center;}.nombre{padding:1px;width:40%;text - align : center;color:aliceblue; font-size: 40px;}.td,a{padding:5px;text - align : center;background - color : rgb(21, 51, 114); font-size:25px;}"
#define CSS_STYLE_TABLE "table {height : auto;width : 100% ; }"
#define MAX_SIZE_RESPONSE 1048576

int IsCssStyle(char *url)
{
    int start_pos = 0;
    for (int i = 0; i < strlen(url); i++)
        if (url[i] == '/')
            start_pos = i;
    for (int i = start_pos; i < strlen(url); i++)
    {
        if (url[i] == '.' && strlen(url) - i == 4 && url[i + 1] == 'c' && url[i + 2] == 's' && url[i + 3] == 's')
            return 0;
    }
    return -1;
}

char *GetFileName(char *url)
{
    char *copy = malloc(sizeof(url));
    int name_start = 0;
    for (int i = 0; i < strlen(url); i++)
    {
        if (url[i] == '\\')
            name_start = i;
    }
    copy = strcpy(copy, url);
    copy += name_start + 1;
    return copy;
}

char *ListDirResponse(char *content)
{
    char *response = (char *)malloc(MAX_SIZE_RESPONSE - strlen(HTML_DOCUMENT_START) - strlen(HTML_DOCUMENT_END));
    char *response_content = (char *)malloc(MAX_SIZE_RESPONSE);
    response_content = strcpy(response_content, HTML_DOCUMENT_START);
    response_content = strcat(response_content, content);
    response_content = strcat(response_content, HTML_DOCUMENT_END);
    sprintf(response,
            "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\nLink: src/css/styles.css; rel=shorturl\r\n\r\n %s%s%s", strlen(response_content), HTML_DOCUMENT_START, content, HTML_DOCUMENT_END);
    content = strcpy(content, response);
    free(response);
    free(response_content);
    return content;
}

char *DownloadFileResponse(char *url, char *content, long size, char *Server_Root)
{
    char *response = (char *)malloc(sizeof(char) * 1024);
    char *file_name = (char *)malloc(sizeof(char) * 256);
    file_name = strcpy(file_name, GetFileName(url));
    if (IsCssStyle(url) == 0)
    {
        int css_size = sizeof(CSS_STYLE_BODY) + sizeof(CSS_STYLE_TITLE) + sizeof(CSS_STYLE_NAME);
        css_size += sizeof(CSS_STYLE_SIZE) + sizeof(CSS_STYLE_ITEMS) + sizeof(CSS_STYLE_TABLE);
        sprintf(response,
                "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/css; charset=utf-8\r\nVary: Accept-Encoding\r\nContent-Length: %i\r\n\r\n", css_size);
        response = strcat(response, CSS_STYLE_BODY);
        response = strcat(response, CSS_STYLE_TITLE);
        response = strcat(response, CSS_STYLE_NAME);
        response = strcat(response, CSS_STYLE_SIZE);
        response = strcat(response, CSS_STYLE_ITEMS);
        response = strcat(response, CSS_STYLE_TABLE);
    }
    else
    {
        sprintf(response,
                "HTTP/1.1 200 OK\r\nContent-Type: application/octet_stream\r\nContent-Disposition: attachment; filename=\"%s\"\r\nContent-Length: %ld\r\n\r\n", file_name, size);
    }
    free(file_name);
    content = strcpy(content, response);
    free(response);
    return content;
}