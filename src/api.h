/**
 * Blitz P2P Bridge - Tunnel TCP for p2p streams
 *
 * This server provides a means for two, or more, app to
 * communicate continuously over TCP. Basically when app A write
 * on channel the app B receive the data and viceversa of course.
 *
 * Copyright 2018 Luis Fernando Batels <luisfbatels@gmail.com>
 */
#include <stdbool.h>
#include <arpa/inet.h>

#define DEFAULT_SERVER_PORT 3055
#define MAX_CLIENT_IO 1024
#define MAX_CLIENTS_WAIT_TO_CONNECT 10
#define MAX_CLIENT_WAITING_LIST 10
#define SIZE_CLIENT_ID 15

struct client_infos {
    char addr[INET_ADDRSTRLEN];
    int socket;
    char id[SIZE_CLIENT_ID + 1];
    char id_partner_expected[SIZE_CLIENT_ID + 1];
    struct client_infos* partner;
    bool closed_by_partner;
};

struct client_infos* waiting_list[MAX_CLIENT_WAITING_LIST];

/**
 * For test
 */
int for_test();

/**
 * Start TCP server
 *
 * @param int port - Port to listen
 *
 * @return bool
 */
bool start_server(int port);

/**
 * Event on client connect on server
 */
void *on_client_conn(void *vargp);
