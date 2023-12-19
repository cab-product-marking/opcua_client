#include <iostream>

#include "cab_client.h"

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
    CAB_Client *cab_client = new CAB_Client();

    int state = cab_client->connect(argc, argv);
    if(state != EXIT_SUCCESS)
    {
        /* Clean up */
        delete cab_client;
        cab_client = nullptr;
        return EXIT_FAILURE;
    }

    while(running)
    {
        /* Workbench */
        cab_client->run_iterate();

    }

    cab_client->disconnect();

    /* Clean up */
    delete cab_client;
    cab_client = nullptr;
    return EXIT_SUCCESS;
}