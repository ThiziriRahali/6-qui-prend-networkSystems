// client_handler.c : gestion d'un client (un thread par client)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server_communication.h"


// Même struct que dans server.c
typedef struct {
    int sock;
    struct sockaddr_in addr;
} client_t;
