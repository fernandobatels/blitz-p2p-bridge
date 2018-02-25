/**
 * Blitz P2P Bridge - Tunnel TCP for p2p streams
 *
 * This server provides a means for two, or more, app to
 * communicate continuously over TCP. Basically when app A write
 * on channel the app B receive the data and viceversa of course.
 *
 * Copyright 2018 Luis Fernando Batels <luisfbatels@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "api.h"

int for_test()
{
    puts("Testing!");

    return 0;
}

bool start_server(int port)
{
    struct sockaddr_in server_addr;
    int server;
    int max_wait_size = 4;

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((server = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket\n");
        return false;
    }

    if (bind(server, &server_addr, sizeof(server_addr)) < 0) {
        perror("Could not bind socket\n");
        return false;
    }

    if (listen(server, max_wait_size) < 0) {
        perror("Could not listen socket\n");
        return false;
    }

    printf("Starting server...\n");

    while (true) {

        int client;
        struct sockaddr_in client_addr;
        int client_addr_len = 0;

        if ((client = accept(server, &client_addr, &client_addr_len)) < 0) {
            perror("Could not accept client\n");
            return false;
        }

        struct client_infos *c;
        pthread_t tid;

        c = malloc(sizeof(struct client_infos));
        inet_ntop(AF_INET, &client_addr.sin_addr, c->addr, INET_ADDRSTRLEN);
        c->socket = client;

        if (pthread_create(&tid, NULL, on_client_conn, (void *) c) < 0) {
            perror("Could not create thread for client");
            return false;
        }

    }

    printf("Closing server...\n");

    close(server);

    return true;
}

void *on_client_conn(void *vargp)
{
    struct client_infos *c = (struct client_infos*) vargp;
    int len_received = 0;
    char buffer_received[MAX_CLIENT_IO];

    printf("Connection client %s...\n", c->addr);

    {//For test
        char buff_test[] = "Hi. Testing...\n";
        send(c->socket, buff_test, strlen(buff_test), 0);
    }

    while ((len_received = recv(c->socket, buffer_received, MAX_CLIENT_IO, 0)) > 0) {
        printf("Received: %d '%s'\n", len_received, buffer_received);
    }

    printf("Closing client %s...\n", c->addr);

    close(c->socket);
}
