#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server_communication.h"

typedef struct {
    int sock;
    struct sockaddr_in addr;
} client_t;
