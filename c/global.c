#include "../headers/global.h"

void send_error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

void send_info(const char* message) {
    printf("%s\n", message);
}