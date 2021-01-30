#ifndef __NET_H_
#define __NET_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define MAX_PACKET 0xFFFFFF
#define MAX_CLIENTS 32

#include <vector>

#include <debug.h>

uint8_t temp_data[MAX_PACKET];

struct Entity {
    float x;
    float y;
    float w;
    float h;
    int sprite;
};

struct Client {
    bool in_use;
    bool ready;
    TCPsocket socket;
};

struct Lobby {
    std::string id;
    std::array<Client, MAX_CLIENTS> clients;
};

enum INPUT {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ACTION
};

enum PLAYER_TYPE {
    DICTATOR,
    WANNABE_NPC
};

struct packet_join_lobby {
    char lobby_id[6];
    char player_name[20];
};

struct packet_join_lobby_response {
    int client_id;
};

struct packet_player_ready {
    int client_id;
    bool ready;
};

struct packet_game_start {
    PLAYER_TYPE player_type;
    // TODO: Buildings
};

struct packet_player_input {
    int client_id;
    INPUT inputs[4];
};

enum PACKET_TYPE {
    JOIN_LOBBY,
    JOIN_LOBBY_RESPONSE,
    PLAYER_READY,
    PLAYER_INPUT,
    GAME_START,
    STATE_UPDATE,
    GAME_END
};

TCPsocket server_setup(int port, SDLNet_SocketSet *socket_set) {
    IPaddress bind_ip;
    TCPsocket server_socket;
    SDLNet_SocketSet socket_set;

    if (SDLNet_ResolveHost(&bind_ip, NULL, port) != 0) {
        Debug::out << SDLNet_GetError();
    }

    server_socket = SDLNet_TCP_Open(&bind_ip);
    if (server_socket == NULL) {
        Debug::out << SDLNet_GetError();
    }

    *socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS);
    if (*socket_set == NULL) {
        Debug::out << SDLNet_GetError();
    }

    if (SDLNet_TCP_AddSocket(*socket_set, server_socket) != 0) {
        Debug::out << SDLNet_GetError();
    }

    return server_socket;
}

bool send_data(TCPsocket &socket, uint8_t *packet_data, int packet_len) {
    int num_sent = SDLNet_TCP_Send(socket, packet_data, packet_len);

    if (num_sent < packet_len) {
        printf("ER: SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

void* recv_data(TCPsocket &socket, PACKET_TYPE &type) {
    int num_recv = SDLNet_TCP_Recv(socket, temp_data, MAX_PACKET);
    void *packet = temp_data;

    *type = *(PACKET_TYPE*)packet;
    void *packet_data = (Entity*)((uint8_t*)packet + sizeof(PACKET_TYPE));
    return packet_data;
}

bool send_join_lobby(TCPsocket &socket) {
    int packet_size = sizeof(PACKET_TYPE) + sizeof(packet_join_lobby);
    uint8_t packet[packet_size];
    uint8_t *packet_ptr = packet;

    *(PACKET_TYPE*)packet_ptr = JOIN_LOBBY;
    packet_ptr += sizeof(PACKET_TYPE);

    return send_data(socket, packet, packet_size);
}

bool send_join_lobby_response(TCPsocket &socket, int client_id) {
    int packet_size = sizeof(PACKET_TYPE) + sizeof(packet_join_lobby_response);
    uint8_t packet[packet_size];
    uint8_t *packet_ptr = packet;

    *(PACKET_TYPE*)packet_ptr = JOIN_LOBBY_RESPONSE;
    packet_ptr += sizeof(PACKET_TYPE);

    *(int*)packet_ptr = client_id;

    return send_data(socket, packet, packet_size);
}

bool send_player_ready(TCPsocket &socket, int client_id, bool ready) {
    int packet_size = sizeof(PACKET_TYPE) + sizeof(packet_join_lobby_response);
    uint8_t packet[packet_size];
    uint8_t *packet_ptr = packet;

    *(PACKET_TYPE*)packet_ptr = PLAYER_READY;
    packet_ptr += sizeof(PACKET_TYPE);

    *(bool*)packet_ptr = ready;

    return send_data(socket, packet, packet_size);
}

bool send_entity_data(TCPsocket &socket, Entity* data, uint16_t num_entities) {
    int offset = 0;
    memcpy(temp_data+offset, &num_entities, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    memcpy(temp_data+offset, data, num_entities * sizeof(Entity));
    offset += num_entities * sizeof(Entity);

    return true;
}


#endif // __NET_H_
