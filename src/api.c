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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include "api.h"

int for_test()
{
    puts("Testing!");

    return 0;
}

bool start_server(int port, void (*on_conn)(saddr, int))
{
    saddr server_addr;
    int server;
    int max_wait_size = 4;

    memset(&server_addr, 0, sizeof(saddr));

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
        saddr client_addr;
        int client_addr_len = 0;

        if ((client = accept(server, &client_addr, &client_addr_len)) < 0) {
            perror("Could not accept client\n");
            return false;
        }

        printf("Connectin client %s...\n", inet_ntoa(client_addr.sin_addr));

        (*on_conn)(client_addr, client);

        printf("Closing client %s...\n", inet_ntoa(client_addr.sin_addr));

        close(client);

    }

    printf("Closing server...\n");

    close(server);

    return true;
}

void on_client_conn(saddr client_addr, int client)
{
    char buffer[] = "Hi! :)\n";

    for (int i = 0; i < 10; i++) {
        send(client, buffer, strlen(buffer), 0);
    }
}
