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

typedef struct sockaddr_in saddr;

/**
 * For test
 */
int for_test();

/**
 * Start TCP server
 *
 * @param int port - Port to listen
 * @param func on_conn - Function to call on new client connection
 *
 * @return bool
 */
bool start_server(int port, void (*on_conn)(saddr, int));

/**
 * Default action in client connect
 *
 * @param saddr client_addr - Client address
 * @param int client - Client connection
 */
void on_client_conn(saddr client_addr, int client);
