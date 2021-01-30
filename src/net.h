#ifndef __NET_H_
#define __NET_H_

#include <SDL.h>
#include <SDL_net.h>

#define MAX_PACKET 0xFFFFFF
#define MAX_CLIENTS 32

#include <vector>

#include <debug.h>

struct EntityUpdate {
    float x;
    float y;
    int entity_id;
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

inline Lobby lobby;

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

struct packet_lobby_update {

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
    LOBBY_UPDATE,
    PLAYER_READY,
    PLAYER_INPUT,
    GAME_START,
    STATE_UPDATE,
    GAME_END
};

// Connection Setup
TCPsocket server_setup(int port, SDLNet_SocketSet *socket_set);
TCPsocket client_connect(const char* server_ip, int server_port, SDLNet_SocketSet *socket_set);

// Receive data
void* recv_data(TCPsocket &socket, PACKET_TYPE *type);

EntityUpdate* parse_state_update(void *packet_data, int *num_entities); // TODO
packet_join_lobby parse_join_lobby(void *packet_data); // TODO
packet_join_lobby_response parse_join_lobby_response(void *packet_data); // TODO
packet_player_ready parse_player_ready(void *packet_data); // TODO
packet_game_start parse_game_start(void *packet_data); // TODO
packet_player_input parse_player_input(void *packet_data); // TODO

// Send data
bool send_data(TCPsocket &socket, uint8_t *packet_data, int packet_len);

bool send_join_lobby(TCPsocket &socket);
bool send_join_lobby_response(TCPsocket &socket, int client_id);
bool send_player_ready(TCPsocket &socket, int client_id, bool ready);
bool send_entity_data(TCPsocket &socket, EntityUpdate* data, uint16_t num_entities);

#endif // __NET_H_
