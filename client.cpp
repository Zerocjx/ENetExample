#include <enet/enet.h>
#include <cstdio>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

std::string get_current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

bool isRunning = true;


int main()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost* client;
    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr)
    {
        fprintf(stderr, "No avaiable peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection to 127.0.0.1:1234 succeeded.");
        // std::string info = "packet data";
        // ENetPacket* packet = enet_packet_create(info.c_str(), info.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        // enet_packet_resize(packet, info.length() + 4);
        // strcpy((char*)&packet->data[info.length()], "foo");
        // enet_peer_send(peer, 0, packet);
        // enet_host_flush(client);
    }
    else
    {
        isRunning = false;
        enet_peer_reset(peer);
        puts("Connection to 127.0.0.1:1234 failed.");
    }

    // Game Loop
    while (isRunning)
    {
        while (enet_host_service(client, &event, 1000) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    printf("[Receive]\tA packet of length %zu containing \"%s\" was received from %s on channel %u.\n",
                            event.packet->dataLength,
                            event.packet->data,
                            (char*)event.peer->data,
                            event.channelID);
                    enet_packet_destroy(event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    puts ("Server disconnected.");
                    isRunning = false;
                    break;
                }
            }
        }

        if (isRunning)
        {
            std::string info = get_current_time_and_date();
            ENetPacket* packet = enet_packet_create(info.c_str(), info.length() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_packet_resize(packet, info.length() + 4);
            strcpy((char*)&packet->data[info.length()], " Ok");
            enet_peer_send(peer, 0, packet);
            enet_host_flush(client);
        }
    }

    enet_peer_disconnect(peer, 0);
    while (enet_host_service(client, &event, 3000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy (event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            puts ("Disconnection succeeded.");
            return EXIT_SUCCESS;
        }
    }
    

    enet_host_destroy(client);
    return 0;
}