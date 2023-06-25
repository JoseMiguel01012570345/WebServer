#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>

// the folders and files to ignore
#define CODE_INFO ".vscode"
#define HTML "HTMLConstructor.h"
#define HTTP "HTTPResponseConstructor.h"
#define MAIN "Main.c"
#define SERVER "Server.h"
#define Main "Main"
#define GIT ".git"
#define SRC "src"

// struct to store the info of the item
typedef struct Item
{
    int Date;
    int Size;
    char *Name;
    char *Struct
}Item;

// struct to store items
typedef struct ListItem
{
    struct Item *value;
    struct ListItem *Next;
} ListItem;

ListItem *Get_Item_To_List(ListItem *list_item, long size, char *name, int date, char *content)
{
    struct Item *item = malloc(sizeof(Item));
    item->Date = date;
    item->Size = size;
    item->Name = malloc(sizeof(name));
    item->Name = strcpy(item->Name, name);
    item->Struct = malloc(sizeof(content));
    item->Struct = strcpy(item->Struct, content);
    list_item->value = item;
    list_item->Next = NULL;
    return list_item;
}

int IsDir(char *url)
{
    if (strcmp(url, "/") == 0)
        return 0;
    struct stat stat_buffer;
    stat(url, &stat_buffer);
    if ((stat_buffer.st_mode & __S_IFMT) == __S_IFDIR)
        return 0;
    return -1;
}

int IsFile(char *url)
{
    struct stat stat_buffer;
    stat(url, &stat_buffer);
    if ((stat_buffer.st_mode & __S_IFMT) != __S_IFDIR)
        return 0;
    return -1;
}
// method that decide what file ignore
int Ignore(char *item)
{
    char *items[] = {CODE_INFO, HTML, HTTP, MAIN, SERVER, Main, GIT,SRC};
    for (int i = 0; i < 7; i++)
        if (strcmp(item, items[i]) == 0)
            return 1;
    return 0;
}
// this method gets the size of a folder recursivly
long Get_Folder_Size(char *url)
{
    struct stat stat_buffer;
    stat(url, &stat_buffer);
    if (IsFile(url) == 0)
        return stat_buffer.st_size;
    else if (IsDir(url) == 0)
    {
        long size = 0;
        DIR *dir = opendir(url);
        struct dirent *dfd;
        char *file = malloc(1024);
        int a = 2;
        while ((dfd = readdir(dir)) != NULL)
        {
            if (a > 0)
            {
                a--;
                continue;
            }
            else
            {
                file = strcpy(file, url);
                file = strcat(file, "/");
                file = strcat(file, dfd->d_name);
                size += Get_Folder_Size(file);
            }
        }
        closedir(dir);
        return size;
    }
    else
        return 0;
}

char *Get_Info_Item(long size, char *result, char *modification_date)
{
    int times = 0;
    while (size > 1024)
    {
        times++;
        size /= 1024;
    }
    switch (times)
    {
    case 1:
        sprintf(result, "</a></td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%ld Kbytes</td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%s</td></tr>", size, modification_date);
        break;
    case 2:
        sprintf(result, "</a></td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%ld Mbytes</td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%s</td></tr>", size, modification_date);
        break;
    case 3:
        sprintf(result, "</a></td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%ld Gbytes</td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%s</td></tr>", size, modification_date);
        break;
    case 0:
        sprintf(result, "</a></td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%ld bytes</td><td class=\"td\">&nbsp&nbsp&nbsp&nbsp%s</td></tr>", size, modification_date);
        break;
    }
    return result;
}

void MakeItems(char *url, char *dest, DIR *dir)
{
    struct dirent *dfd;
    int a = 2;
    char date[70];
    struct stat stat_buffer;
    struct tm *m_time;
    char *file = (char *)malloc(sizeof(char) * 256);
    while ((dfd = readdir(dir)) != NULL)
    {
        if (a > 0)
        {
            a--;
            continue;
        }
        else
        {
            if (Ignore(dfd->d_name) == 0)
            {
                dest = strcat(dest, "<tr><td class=\"td\"><a href=\"");
                dest = strcat(dest, url);
                dest = strcat(dest, "/");
                dest = strcat(dest, dfd->d_name);
                dest = strcat(dest, "\">");
                dest = strcat(dest, dfd->d_name);
                file = strcpy(file, url);
                file = strcat(file, "/");
                file = strcat(file, dfd->d_name);
                long size = Get_Folder_Size(file);
                stat(file, &stat_buffer);
                time_t time = stat_buffer.st_mtime;
                m_time = localtime(&time);
                char *format = "%Y-%m-%d %H:%M:%S";
                strftime(date, sizeof(date), format, m_time);
                dest = strcat(dest, Get_Info_Item(size, file, date));
            }
        }
    }
}
// method to build the html content of the page, the url most exists and most be a folder
char *MakeListContent(char *url, char *html_content, DIR *dir_url)
{
    html_content = strcpy(html_content, "<table id=\"table\"><tr class=\"nombre\"><th class=\"nombre\" id=\"name\">Name</th><th class=\"nombre\" id=\"size\">Size</th><th class=\"nombre\" id=\"name\">Last Modification Date</th></tr>");
    MakeItems(url, html_content, dir_url);
    html_content = strcat(html_content, "</table>");
    return html_content;
}