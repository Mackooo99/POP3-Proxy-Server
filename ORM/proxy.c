#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>

#define DEFAULT_LEN 35
#define MAX_CLIENT 5
#define POP3_PORT 110
#define PROXY_PORT 27015
#define ADDR "192.168.1.13"
#define DEFAULT_BUFLEN 1024

typedef struct {
    char username[DEFAULT_LEN];
} blackListT;

blackListT blackList[MAX_CLIENT];
int blackL = 0;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

bool readingBlackList();
bool CheckBlackList(char *);
void *tcpForward(void *);
void *handleClient(void *);

int main() {
    if (!readingBlackList())
        return EXIT_FAILURE;

    int proxySock, clientSock;
    struct sockaddr_in proxy, client;
    socklen_t clilen;

    if ((proxySock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("ERROR opening socket");

    int opt = 1;
    setsockopt(proxySock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    proxy.sin_family = AF_INET;
    proxy.sin_addr.s_addr = INADDR_ANY;
    proxy.sin_port = htons(PROXY_PORT);

    if (bind(proxySock, (struct sockaddr *)&proxy, sizeof(proxy)) < 0)
        error("Error on binding");

    listen(proxySock, 5);

    while (1) {
        clilen = sizeof(client);
        clientSock = accept(proxySock, (struct sockaddr *)&client, &clilen);
        if (clientSock < 0)
            error("Accept failed!");

        pthread_t client_thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = clientSock;
        if (pthread_create(&client_thread, NULL, handleClient, (void *)new_sock) != 0) {
            perror("Error creating thread");
            close(clientSock);
            free(new_sock);
        }
        pthread_detach(client_thread);
    }

    close(proxySock);
    return 0;
}

void *handleClient(void *arg) {
    int clientSock = *((int *)arg);
    free(arg);
    char messageBuffer[DEFAULT_BUFLEN];
    int readSize;

    if (send(clientSock, "email: ", 7, 0) < 0) {
        perror("Send failed");
        close(clientSock);
        return NULL;
    }

    if ((readSize = recv(clientSock, messageBuffer, DEFAULT_BUFLEN - 1, 0)) <= 0) {
        perror("Receive failed");
        close(clientSock);
        return NULL;
    }
    messageBuffer[readSize] = '\0';

    if (CheckBlackList(messageBuffer)) {
        send(clientSock, "You are on the black list\n", 26, 0);
        close(clientSock);
        return NULL;
    }

    int pop3sock = socket(AF_INET, SOCK_STREAM, 0);
    if (pop3sock < 0) {
        perror("ERROR opening POP3 socket");
        close(clientSock);
        return NULL;
    }

    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(POP3_PORT);
    server.sin_addr.s_addr = inet_addr(ADDR);

    if (connect(pop3sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("ERROR connecting to POP3 server");
        close(pop3sock);
        close(clientSock);
        return NULL;
    }

    int sockets_c2s[2] = {clientSock, pop3sock};
    int sockets_s2c[2] = {pop3sock, clientSock};

    pthread_t clientToServerThread, serverToClientThread;
    pthread_create(&clientToServerThread, NULL, tcpForward, (void *)sockets_c2s);
    pthread_create(&serverToClientThread, NULL, tcpForward, (void *)sockets_s2c);

    pthread_join(clientToServerThread, NULL);
    pthread_join(serverToClientThread, NULL);

    close(pop3sock);
    close(clientSock);
    return NULL;
}

void *tcpForward(void *args) {
    int *sockets = (int *)args;
    int from_sock = sockets[0];
    int to_sock = sockets[1];

    char buffer[DEFAULT_BUFLEN];
    int bytesRead;

    while ((bytesRead = recv(from_sock, buffer, DEFAULT_BUFLEN, 0)) > 0) {
        if (send(to_sock, buffer, bytesRead, 0) < 0) {
            perror("Error sending to socket");
            break;
        }
    }

    close(from_sock);
    close(to_sock);
    return NULL;
}

bool readingBlackList() {
    FILE *blackListFile = fopen("crnaLista.txt", "r");
    if (blackListFile == NULL) {
        perror("Error opening blacklist file");
        return false;
    }

    int i = 0;
    while (i < MAX_CLIENT && fscanf(blackListFile, "%34s", blackList[i].username) == 1) {

        blackList[i].username[strcspn(blackList[i].username, "\n")] = 0;
        i++;
    }
    fclose(blackListFile);
    blackL = i;
    return true;
}

bool CheckBlackList(char *username) {
    // Uklanjanje suvišnih razmaka i novih linija sa kraja unosa
    username[strcspn(username, "\n")] = 0;  // Ukloni novi red sa kraja
    username[strcspn(username, "\r")] = 0;
    username[strcspn(username, " ")] = 0;

    for (int i = 0; i < blackL; i++) {
        // Takođe uklanjamo razmake sa kraja imena sa crne liste
        blackList[i].username[strcspn(blackList[i].username, "\n")] = 0;


        // Proveravamo da li je korisničko ime sa crne liste tačno isto kao korisničko ime uneto od strane klijenta
        if (strcmp(username, blackList[i].username) == 0) {
            return true;  // Korisnik je na crnoj listi
        }
    }
    return false;  // Korisnik nije na crnoj listi
}
