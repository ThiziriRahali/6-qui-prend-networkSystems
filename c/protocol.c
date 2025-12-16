#include "global.h"

static inline int Protocol_SendMessage(int sock, const char *format, ...) {
    if (sock == -1) return -1;

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    strcat(buffer, "\n");

    ssize_t sent = send(sock, buffer, strlen(buffer), 0);
    if (sent == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

static inline int Protocol_RecvMessage(int sock, char *buffer, int size) {
    if (sock == -1 || !buffer || size <= 0) return -1;

    ssize_t n = recv(sock, buffer, size - 1, 0);
    if (n <= 0) return -1;

    buffer[n] = '\0';

    if (buffer[n - 1] == '\n') {
        buffer[n - 1] = '\0';
    }

    return 0;
}

