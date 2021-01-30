#include "net.h"


uint8_t temp_data[MAX_PACKET];

#define PACKET_PARSE(packet_type) packet_##packet_type parse_##packet_type(void *packet_data) { packet_##packet_type result; memcpy(&result, packet_data, sizeof(packet_##packet_type)); return result; }

// Connection Setup

TCPsocket server_setup(int port, SDLNet_SocketSet *socket_set) {
    IPaddress bind_ip;
    TCPsocket server_socket;

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

TCPsocket client_connect(const char* server_ip, int server_port, SDLNet_SocketSet *socket_set) {
    TCPsocket socket;
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, server_ip, server_port) != 1) {
        Debug::out << SDLNet_GetError();
    }

    socket = SDLNet_TCP_Open(&ip);
    if (socket == NULL) {
        Debug::out << SDLNet_GetError();
    }

    *socket_set = SDLNet_AllocSocketSet(1);
    if (*socket_set == NULL) {
        Debug::out << SDLNet_GetError();
    }

    if (SDLNet_TCP_AddSocket(*socket_set, socket) != 0) {
        Debug::out << SDLNet_GetError();
    }

    return socket;
}

// Receive data

void* recv_data(TCPsocket &socket, PACKET_TYPE *type) {
    int num_recv = SDLNet_TCP_Recv(socket, temp_data, MAX_PACKET);
    void *packet = temp_data;

    *type = *(PACKET_TYPE*)packet;
    void *packet_data = (EntityUpdate*)((uint8_t*)packet + sizeof(PACKET_TYPE));
    return packet_data;
}

EntityUpdate* parse_state_update(void *packet_data, int *num_entities) {
    uint8_t *packet = (uint8_t*)packet_data;
    *num_entities = *(int*)packet;
    packet += sizeof(int);
    return (EntityUpdate*)packet;
}

PACKET_PARSE(join_lobby)
PACKET_PARSE(join_lobby_response)
PACKET_PARSE(player_ready)
PACKET_PARSE(game_start)
PACKET_PARSE(player_input)

bool send_data(TCPsocket &socket, uint8_t *packet_data, int packet_len) {
    int num_sent = SDLNet_TCP_Send(socket, packet_data, packet_len);

    if (num_sent < packet_len) {
        printf("ER: SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

bool send_join_lobby(TCPsocket &socket) {
    constexpr int packet_size = sizeof(PACKET_TYPE) + sizeof(packet_join_lobby);
    uint8_t packet[packet_size];
    uint8_t *packet_ptr = packet;

    *(PACKET_TYPE*)packet_ptr = JOIN_LOBBY;
    packet_ptr += sizeof(PACKET_TYPE);

    return send_data(socket, packet, packet_size);
}

bool send_join_lobby_response(TCPsocket &socket, int client_id) {
    constexpr size_t packet_size = sizeof(PACKET_TYPE) + sizeof(packet_join_lobby_response);
    uint8_t packet[packet_size];
    uint8_t *packet_ptr = packet;

    *(PACKET_TYPE*)packet_ptr = JOIN_LOBBY_RESPONSE;
    packet_ptr += sizeof(PACKET_TYPE);

    *(int*)packet_ptr = client_id;

    return send_data(socket, packet, packet_size);
}

bool send_player_ready(TCPsocket &socket, int client_id, bool ready) {
    constexpr int packet_size = sizeof(PACKET_TYPE) + sizeof(packet_join_lobby_response);
    uint8_t packet[packet_size];
    uint8_t *packet_ptr = packet;

    *(PACKET_TYPE*)packet_ptr = PLAYER_READY;
    packet_ptr += sizeof(PACKET_TYPE);

    *(bool*)packet_ptr = ready;

    return send_data(socket, packet, packet_size);
}

bool send_entity_data(TCPsocket &socket, EntityUpdate* data, uint16_t num_entities) {
    int offset = 0;
    memcpy(temp_data+offset, &num_entities, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    memcpy(temp_data+offset, data, num_entities * sizeof(EntityUpdate));
    offset += num_entities * sizeof(EntityUpdate);

    return true;
}
