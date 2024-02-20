#include <iostream>
#include <signal.h>

#include "client.h"

bool running = {true};

static void 
stop_handler(int sign)
{
    running = false;
    return;
}

int main(int argc, char *argv[])
{
    /* Catch ctrl-c */
    signal(SIGINT, stop_handler);

    /* New cab client */
    cab::Client *client = new cab::Client();

    int state = client->connect(argc, argv);
    if(state != EXIT_SUCCESS)
    {
        /* Clean up */
        delete client;
        client = nullptr;
        return EXIT_FAILURE;
    }

    while(running)
    {
        /* Workbench */
        client->run_iterate();

    }

    client->disconnect();

    /* Clean up */
    delete client;
    client = nullptr;
    return EXIT_SUCCESS;
}

/* Eof */