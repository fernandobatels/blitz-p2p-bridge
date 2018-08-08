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
#include <unistd.h>
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

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((server = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Could not create socket\n");
        return false;
    }

    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Could not bind socket\n");
        return false;
    }

    if (listen(server, MAX_CLIENTS_WAIT_TO_CONNECT) < 0) {
        perror("Could not listen socket\n");
        return false;
    }

    event_log(0, "Server started", MS_SERVER);

    while (true) {

        int client;
        struct sockaddr_in client_addr;
        int client_addr_len = 0;

        if ((client = accept(server, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Could not accept client\n");
            return false;
        }

        struct client_infos *c;
        pthread_t tid;

        c = malloc(sizeof(struct client_infos));
        inet_ntop(AF_INET, &client_addr.sin_addr, c->addr, INET_ADDRSTRLEN);
        c->socket = client;

        if (pthread_create(&tid, NULL, on_client_conn, (void *) c) < 0) {
            perror("Could not create thread for client\n");
            return false;
        }

    }

    event_log(0, "Server closed", MS_SERVER);

    close(server);

    return true;
}

void *on_client_conn(void *vargp)
{
    struct client_infos *c = (struct client_infos*) vargp;
    int len_received = 0;
    char buffer_received[MAX_CLIENT_IO + 1];

    event_log(c, "Connection started", MS_CLIENT);

    c->partner_connected = false;
    bool id_valid = false;

    while ((len_received = recv(c->socket, buffer_received, MAX_CLIENT_IO, 0)) > 0) {
        //First step: Identify the client

        buffer_received[len_received] = 0;

        if (buffer_received[len_received - 1] == '\n')
            buffer_received[len_received - 2] = '\0';

        if (strlen(buffer_received) != SIZE_CLIENT_ID) {
            perror("Client sent invalid id. Aborting connection..\n");
            break;
        }

        strcpy(c->id, buffer_received);

        event_log(c, "Client id defined", MS_CLIENT);

        id_valid = true;
        break;
    }

    if (id_valid) {

        id_valid = false;

        while ((len_received = recv(c->socket, buffer_received, MAX_CLIENT_IO, 0)) > 0) {
            //Second step: Identify/Connect the partner

            buffer_received[len_received] = 0;

            if (buffer_received[len_received - 1] == '\n')
                buffer_received[len_received - 2] = '\0';

            if (strlen(buffer_received) != SIZE_CLIENT_ID) {
                perror("Client sent invalid partner id. Aborting connection..\n");
                break;
            }

            strcpy(c->id_partner_expected, buffer_received);

            bool need_waiting_list = true;

            //Checking if other client is waiting for him
            for (int i = 0; i < MAX_CLIENT_WAITING_LIST; i++) {
                if (waiting_list[i]) {
                    if (strcmp(c->id, waiting_list[i]->id_partner_expected) == 0) {

                        event_log(c, "Is partner of", MS_CLIENT_PARTNER);

                        c->partner = waiting_list[i];
                        waiting_list[i]->partner = c;
                        c->partner_connected = true;
                        waiting_list[i]->partner_connected = true;

                        need_waiting_list = false;
                        waiting_list[i] = 0;

                        // Warns them that they are connected
                        char* sent = "serversay:partner-connected\n";
                        send(c->socket, sent, strlen(sent), 0);
                        send(c->partner->socket, sent, strlen(sent), 0);

                        break;
                    }
                }
            }

            //Without partner him go to waiting list
            if (need_waiting_list) {

                event_log(c, "Is waiting for", MS_CLIENT_PARTNER);

                for (int i = 0; i < MAX_CLIENT_WAITING_LIST; i++) {
                    if (!waiting_list[i]) {
                        waiting_list[i] = c;
                        break;
                    }
                }
            }

            id_valid = true;
            break;
        }

        if (id_valid) {
            while ((len_received = recv(c->socket, buffer_received, MAX_CLIENT_IO, 0)) > 0) {

                buffer_received[len_received] = 0;

                if (c->partner_connected) {
                    char sent[30];
                    sprintf(sent, "Sent %d bytes to", len_received);

                    event_log(c, sent, MS_CLIENT_PARTNER);

                    send(c->partner->socket, buffer_received, len_received, 0);

                } else {
                    char sent[60];
                    sprintf(sent, "Sent %d bytes, but he does not have a partner", len_received);

                    event_log(c, sent, MS_CLIENT);
                }

            }
        }

    }

    if (!c->closed_by_partner) {

        event_log(c, "Connection terminated", MS_CLIENT);

        close(c->socket);

        // If the client informed the expected partner
        if (strlen(c->id_partner_expected) > 0) {

            // Removing client from wainting list if his partner did not arrive
            for (int i = 0; i < MAX_CLIENT_WAITING_LIST; i++) {
                if (waiting_list[i]) {
                    if (strcmp(c->id, waiting_list[i]->id_partner_expected) == 0) {
                        waiting_list[i] = 0;
                        break;
                    }
                }
            }

            // When client partner is linked
            if (c->partner) {

                event_log(c, "Partner connection terminated", MS_CLIENT);

                c->partner->closed_by_partner = true;
                close(c->partner->socket);
            }

        }

    }


    free(c);
}

void event_log(struct client_infos* client, char msg[], int typeMsg)
{
    time_t timer;
    char buff_timer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buff_timer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    switch (typeMsg) {
        case MS_SERVER:
            printf("%s %s\n", buff_timer, msg);
        break;
        case MS_CLIENT:
            printf("%s %s[%s] %s\n", buff_timer, client->addr, client->id, msg);
        break;
        case MS_CLIENT_PARTNER:
            printf("%s %s[%s] %s [%s]\n", buff_timer, client->addr, client->id, msg, client->id_partner_expected);
        break;
    }

    fflush(stdout);
}
