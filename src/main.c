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
#include "api.h"

int main(int argc, char ** argv)
{

    for (int i = 0; i < 10; i++) {
        forTest();
    }

    return 1;
}
