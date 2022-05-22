#define IP_ADDR "127.0.0.1"
#define PORT 1667
#define HANDSHAKE "22"
#define MAX_CMD_LENGTH 7
#define PROGRAM_VERSION "0.3a"

enum tcp_client_commands {
    POWER       = 0,
    BRIGHTNESS  = 1,
    GET         = 2,
    FADE        = 3,
};

