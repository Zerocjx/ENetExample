#include <enet/enet.h>
#include <cstdio>

int main()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    ENetHost* server;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (server == nullptr)
    {
        fprintf(stderr, "An error occured while trying to create an ENet server host.\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    while (1)
    {
        while (enet_host_service(server, &event, 1000) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    printf("[Connected] A new client connected from %x:%u.\n",
                            event.peer->address.host,
                            event.peer->address.port);
                    event.peer->data = "Client information";
                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    printf("[Receive] A packet of length %zu containing \"%s\" was received from %s on channel %u.\n",
                            event.packet->dataLength,
                            event.packet->data,
                            (char*)event.peer->data,
                            event.channelID);
                    enet_packet_destroy(event.packet);
                    break;
                }
                
                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    printf("[Disconnected] %s disconnected.\n", (char*)event.peer->data);
                    event.peer->data = nullptr;
                }
            }
        }
    }

    enet_host_destroy(server);
    return 0;
}